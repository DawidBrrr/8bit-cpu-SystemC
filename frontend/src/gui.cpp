#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include "editor_widget.h"
#include "cpu_viewer.h"
#include "cpu_controller.h"


int main(int, char**)
{
    if (!glfwInit()) return 1;

    // Decide GL+GLSL versions
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);  

    GLFWwindow* window = glfwCreateWindow(1280, 720, "8-bit CPU Emulator", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Create widgets
    EditorWidget editor;
    CPUViewer cpuViewer;
    CPUController cpuController;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Get the current window size
        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);

        // Main window - full screen
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2((float)window_width, (float)window_height), ImGuiCond_Always);
        ImGui::Begin("8-bit CPU Emulator", nullptr, 
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | 
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        
        // Calculate column widths (60% left, 40% right)
        float left_width = window_width * 0.6f - 15;
        float right_width = window_width * 0.4f - 15;
        
        // Left column - Code Editor
        ImGui::BeginChild("LeftPanel", ImVec2(left_width, window_height - 80), true);
        editor.Render();
        ImGui::EndChild();
        
        ImGui::SameLine();
        
        // Right column - CPU State
        ImGui::BeginChild("RightPanel", ImVec2(right_width, window_height - 80), true);
        cpuViewer.Render();
        ImGui::EndChild();
        
        // Bottom control panel
        ImGui::Separator();
        ImGui::Spacing();
        
        // Control buttons
        bool isRunning = cpuController.IsRunning();
        
        if (isRunning) {
            if (ImGui::Button("Stop")) {
                cpuController.Stop();
            }
        } else {
            if (ImGui::Button("Play")) {
                cpuController.LoadProgram(editor.GetCode());
                cpuController.Start();
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Step")) {
            cpuController.Step();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            cpuController.Reset();
        }
        
        ImGui::SameLine();
        ImGui::Text("  |  FPS: %.1f", io.Framerate);
        
        // Update CPU viewer with current state
        if (isRunning || ImGui::IsKeyPressed(ImGuiKey_Space)) {
            uint8_t a, x, y, s, p;
            uint16_t pc;
            cpuController.GetCPUState(a, x, y, s, p, pc);
            cpuViewer.UpdateState(a, x, y, s, p, pc);
        }
        
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
