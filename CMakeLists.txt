cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(MCU_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

add_compile_options(${MCU_FLAGS})
add_link_options(${MCU_FLAGS})

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME TripteronCartesiano)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    tripteron_firmware/main.cpp
    tripteron_firmware/src/timer/timer.cpp
    tripteron_firmware/src/gpio/gpio_wrapper.cpp
    tripteron_firmware/src/tmc/tmc.cpp
    tripteron_firmware/src/hardware_manager/hardware_manager.cpp
    tripteron_firmware/src/i2c/i2c_wrapper.cpp
    tripteron_firmware/src/encoder/encoder.cpp
<<<<<<< HEAD
    tripteron_firmware/src/hardware_manager/managers_factories.cpp
    tripteron_firmware/src/gpio/gpio_input.cpp
    tripteron_firmware/src/gpio/gpio_output.cpp
=======
    tripteron_firmware/src/hardware_manager/make_managers.cpp
    tripteron_firmware/src/trayectory_profile/trapezoidal_profile.cpp
>>>>>>> 8c847bf (Añadida clase TrayectoryProfile y prueba de concepto del s-curve)
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    tripteron_firmware/inc
    tripteron_firmware/inc/gpio
    tripteron_firmware/inc/timer
    tripteron_firmware/inc/tmc
    tripteron_firmware/inc/hardware_manager
    tripteron_firmware/inc/i2c
    tripteron_firmware/inc/encoder
    tripteron_firmware/inc/trayectory_profile
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
