#=============================================================================#
# Author: Amorielo Hutti                                                      #
# Date:   10.01.2015                                                          #
#                                                                             #
#                                                                             #
#=============================================================================#


set(PROJECT_THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(CMAKE_TOOLCHAIN_FILE ${PROJECT_THIRD_PARTY_DIR}/arduino-cmake/cmake/ArduinoToolchain.cmake) # Arduino Toolchain


cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(TokenOnBoard C CXX)

register_hardware_platform(${PROJECT_THIRD_PARTY_DIR}/hardware/blend)

link_directories(${PROJECT_THIRD_PARTY_DIR}/libraries)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
add_subdirectory(src)
