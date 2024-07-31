cmake_minimum_required(VERSION 3.16)
project(text_editor)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 17)

set(IMGUI_PATH "../submodules/imgui")
set(IMGUI_BACKENDS_PATH "../submodules/imgui/backends")
set(LEXER_PATH "../submodules/lexertk")

include_directories(${IMGUI_PATH})
include_directories(${LEXER_PATH})

add_executable(text_editor
        main.cpp
        GUI/TextBox.cpp
        GUI/TextBox.h
        GUI/Cursor.cpp
        GUI/Cursor.h
        GUI/TextEditor.cpp
        GUI/TextEditor.h
        GUI/Font.cpp
        GUI/Font.h
        GUI/FontManager.cpp
        GUI/FontManager.h
        GUI/Selection.cpp
        GUI/Selection.h
        GUI/LineBuffer.cpp
        GUI/LineBuffer.h
        GUI/TextCoordinates.cpp
        GUI/TextCoordinates.h
        GUI/Scroll.cpp
        GUI/Scroll.h
        GUI/MyRectangle.cpp
        GUI/MyRectangle.h
        GUI/Theme.cpp
        GUI/Theme.h
        GUI/ThemeManager.cpp
        GUI/ThemeManager.h
        GUI/TextHighlighter.cpp
        GUI/TextHighlighter.h
        PieceTable/PieceDescriptor.cpp
        PieceTable/PieceDescriptor.h
        PieceTable/SourceType.h
        PieceTable/PieceTable.cpp
        PieceTable/PieceTable.h
        PieceTable/ActionDescriptor.cpp
        PieceTable/ActionDescriptor.h
        PieceTable/InsertBuffer.cpp
        PieceTable/InsertBuffer.h
        PieceTable/DeleteBuffer.cpp
        PieceTable/DeleteBuffer.h
        PieceTable/PieceTableInstance.cpp
        PieceTable/PieceTableInstance.h
        File.cpp
        File.h
        ${LEXER_PATH}/lexertk.hpp
        GUI/LanguageMode.h)

file( GLOB LIB_SOURCES ${IMGUI_PATH}/*.cpp ${IMGUI_BACKENDS_PATH}/imgui_impl_win32.cpp ${IMGUI_BACKENDS_PATH}/imgui_impl_dx12.cpp)
file( GLOB LIB_HEADERS ${IMGUI_PATH}/*.h ${IMGUI_BACKENDS_PATH}/imgui_impl_win32.h ${IMGUI_BACKENDS_PATH}/imgui_impl_dx12.h)

add_library(imgui ${LIB_SOURCES} ${LIB_HEADERS})

target_link_libraries(text_editor PRIVATE imgui)
target_link_libraries(text_editor PRIVATE d3d12.lib)
target_link_libraries(text_editor PRIVATE dxgi.lib)
