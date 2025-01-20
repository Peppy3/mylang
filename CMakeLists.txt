cmake_minimum_required(VERSION 3.15)

project(mylang C)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	add_compile_options(-fsanitize=address)
	add_link_options(-fsanitize=address)
endif()

add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)

add_subdirectory(src)

