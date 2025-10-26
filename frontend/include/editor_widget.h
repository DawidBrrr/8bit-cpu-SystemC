#pragma once
#include "imgui.h"
#include <string>

class EditorWidget {
public:
    EditorWidget();
    void Render();
    
    std::string GetCode() const { return std::string(code_buffer); }
    void SetCode(const std::string& code);
    void LoadFromFile(const char* filepath);
    void SaveToFile(const char* filepath);
    
private:
    static const int BUFFER_SIZE = 1024 * 16; // 16KB buffer for code
    char code_buffer[BUFFER_SIZE];
    bool show_line_numbers;
};
