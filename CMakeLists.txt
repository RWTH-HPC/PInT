cmake_minimum_required (VERSION 3.12)
project (HPC-pattern-tool)

cmake_policy(SET CMP0074 NEW)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra")

set (CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wall -Wextra")

set (CLANG_INCLUDE_DIR "/rwthfs/rz/SW/UTIL/clang/7.0.0/lib64/clang/7.0.0" CACHE STRING
	"Directory where the clang system library header files are located")
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
set(CMAKE_BUILD_TYPE Release)

add_llvm_executable (HPC-pattern-tool HPCPatternTool.cpp HPCPatternInstrASTTraversal.cpp HPCParallelPattern.cpp HPCPatternInstrHandler.cpp TreeVisualisation.cpp HPCPatternStatistics.cpp Helpers.cpp SimilarityMetrics.cpp PatternGraph.cpp DesignSpaces.cpp HPCRunningStats.cpp ToolInformation.cpp HPCError.cpp)
target_compile_options(HPC-pattern-tool
  PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fexceptions >
)
target_link_libraries (HPC-pattern-tool PUBLIC ${llvm_libs} clangBasic clangTooling)
