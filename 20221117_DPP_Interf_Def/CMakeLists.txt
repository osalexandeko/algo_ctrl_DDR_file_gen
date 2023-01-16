cmake_minimum_required(VERSION 3.7.1)
project(DPP_Interface_Definition)
set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_C_STANDARD 99)
 
set(SOURCES 
		main.c
		aligned_malloc.c
		aligned_malloc.h
	) 
 
add_executable(DPP_Interface_Definition ${SOURCES})