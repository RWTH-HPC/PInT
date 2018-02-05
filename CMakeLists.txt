cmake_minimum_required (VERSION 2.6)
project (HPC-pattern-tool)

SET (CMAKE_C_COMPILER /rwthfs/rz/SW/UTIL/clang/4.0.1/bin/clang)
SET (CMAKE_CXX_COMPILER /rwthfs/rz/SW/UTIL/clang/4.0.1/bin/clang++)

add_executable (HPC-pattern-tool hpctool.cpp)
