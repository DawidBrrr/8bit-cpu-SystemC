#include "editor_widget.h"
#include <cstring>
#include <fstream>
#include <sstream>

EditorWidget::EditorWidget() : show_line_numbers(true) {
    memset(code_buffer, 0, BUFFER_SIZE);
    // Default example code
    const char* default_code = 
        "A9 48    ; LDA #$48 ('H')\n"
        "8D 00 02 ; STA $0200\n"
        "A9 65    ; LDA #$65 ('e')\n"
        "8D 01 02 ; STA $0201\n"
        "00       ; BRK\n";
    strncpy(code_buffer, default_code, BUFFER_SIZE - 1);
}

void EditorWidget::Render() {
    ImGui::BeginChild("CodeEditor", ImVec2(0, -35), true);
    
    ImGui::Text("Assembly Code Editor");
    ImGui::Separator();
    
    ImGui::InputTextMultiline("##code", code_buffer, BUFFER_SIZE, 
                               ImVec2(-1, -1), 
                               ImGuiInputTextFlags_AllowTabInput);
    
    ImGui::EndChild();
    
    // Bottom buttons
    if (ImGui::Button("Load Program")) {
        LoadFromFile("../programs/hello.txt");
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        memset(code_buffer, 0, BUFFER_SIZE);
    }
}

void EditorWidget::SetCode(const std::string& code) {
    strncpy(code_buffer, code.c_str(), BUFFER_SIZE - 1);
    code_buffer[BUFFER_SIZE - 1] = '\0';
}

void EditorWidget::LoadFromFile(const char* filepath) {
    std::ifstream file(filepath);
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        SetCode(buffer.str());
        file.close();
    }
}

void EditorWidget::SaveToFile(const char* filepath) {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << code_buffer;
        file.close();
    }
}
