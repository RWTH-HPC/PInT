cmake_minimum_required (VERSION 2.6)
project (test-example)

SET (CMAKE_C_COMPILER /rwthfs/rz/SW/UTIL/clang/5.0.1/bin/clang)
SET (CMAKE_CXX_COMPILER /rwthfs/rz/SW/UTIL/clang/5.0.1/bin/clang++)
SET (CMAKE_CXX_VERSION 14)

include(FindOpenMP)

find_package (LLVM REQUIRED CONFIG)

message (STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message (STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}") 

include (AddLLVM)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


add_llvm_executable(test-example test-example.cpp external-function.cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
