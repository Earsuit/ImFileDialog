cmake_minimum_required(VERSION 3.1)
project(ImFileDialogExample)

cmake_policy(SET CMP0072 NEW)

# source code
set(SOURCES
	example.cpp
	ImFileDialog.cpp
    StbImpl.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

SET(IMGUI_SRC 
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/misc/cpp/imgui_stdlib.cpp
)

include_directories(external/imgui)
include_directories(external/stb)
include_directories(external/magic_enum/include)

# pkg
if (UNIX)
    find_package(PkgConfig REQUIRED)
endif()

if (LINUX)
    pkg_check_modules(GIO2 REQUIRED IMPORTED_TARGET gio-2.0)
    pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
endif()

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED)

# According to https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1423r2.html
# The simplest solution to upgrade to c++20 with char8_t 
if (MSVC)
    add_compile_options(/Zc:char8_t-)
else()
    # -ftemplate-depth for the DEFAULT_FILE_ICON and DEFAULT_FOLDER_ICON
    add_compile_options(-fno-char8_t -ftemplate-depth=2048)
    if (APPLE)
        # https://github.com/llvm/llvm-project/issues/48757, use -Wno-elaborated-enum-base to suppress the enum bug on Apple
        add_compile_options(-Wno-elaborated-enum-base -ObjC++ -DGL_SILENCE_DEPRECATION)
    endif()
endif()

# create executable
add_executable(ImFileDialogExample ${SOURCES} ${IMGUI_SRC})

# properties
set_target_properties(ImFileDialogExample PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

# link libraries
target_link_libraries(ImFileDialogExample PRIVATE OpenGL::GL glfw GLEW::GLEW ${CMAKE_DL_LIBS} Threads::Threads)

if (LINUX)
    target_link_libraries(ImFileDialogExample PRIVATE PkgConfig::GIO2 PkgConfig::GTK3)
endif()

if (APPLE)
    target_link_libraries(ImFileDialogExample PRIVATE "-framework CoreFoundation" "-framework CoreGraphics" "-framework ImageIO" "-framework AppKit")
endif()