#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <openssl/ssl.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

typedef struct {
    ngx_flag_t enable;
} ngx_http_ssl_fp_conf_t;

static ngx_int_t ngx_http_ssl_fp_add_variables(ngx_conf_t *cf);
static void *ngx_http_ssl_fp_create_conf(ngx_conf_t *cf);
static char *ngx_http_ssl_fp_merge_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_http_ssl_fp_ja3_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_ssl_fp_ja4_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);

static ngx_command_t ngx_http_ssl_fp_commands[] = {
    {
        ngx_string("ssl_fingerprint"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ssl_fp_conf_t, enable),
        NULL
    },
    ngx_null_command
};

static ngx_http_module_t ngx_http_ssl_fp_module_ctx = {
    ngx_http_ssl_fp_add_variables,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ngx_http_ssl_fp_create_conf,
    ngx_http_ssl_fp_merge_conf
};

ngx_module_t ngx_http_ssl_fingerprint_module = {
    NGX_MODULE_V1,
    &ngx_http_ssl_fp_module_ctx,
    ngx_http_ssl_fp_commands,
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING
};

static ngx_http_variable_t ngx_http_ssl_fp_vars[] = {
    { ngx_string("ssl_ja3"), NULL, ngx_http_ssl_fp_ja3_variable, 0, 0, 0 },
    { ngx_string("ssl_ja4"), NULL, ngx_http_ssl_fp_ja4_variable, 0, 0, 0 },
    ngx_http_null_variable
};

static ngx_int_t ngx_http_ssl_fp_add_variables(ngx_conf_t *cf) {
    ngx_http_variable_t *var, *v;
    for (v = ngx_http_ssl_fp_vars; v->name.len; v++) {
        var = ngx_http_add_variable(cf, &v->name, v->flags);
        if (var == NULL) {
            return NGX_ERROR;
        }
        var->get_handler = v->get_handler;
        var->data = v->data;
    }
    return NGX_OK;
}

static void *ngx_http_ssl_fp_create_conf(ngx_conf_t *cf) {
    ngx_http_ssl_fp_conf_t *conf;
    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ssl_fp_conf_t));
    if (conf == NULL) {
        return NULL;
    }
    conf->enable = NGX_CONF_UNSET;
    return conf;
}

static char *ngx_http_ssl_fp_merge_conf(ngx_conf_t *cf, void *parent, void *child) {
    ngx_http_ssl_fp_conf_t *prev = parent;
    ngx_http_ssl_fp_conf_t *conf = child;
    ngx_conf_merge_value(conf->enable, prev->enable, 0);
    return NGX_CONF_OK;
}

static void ngx_http_ssl_fp_md5_hex(u_char *dst, u_char *src, size_t len) {
    MD5_CTX md5;
    u_char hash[MD5_DIGEST_LENGTH];
    MD5_Init(&md5);
    MD5_Update(&md5, src, len);
    MD5_Final(hash, &md5);
    ngx_hex_dump(dst, hash, MD5_DIGEST_LENGTH);
}

static ngx_int_t ngx_http_ssl_fp_ja3_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data) {
    SSL *ssl;
    u_char *p, *ja3_str;
    size_t len;
    ngx_str_t ja3_hash;
    
    if (r->connection->ssl == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }
    
    ssl = r->connection->ssl->connection;
    if (ssl == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }
    
    len = 256;
    ja3_str = ngx_pnalloc(r->pool, len);
    if (ja3_str == NULL) {
        return NGX_ERROR;
    }
    
    p = ngx_snprintf(ja3_str, len, "%d,%d,%d", 
                     SSL_version(ssl),
                     SSL_CIPHER_get_id(SSL_get_current_cipher(ssl)),
                     0);
    
    ja3_hash.len = 32;
    ja3_hash.data = ngx_pnalloc(r->pool, ja3_hash.len);
    if (ja3_hash.data == NULL) {
        return NGX_ERROR;
    }
    
    ngx_http_ssl_fp_md5_hex(ja3_hash.data, ja3_str, p - ja3_str);
    
    v->len = ja3_hash.len;
    v->data = ja3_hash.data;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    
    return NGX_OK;
}

static void ngx_http_ssl_fp_sha256_hex(u_char *dst, u_char *src, size_t len, size_t out_len) {
    SHA256_CTX sha256;
    u_char hash[SHA256_DIGEST_LENGTH];
    size_t i, max;
    static const u_char hex[] = "0123456789abcdef";
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, src, len);
    SHA256_Final(hash, &sha256);
    max = out_len / 2;
    if (max > SHA256_DIGEST_LENGTH) max = SHA256_DIGEST_LENGTH;
    for (i = 0; i < max; i++) {
        dst[i * 2] = hex[hash[i] >> 4];
        dst[i * 2 + 1] = hex[hash[i] & 0x0f];
    }
}

static ngx_int_t ngx_http_ssl_fp_ja4_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data) {
    SSL *ssl;
    u_char *ja4_str, *p, cipher_buf[32], ext_buf[32], tmp[64];
    int version, cipher_id;
    const char *ver_str, *alpn_str;
    char direction, sni_flag;
    const unsigned char *alpn;
    unsigned int alpn_len;
    u_char *ext_data;
    size_t ext_len;
    
    if (r->connection->ssl == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }
    
    ssl = r->connection->ssl->connection;
    if (ssl == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }
    
    ja4_str = ngx_pnalloc(r->pool, 64);
    if (ja4_str == NULL) {
        return NGX_ERROR;
    }
    
    version = SSL_version(ssl);
    cipher_id = SSL_CIPHER_get_id(SSL_get_current_cipher(ssl)) & 0xFFFF;
    direction = SSL_is_server(ssl) ? 'i' : 'd';
    sni_flag = (r->headers_in.server.len > 0) ? 'd' : 'i';
    
    switch(version) {
        case 0x0301: ver_str = "10"; break;
        case 0x0302: ver_str = "11"; break;
        case 0x0303: ver_str = "12"; break;
        case 0x0304: ver_str = "13"; break;
        default: ver_str = "00"; break;
    }
    
    SSL_get0_alpn_selected(ssl, &alpn, &alpn_len);
    if (alpn_len == 2 && alpn[0] == 'h' && alpn[1] == '2') {
        alpn_str = "h2";
    } else if (alpn_len == 2 && alpn[0] == 'h' && alpn[1] == '3') {
        alpn_str = "h3";
    } else {
        alpn_str = "00";
    }
    
    ngx_sprintf(tmp, "%04xd", cipher_id);
    ngx_http_ssl_fp_sha256_hex(cipher_buf, tmp, 5, 24);
    
    ext_data = tmp;
    ext_len = 0;
    if (alpn_len > 0 && alpn_len < 32) {
        ngx_memcpy(ext_data + ext_len, alpn, alpn_len);
        ext_len += alpn_len;
    }
    if (r->headers_in.server.len > 0 && r->headers_in.server.len < 32) {
        ngx_memcpy(ext_data + ext_len, r->headers_in.server.data, 
                   ngx_min(r->headers_in.server.len, 32 - ext_len));
        ext_len += ngx_min(r->headers_in.server.len, 32 - ext_len);
    }
    
    if (ext_len > 0) {
        ngx_http_ssl_fp_sha256_hex(ext_buf, ext_data, ext_len, 24);
    } else {
        ngx_memcpy(ext_buf, "000000000000", 12);
    }
    
    p = ja4_str;
    *p++ = 't';
    *p++ = ver_str[0];
    *p++ = ver_str[1];
    *p++ = direction;
    *p++ = sni_flag;
    *p++ = '0';
    *p++ = '1';
    *p++ = '0';
    *p++ = '3';
    *p++ = alpn_str[0];
    *p++ = alpn_str[1];
    *p++ = '_';
    ngx_memcpy(p, cipher_buf, 12);
    p += 12;
    *p++ = '_';
    ngx_memcpy(p, ext_buf, 12);
    p += 12;
    
    v->len = p - ja4_str;
    v->data = ja4_str;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    
    return NGX_OK;
}
