cmake_minimum_required(VERSION 3.15)

# Minimal GUI target using GLFW + ImGui
add_executable(cpu_gui
    src/gui.cpp
    src/editor_widget.cpp
    src/cpu_viewer.cpp
    src/cpu_controller.cpp
    ${IMGUI_SOURCES}
)

target_sources(cpu_gui PRIVATE ${CPU_SRC_FILES})

target_include_directories(cpu_gui PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/frontend/include
    ${CMAKE_SOURCE_DIR}/frontend
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${systemc_SOURCE_DIR}/src
)

target_link_libraries(cpu_gui PRIVATE
    glfw
    ${CMAKE_DL_LIBS}
    opengl32
    systemc
)

# set compile options
target_compile_options(cpu_gui PRIVATE /EHsc)
