# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Mandelbrot_07 C CXX ASM)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(Mandelbrot_07
        main.c
        commands.c
        calculations.c
        screen.c
        project.h

        iterate.s
        multiply.s
        multiply_safe.s
        add_sub.s
        square.s

        drivers/drivers.h
        drivers/driver-defines.h

        drivers/draw.c
        drivers/font-5x10.c
        drivers/font-8x10.c
        drivers/keyboard.c
        drivers/lcd.c
        drivers/picocalc.c
        drivers/southbridge.c
        drivers/terminal.c
        )

pico_set_program_name(Mandelbrot_07 "Mandelbrot_07")
pico_set_program_version(Mandelbrot_07 "07")

# Modify the below lines to enable/disable output over UART/USB (0 is disabled)
pico_enable_stdio_uart(Mandelbrot_07 0)
pico_enable_stdio_usb(Mandelbrot_07 0)

# Turn on all warnings
target_compile_options(Mandelbrot_07 PRIVATE -Wall -Werror)

# Add the standard library to the build
target_link_libraries(Mandelbrot_07
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(Mandelbrot_07 PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# Add any user requested libraries
target_link_libraries(Mandelbrot_07
        pico_stdlib
        pico_printf
        pico_float
        pico_multicore
        pico_sync
        pico_time
        hardware_gpio
        hardware_i2c
        hardware_spi
        hardware_pio
        hardware_clocks
        )

pico_add_extra_outputs(Mandelbrot_07)

