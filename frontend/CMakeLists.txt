cmake_minimum_required(VERSION 3.15)

# Minimal GUI target using GLFW + ImGui
add_executable(cpu_gui
    main_imgui.cpp
    ${IMGUI_SOURCES}
)

target_include_directories(cpu_gui PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(cpu_gui PRIVATE
    glfw
    ${CMAKE_DL_LIBS}
    opengl32
)

# set compile options
target_compile_options(cpu_gui PRIVATE /EHsc)
