if ( $request_method !~ ^(GET|DELETE|OPTIONS|POST)$ ) { return 405; }
location / {
  return 403;
}
if ( $request_method !~ ^(GET|DELETE|OPTIONS|POST|PUT|UPDATE|PATCH)$ ) { return 405; }
location / {
  return 405;
}
