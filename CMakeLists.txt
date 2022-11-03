cmake_minimum_required(VERSION 3.13.4)

if(POLICY CMP0068)
	cmake_policy(SET CMP0068 NEW)
	set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR ON)
endif()

if(POLICY CMP0075)
	cmake_policy(SET CMP0075 NEW)
endif()

if(POLICY CMP0077)
	cmake_policy(SET CMP0077 NEW)
endif()

project(LLVM_Tutorail)

set(CMAKE_CXX_STANDARD 17)

set(LLVM_DIR "F:\\llvm-12\\lib\\cmake\\llvm")
#set(LLVM_DIR "D:\\dev\\libs\\llvm\\lib\\cmake\\llvm")


find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_BUILD_LIBARARY_DIR})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs core support nativecodegen orcjit)

include_directories(include)

add_llvm_executable(Kaleidoscope main.cpp Lexer.cpp Parser.cpp AST.cpp)
target_link_libraries(Kaleidoscope ${llvm_libs})

