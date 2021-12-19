# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(white_hub75 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(white_hub75)

pico_enable_stdio_usb(white_hub75 0)
pico_enable_stdio_uart(white_hub75 1)

# pico_generate_pio_header(pio_hub75 ${CMAKE_CURRENT_LIST_DIR}/hub75.pio)

target_sources(white_hub75 PRIVATE hub75.c)

# target_compile_definitions(pio_hub75 PRIVATE
# 	PICO_DEFAULT_UART_TX_PIN=28
# 	PICO_DEFAULT_UART_RX_PIN=29
# )

target_link_libraries(white_hub75 PRIVATE pico_stdlib)
