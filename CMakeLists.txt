cmake_minimum_required (VERSION 2.6)
project (HPC-pattern-tool)

SET (CMAKE_C_COMPILER /rwthfs/rz/SW/UTIL/clang/4.0.1/bin/clang)
SET (CMAKE_CXX_COMPILER /rwthfs/rz/SW/UTIL/clang/4.0.1/bin/clang++)

add_compile_options (-std=c++14)


# FIND LLVM for linking it later
find_package (LLVM REQUIRED CONFIG)

message (STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message (STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core irreader)


# Link against libraries
add_executable (HPC-pattern-tool hpcpatterntool.cpp)
target_link_libraries (HPC-pattern-tool clangTooling)
target_link_libraries (HPC-pattern-tool ${llvm_libs})
