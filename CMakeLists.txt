cmake_minimum_required(VERSION 3.15)

project(mylang C)

add_compile_options(-Wall -Wextra)

if(${DO_ASAN})
	add_compile_options(-fsanitize=address)
	add_link_options(-fsanitize=address)
endif()

if(${MEMORY_CHECK})
	add_compile_options(-DMEMORY_CHECK)
endif()

add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)

target_include_directories(${PROJECT_NAME} PRIVATE ./src)

add_subdirectory(src)


