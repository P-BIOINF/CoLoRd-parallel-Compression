cmake_minimum_required(VERSION 3.28)
project(Colord-Compression VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(Compression CoLoRd-parallel-compression.cpp Parallel.cpp)
