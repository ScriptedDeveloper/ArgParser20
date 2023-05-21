cmake_minimum_required(VERSION 3.16)
project(ArgParser++ VERSION 1.0.0 DESCRIPTION "Compile-time header-only argument parser library written in C++20")

set(CMAKE_CXX_STANDARD 20 REQUIRED)
set(CMAKE_BUILD_TYPE Debug)

add_library(ArgParser SHARED include/argparser.hpp)

set_target_properties(ArgParser PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(ArgParser PROPERTIES PUBLIC_HEADER include/argparser.hpp)
set_target_properties(ArgParser PROPERTIES LINKER_LANGUAGE CXX)

include(GNUInstallDirs)
install(TARGETS ArgParser LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


