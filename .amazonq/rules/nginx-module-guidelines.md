# Nginx Module Development Guidelines

## Memory Management
- Always use nginx memory pools: `ngx_pnalloc(r->pool, size)`
- Never use malloc/free directly
- Memory is automatically freed when request completes

## String Handling
- Use `ngx_memcpy()` for copying data
- Use `ngx_sprintf()` for simple formatting only
- For complex strings, build manually character by character
- Always null-terminate strings when needed

## SSL/TLS Access
- Check `r->connection->ssl` before accessing
- Check `r->connection->ssl->connection` (SSL*) before using
- Use OpenSSL API functions: `SSL_version()`, `SSL_get_current_cipher()`, etc.
- Handle missing data gracefully (return empty or default values)

## Variable Registration
- Register variables in `add_variables` callback
- Set `get_handler` for variable value retrieval
- Return `NGX_OK` on success, `NGX_ERROR` on failure
- Set `v->not_found = 1` when data unavailable

## Configuration Directives
- Use `ngx_conf_set_flag_slot` for on/off directives
- Store configuration in module context structure
- Merge parent/child configurations properly

## Error Handling
- Always check return values
- Log errors using `ngx_log_error()`
- Return appropriate status codes
- Never crash - handle all edge cases

## Performance
- Minimize allocations
- Cache computed values when possible
- Use efficient algorithms (SHA256 over MD5 when security matters)
- Avoid unnecessary string operations
