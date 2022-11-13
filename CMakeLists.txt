cmake_minimum_required(VERSION 2.8)
project(563s21_p3_cpp)

set(CMAKE_CXX_STANDARD 11)

set(
        sim_cache_src cache.cc
)
set(
        sim_cache_hdr cache.h
)

set(
	sim_cache_matrix matrix.cc
)

add_library(
        sim_cache STATIC
        ${sim_cache_src}
        ${sim_cache_hdr}
	${sim_cache_matrix}
)
target_include_directories(sim_cache PUBLIC .)
include_directories(traces)
add_subdirectory(testcases)
add_executable(matrix matrix.cc)
