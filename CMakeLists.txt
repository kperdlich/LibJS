cmake_minimum_required(VERSION 3.17)
project(LibJS)

set(CMAKE_CXX_STANDARD 20)

add_executable(LibJS main.cpp AST.h Value.h Types.h Interpreter.h Value.cpp)