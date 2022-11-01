cmake_minimum_required(VERSION 3.23)
project(LLVM_Tutorail)

set(CMAKE_CXX_STANDARD 17)

set(LLVM_DIR "F:\\llvm-12\\lib\\cmake\\llvm")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs core support nativecodegen orcjit)

include_directories(include)

file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(Kaleidoscope ${SRC})

