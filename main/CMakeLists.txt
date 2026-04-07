idf_component_register(SRCS "main.c" "wifi.c"
                       INCLUDE_DIRS "."
                       REQUIRES ssd1306 servo nvs_flash esp_wifi esp_netif)
