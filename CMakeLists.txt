cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

message("CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")

set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf32")
enable_language(ASM_NASM)

project(MaOS C)

add_subdirectory(kernel)