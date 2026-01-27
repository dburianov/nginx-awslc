
# Custom Nginx Build with AWS-LC

## Features
- Nginx 1.29.x with AWS-LC SSL library
- HTTP/2 and HTTP/3 (QUIC) support
- ModSecurity WAF
- Lua scripting support
- JA3/JA4 SSL fingerprinting
- Multi-platform: linux/amd64, linux/arm64

## SSL Fingerprinting
Custom module provides:
- `$ssl_ja3` - JA3 fingerprint hash
- `$ssl_ja4` - JA4 fingerprint (format: `t13i00_1301`)

## Nginx Modules
- ModSecurity-nginx
- lua-nginx-module
- ngx_brotli
- nginx-module-vts
- headers-more-nginx-module
- ngx_http_geoip2_module
- And 20+ more modules
## Usage
```bash
docker run -d \
  --name nginx-awslc \
  -p 80:80/tcp \
  -p 443:443/tcp \
  -p 443:443/udp \
  -v /opt/nginx/nginx.conf:/usr/local/nginx/conf/nginx.conf \
  -v /opt/nginx/ssl:/usr/local/nginx/conf/ssl:ro \
  nginx-awslc:latest
```

## Build
### Multi-platform
```bash
docker buildx build \
  --platform linux/amd64,linux/arm64 \
  --tag nginx-awslc:latest \
  --push \
  -f Dockerfile.awslc \
  .
```

### Single platform
```bash
docker buildx build \
  --platform linux/amd64 \
  --tag nginx-awslc:latest \
  --load \
  -f Dockerfile.awslc \
  .
```

## Configuration Example
```nginx
http {
    ssl_fingerprint on;
    
    log_format fingerprint '$remote_addr $ssl_ja3 $ssl_ja4 $request';
    
    server {
        listen 443 ssl http2;
        listen 443 quic;
        
        ssl_certificate /usr/local/nginx/conf/ssl/cert.pem;
        ssl_certificate_key /usr/local/nginx/conf/ssl/key.pem;
        
        location / {
            add_header X-JA3 $ssl_ja3;
            add_header X-JA4 $ssl_ja4;
            add_header Alt-Svc 'h3=":443"; ma=86400';
        }
    }
}
```tc/letsencrypt/live/server_name/privkey.pem:/usr/local/nginx/conf/ssl/server_name/privkey.pem:ro \
  dburianov/nginx-aio:buildx-latest
```

