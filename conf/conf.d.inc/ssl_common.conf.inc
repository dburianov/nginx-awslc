ssl_session_timeout 1d;
ssl_session_cache shared:TLS:10m;  # about 40000 sessions

ssl_session_tickets on;

ssl_dhparam /usr/local/nginx/conf.docker/ssl.dh/dhparams4096.pem;

ssl_protocols TLSv1.3 TLSv1.2;

# Enable Perfect Forward Secrecy (PFS)
ssl_prefer_server_ciphers on;
# ssl_prefer_server_ciphers off;

#
ssl_ecdh_curve X25519:secp384r1:secp521r1:prime256v1;
# ssl_ecdh_curve auto;
# ssl_ciphers EECDH+AES256GCM:EECDH+AES256:!CBC;
ssl_ciphers "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:AES256-GCM-SHA384:TLS_AES_128_GCM_SHA256:!CBC";

# ssl_buffer_size 4k;

# openssl rand 80 > ticket.key

# OCSP stapling
ssl_stapling on;
ssl_stapling_verify on;

#  # verify chain of trust of OCSP response using Root CA and Intermediate certs
#  ssl_trusted_certificate /certs/dir/chain.pem;
#  ssl_trusted_certificate /etc/nginx/ssl/fullchain.pem;
#  ssl_stapling_file /tmp/nginx/conf.d.inc/ssl/self-signed.crt;

#  Due to usage of BoringSSL instead of OpenSSL, some directives may not work, e.g. ssl_conf_command.
#  Besides, direct OCSP stapling via ssl_stapling on; ssl_stapling_verify on; does not work too.
#  You should use ssl_stapling on; ssl_stapling_file /path/to/ocsp;.
#  The OCSP file can be generated via
#  openssl ocsp -no_nonce -issuer /path/to/intermediate -cert /path/to/cert -url "$(openssl x509 \
#    -in /path/to/cert -noout -ocsp_uri)" -respout /path/to/ocsp.

# ssl_verify_client off;

# 0-RTT QUIC connection resumption
ssl_early_data on;

# https://security.stackexchange.com/questions/251713/how-to-use-dh-certificate-to-configure-nginx-server
# https://security.stackexchange.com/questions/44251/openssl-generate-different-types-of-self-signed-certificate
# https://www.ssllabs.com/ssltest/analyze.html?d=registry.burianov.net
# https://www.nginx.com/blog/http-strict-transport-security-hsts-and-nginx/
# https://hstspreload.org/?domain=burianov.net
# https://clienttest.ssllabs.com:8443/ssltest/viewMyClient.html
# https://medium.com/hackernoon/rsa-and-ecdsa-hybrid-nginx-setup-with-letsencrypt-certificates-ee422695d7d3

# openssl ocsp -no_nonce -issuer /etc/letsencrypt/live/burianov.net/chain.pem -cert /etc/letsencrypt/live/burianov.net/cert.pem -url "$(openssl x509 -in /etc/letsencrypt/live/burianov.net/cert.pem -noout -ocsp_uri)" -respout ./burianov.net.ocsp
