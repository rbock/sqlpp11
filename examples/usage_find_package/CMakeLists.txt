### Preamble ###
cmake_minimum_required(VERSION 3.14)
project(Test VERSION 0.1 LANGUAGES CXX)

##### Project wide setup ####
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

### Dependencies ###
find_package(Sqlpp11 REQUIRED)

### Main Targets ###
add_subdirectory(src)