
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
## Build
### nginx+boringssl
```bash
tag=$(date +%Y%m%d%H%M)
sudo docker buildx build
  --push \
  --platform linux/amd64 \
  --tag nginx-aio:latest \
  --tag nginx-aio:$tag \
  .
```
### nginx+quictls
```bash
tag=$(date +%Y%m%d%H%M)
sudo docker buildx build
  --push \
  --platform linux/amd64 \
  --tag nginx-aio:latest \
  --tag nginx-aio:$tag \
  -f Dockerfile.quictls \
  .
```


### HighLoad
https://www.google.com/search?q=systemd+nginx+10k+per+seccond&sca_esv=a982c7140206c012&sca_upv=1&sxsrf=ADLYWIKn9xZOAiADom_yAmG_lOmB2O35cA%3A1727795091227&ei=kw_8ZrTcBYvXi-gP552gmQk&ved=0ahUKEwj08Ziluu2IAxWL6wIHHecOKJMQ4dUDCA8&uact=5&oq=systemd+nginx+10k+per+seccond&gs_lp=Egxnd3Mtd2l6LXNlcnAiHXN5c3RlbWQgbmdpbnggMTBrIHBlciBzZWNjb25kSMocUL8LWPsacAF4AZABAJgBaKABkAaqAQMzLjW4AQPIAQD4AQGYAgagAoYEwgIKEAAYsAMY1gQYR8ICBxAjGLACGCfCAggQABiABBiiBJgDAOIDBRIBMSBAiAYBkAYIkgcDMy4zoAeJFQ&sclient=gws-wiz-serp
https://gist.github.com/v0lkan/90fcb83c86918732b894
https://stackoverflow.com/questions/61407093/optimal-nginx-config-to-handle-thousands-of-request-within-seconds
https://serverfault.com/questions/652180/nginx-10k-concurrent-connections
https://stackoverflow.com/questions/38310740/10k-concurrent-connections-with-nginx
https://gist.github.com/denji/8359866
https://github.com/openbridge/nginx/tree/master
https://devilslane.com/10000-hls-rtmp-live-video-streaming-servers-with-docker-nginx/
https://www.cyberciti.biz/faq/linux-unix-nginx-too-many-open-files/
https://mtyurt.net/post/docker-how-to-increase-number-of-open-files-limit.html
https://github.com/moby/moby/issues/44547
