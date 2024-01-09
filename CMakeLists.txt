cmake_minimum_required(VERSION 3.26)
project(bf)

set(CMAKE_CXX_STANDARD 23)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(bf src/main.cpp src/Scanner.cpp src/Token.h src/AST.h src/Parser.cpp src/Interpreter.cpp src/Compiler.cpp
        src/Optimizer.h
        src/Compiler.h
)

llvm_map_components_to_libnames(llvm_libs support core irreader passes)
target_link_libraries(bf ${llvm_libs})
