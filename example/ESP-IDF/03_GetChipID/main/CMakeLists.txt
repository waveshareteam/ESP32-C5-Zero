idf_component_register(SRCS "main.c"
                    REQUIRES  esp_driver_gpio spi_flash esp_hw_support esp_system
                    INCLUDE_DIRS ".")
