cmake_minimum_required(VERSION 3.16)
project(text_editor)

set(CMAKE_CXX_STANDARD 17)

include_directories("../submodules/imgui")

set(IMGUI_PATH "../submodules/imgui")
set(IMGUI_BACKENDS_PATH "../submodules/imgui/backends")

add_executable(text_editor
        main.cpp
        PieceTable/PieceDescriptor.cpp
        PieceTable/PieceDescriptor.h
        PieceTable/SourceType.h
        PieceTable/PieceTable.cpp
        PieceTable/PieceTable.h
        PieceTable/ActionDescriptor.cpp
        PieceTable/ActionDescriptor.h
        )

file( GLOB LIB_SOURCES ${IMGUI_PATH}/*.cpp ${IMGUI_BACKENDS_PATH}/imgui_impl_win32.cpp ${IMGUI_BACKENDS_PATH}/imgui_impl_dx12.cpp)
file( GLOB LIB_HEADERS ${IMGUI_PATH}/*.h ${IMGUI_BACKENDS_PATH}/imgui_impl_win32.h ${IMGUI_BACKENDS_PATH}/imgui_impl_dx12.h)
add_library(imgui ${LIB_SOURCES} ${LIB_HEADERS} GUI/TextBox.cpp GUI/TextBox.h GUI/Cursor.cpp GUI/Cursor.h)

target_link_libraries(text_editor PRIVATE imgui)
target_link_libraries(text_editor PRIVATE d3d12.lib)
target_link_libraries(text_editor PRIVATE dxgi.lib)
#target_include_directories(text_editor PRIVATE "C:/Users/bbard/OneDrive/Desktop/MasterRad/master-rad/submodules/imgui")
