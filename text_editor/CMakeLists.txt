cmake_minimum_required(VERSION 3.16)
project(text_editor)

set(CMAKE_CXX_STANDARD 17)

add_executable(text_editor main.cpp PieceTable/PieceDescriptor.cpp PieceTable/PieceDescriptor.h PieceTable/SourceType.h)
