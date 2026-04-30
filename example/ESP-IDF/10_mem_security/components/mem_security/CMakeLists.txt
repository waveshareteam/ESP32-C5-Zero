idf_component_register(SRCS "mem_security.c"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_system spi_flash freertos heap mbedtls esp_security bootloader_support)
