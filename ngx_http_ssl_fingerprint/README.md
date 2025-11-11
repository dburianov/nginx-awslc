# Nginx SSL Fingerprint Module (JA3/JA4)

Minimal nginx module for SSL/TLS fingerprinting compatible with AWS-LC.

## Features
- JA3 fingerprint generation (MD5 hash)
- JA4 fingerprint generation (simplified format)
- Compatible with AWS-LC, OpenSSL, QuicTLS, BoringSSL

## Variables
- `$ssl_ja3` - JA3 fingerprint hash
- `$ssl_ja4` - JA4 fingerprint string

## Configuration
```nginx
http {
    ssl_fingerprint on;

    server {
        listen 443 ssl;

        location / {
            add_header X-JA3 $ssl_ja3;
            add_header X-JA4 $ssl_ja4;
        }
    }
}
```

## Usage in logs
```nginx
log_format fingerprint '$remote_addr - $ssl_ja3 - $ssl_ja4 - $request';
access_log /var/log/nginx/fingerprint.log fingerprint;
```
