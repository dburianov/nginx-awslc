# JA4 Fingerprint Specification

## Format
`<protocol><version><direction><sni><cipher_count><ext_count><alpn>_<cipher_hash>_<ext_hash>`

Example: `t13id0103h2_55b375c5d22e_5a06198afb93`

## Components

### Protocol (1 char)
- `t` = TCP
- `q` = QUIC

### Version (2 chars)
- `10` = TLS 1.0
- `11` = TLS 1.1
- `12` = TLS 1.2
- `13` = TLS 1.3

### Direction (1 char)
- `d` = client/downstream
- `i` = server/inbound

### SNI (1 char)
- `d` = SNI present
- `i` = SNI not present

### Cipher Count (2 digits)
- Number of cipher suites (01-99)

### Extension Count (2 digits)
- Number of extensions (01-99)

### ALPN (2 chars)
- `h2` = HTTP/2
- `h3` = HTTP/3
- `00` = no ALPN

### Cipher Hash (12 chars)
- SHA256 hash of cipher suite, truncated to 12 hex chars

### Extension Hash (12 chars)
- SHA256 hash of extensions, truncated to 12 hex chars

## Implementation Notes
- Use lowercase hex for hashes
- Separate sections with underscore
- Total length: 10 + 1 + 12 + 1 + 12 = 36 chars
- Handle missing data with defaults (00, zeros)
