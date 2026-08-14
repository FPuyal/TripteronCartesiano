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
file(GLOB_RECURSE USER_SOURCES "tripteron_firmware/*.cpp")
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${USER_SOURCES})

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    tripteron_firmware/inc/hal/gpio
    tripteron_firmware/inc/hal/timer
    tripteron_firmware/inc/hal/i2c
    tripteron_firmware/inc/hal/uart
    tripteron_firmware/inc/hal/usb_cdc
    tripteron_firmware/inc/comms
    tripteron_firmware/inc/drivers/tmc
    tripteron_firmware/inc/drivers/encoder
    tripteron_firmware/inc/hardware_manager
    tripteron_firmware/inc/motion/trajectory_generator
    tripteron_firmware/inc/motion/step_engine
    tripteron_firmware/inc/motion/kinematics
    tripteron_firmware/inc/motion/motion_controller
    tripteron_firmware/inc/robot
    tripteron_firmware/inc/common

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
