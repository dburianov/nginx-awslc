geoip_country /usr/local/nginx/geoip/GeoIP.dat;
geoip_city    /usr/local/nginx/geoip/GeoIPCity.dat;
geoip_org     /usr/local/nginx/geoip/GeoIPASNum.dat;

map $geoip_country_code $allowed_country {
    default yes;
    RU      no;
}

geo $blacklist {
    default         0;
    172.16.0.0/24   1;
}

# GeoIP Block
#if ($allowed_country = no) {
##    add_header X-301 badcountry;
#    return 489;
#}
# Drop method
#if ( $request_method !~ ^(GET|HEAD)$ ) {
##    add_header X-301 badmethod;
#    return 490;
#}

#if ($blacklist) {
#    return 484;
#}

# load_module modules/ngx_http_geoip2_module.so;
# geoip2 /usr/share/GeoIP/GeoLite2-City.mmdb {
#   auto_reload 60m;
#   $geoip2_metadata_city_build metadata build_epoch;
#   $geoip2_data_country_name country names en;
#   $geoip2_data_country_code country iso_code;
#   $geoip2_data_city_name city names en;
#   $geoip2_data_region_name subdivisions 0 names en;
#   $geoip2_data_state_code  subdivisions 0 iso_code;
# }
# $geoip2_metadata_city_build - временная метка сборки базы
# $geoip2_data_country_name - название страны на английском
# $geoip2_data_country_code - двухбуквенный код страны
# $geoip2_data_city_name - имя города на английском
# $geoip2_data_region_name - имя региона
# $geoip2_data_state_code - код региона. Это именно то, что нам надо для фильтрации. Бывают двухбуквенные коды, трехбуквенные, цифровые коды. И даже однобуквенные. Иногда этот параметр вовсе отсутствует в базе.

# map "$geoip2_data_country_code:$geoip2_data_state_code" $allowed_reg {
#   default no;
#   ~^RU: yes; #Россия
#   ~^BY: yes; #Белоруссия
#   ~^AM: yes; #Армения
#   ~^KZ: yes; #Казахстан
#   ~^KG: yes; #Кыргызстан
#   UA:40 yes; #Севастополь
#   UA:43 yes; #Крым
#   UA:14 yes; #Донецкая область
#   UA:09 yes; #Луганская область
#   UA:23 yes; #Запорожская область
#   UA:65 yes; #Херсонская область
#   GE:AB yes; #Абхазия
# }
# server {
#   #Block forbidden country
#   if ($allowed_reg = no) {
#     return 444;
#   }
