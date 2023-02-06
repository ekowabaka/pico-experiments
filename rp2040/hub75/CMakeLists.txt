# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pio_hub75 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(starfield)
pico_enable_stdio_usb(starfield 0)
pico_enable_stdio_uart(starfield 1)
pico_generate_pio_header(starfield ${CMAKE_CURRENT_LIST_DIR}/hub75.pio)
target_sources(starfield PRIVATE starfield.c display.c)
target_link_libraries(starfield PRIVATE pico_stdlib hardware_pio)

add_executable(smiley)
pico_enable_stdio_usb(smiley 0)
pico_enable_stdio_uart(smiley 1)
pico_generate_pio_header(smiley ${CMAKE_CURRENT_LIST_DIR}/hub75.pio)
target_sources(smiley PRIVATE smiley.c display.c)
target_link_libraries(smiley PRIVATE pico_stdlib hardware_pio)

add_executable(text)
pico_enable_stdio_usb(text 0)
pico_enable_stdio_uart(text 1)
pico_generate_pio_header(text ${CMAKE_CURRENT_LIST_DIR}/hub75.pio)
target_sources(text PRIVATE text.c display.c)
target_link_libraries(text PRIVATE pico_stdlib hardware_pio)

add_executable(multicore)
pico_enable_stdio_usb(multicore 0)
pico_enable_stdio_uart(multicore 1)
pico_generate_pio_header(multicore ${CMAKE_CURRENT_LIST_DIR}/hub75.pio)
target_sources(multicore PRIVATE multicore.c display.c)
target_link_libraries(multicore PRIVATE pico_stdlib pico_multicore hardware_pio hardware_rtc)

