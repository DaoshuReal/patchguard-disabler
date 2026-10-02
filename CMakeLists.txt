cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_VERSION "10.0.28000.0")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(PatchGuardDisabler LANGUAGES C CXX ASM_MASM)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)

add_subdirectory(src/Driver)

add_subdirectory(src/Canary)
