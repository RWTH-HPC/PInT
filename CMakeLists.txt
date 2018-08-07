cmake_minimum_required (VERSION 2.6)
project (HPC-pattern-tool)

set (CMAKE_C_COMPILER /rwthfs/rz/SW/UTIL/clang/5.0.1/bin/clang)
set (CMAKE_CXX_COMPILER /rwthfs/rz/SW/UTIL/clang/5.0.1/bin/clang++)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra")

set (CLANG_INCLUDE_DIR "/rwthfs/rz/SW/UTIL/clang/5.0.1/lib64/clang/5.0.1" CACHE STRING
	"Dirrectory where the clang system library header files are located")
add_definitions (-DCLANG_INCLUDE_DIR="${CLANG_INCLUDE_DIR}")

set (PRINT_DEBUG_INFORMATION OFF CACHE BOOL
	"Toogle output of AST Information and RAW Comment List")
if (PRINT_DEBUG_INFORMATION)
	add_definitions (-DPRINT_DEBUG)
endif ()


find_package(Clang REQUIRED clangTooling libClang clangAST)
message (STATUS "Found clang ${CLANG_VERSION_STRING}")

find_package(LLVM REQUIRED CONFIG)
message (STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message (STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)


include_directories(${CLANG_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS} ${CLANG_DEFINITIONS})


add_llvm_executable (HPC-pattern-tool HPCPatternTool.cpp HPCPatternInstrASTTraversal.cpp HPCParallelPattern.cpp HPCPatternInstrHandler.cpp TreeVisualisation.cpp HPCPatternStatistics.cpp Helpers.cpp SimilarityMetrics.cpp PatternGraph.cpp DesignSpaces.cpp) 
target_link_libraries (HPC-pattern-tool ${llvm_libs} clangBasic clangTooling)

