cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/arv-gcc-toolchain.cmake")

project(mcu C CXX ASM)

set(MCU_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../mcu/src)
FILE(GLOB_RECURSE MyCSources ${MCU_SOURCE_DIR}/*.c)
include_directories(
        ${MCU_SOURCE_DIR}
)

get_property(IncludeDirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

message("List include dir: ${IncludeDirs}")

message("List source file: ${MyCSources}") 

add_definitions(
        -DF_CPU=16000000
        #-DUSE_UART_ASYNC=0
)

add_avr_executable(${PROJECT_NAME} "atmega2560"
                ${MyCSources}
)