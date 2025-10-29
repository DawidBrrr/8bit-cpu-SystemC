#pragma once
#include "imgui.h"
#include <filesystem>
#include <string>
#include <vector>

class EditorWidget {
public:
    EditorWidget();
    void Render();
    
    std::string GetCode() const { return std::string(code_buffer); }
    void SetCode(const std::string& code);
    bool LoadFromFile(const std::string& filepath);
    bool SaveToFile(const std::string& filepath);
    
private:
    static const int BUFFER_SIZE = 1024 * 16; // 16KB buffer for code
    char code_buffer[BUFFER_SIZE];
    bool show_line_numbers;
    std::string programs_directory_;
    std::vector<std::string> program_files_;
    bool show_program_browser_;
    bool show_save_dialog_;
    char save_name_buffer_[256];
    std::string status_message_;

    void RefreshProgramList();
    bool DeleteProgram(const std::string& filename);
    void RenderProgramBrowser();
    void RenderSaveDialog();
};
