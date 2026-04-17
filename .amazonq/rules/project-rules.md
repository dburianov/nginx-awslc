# Nginx AWS-LC Project Rules

## Project Overview
Custom Nginx build with AWS-LC SSL library, HTTP/3 support, and JA3/JA4 SSL fingerprinting module.

## Code Style
- Write minimal, efficient code
- Avoid verbose implementations
- Use nginx coding conventions for C modules
- Follow Docker best practices for multi-stage builds

## Nginx Module Development
- Use nginx memory pool allocation (ngx_pnalloc, ngx_pcalloc)
- Avoid segfaults - always check pointers before dereferencing
- Use manual string building instead of sprintf for complex formats
- Test modules in isolated containers before integration

## JA3/JA4 Fingerprinting
- JA3: MD5 hash of TLS parameters
- JA4 format: `t13id0103h2_<cipher_hash>_<ext_hash>` (10 chars + hashes)
- Use SHA256 for hashing, truncate to 12 hex chars
- Handle missing SSL data gracefully

## Docker Build
- Base image: ubuntu:latest
- Multi-platform: linux/amd64, linux/arm64
- Use buildx for cross-platform builds
- Tag format: latest, YYYYMMDDHHMM

## SSL/TLS
- AWS-LC as SSL library (Amazon's cryptographic library)
- Support TLS 1.2, TLS 1.3
- HTTP/3 over QUIC
- ALPN: h2, h3

## Testing
- Always test in container before pushing
- Verify JA3/JA4 output format
- Check for worker process crashes
- Test with different TLS versions and ALPN protocols

## File Structure
- Dockerfile.awslc - Main build file
- ngx_http_ssl_fingerprint/ - Custom fingerprinting module
- conf/ - Nginx configuration templates
- README.md - Project documentation
