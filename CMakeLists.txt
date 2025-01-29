cmake_minimum_required(VERSION 3.19)

project(mylang C)

add_compile_options(-Wall -Wextra)

if(${DO_ASAN})
	add_compile_options(-fsanitize=address)
	add_link_options(-fsanitize=address)
endif()

if(${MEMORY_CHECK})
	add_compile_options(-DMEMORY_CHECK)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED 19 < 19)

message("LLVM version: ${LLVM_PACKAGE_VERSION}")

add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)

target_include_directories(${PROJECT_NAME} PRIVATE ./src ${LLVM_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE LLVM)

add_subdirectory(src)



