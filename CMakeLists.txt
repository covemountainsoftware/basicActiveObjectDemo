cmake_minimum_required(VERSION 3.16)
project(basicActiveObjectDemo)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror)

add_subdirectory(core)
add_subdirectory(drivers)
add_subdirectory(services)
add_subdirectory(apps)