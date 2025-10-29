#include "editor_widget.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>

namespace {
std::string TrimString(const std::string& value) {
    const char* whitespace = " \t\r\n";
    const auto start = value.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    const auto end = value.find_last_not_of(whitespace);
    return value.substr(start, end - start + 1);
}
}

EditorWidget::EditorWidget()
    : show_line_numbers(true),
      programs_directory_("../programs"),
      show_program_browser_(false),
      show_save_dialog_(false) {
    memset(code_buffer, 0, BUFFER_SIZE);
    memset(save_name_buffer_, 0, sizeof(save_name_buffer_));
    // Default example code
    const char* default_code = 
        "A9 48    ; LDA #$48 ('H')\n"
        "8D 00 02 ; STA $0200\n"
        "A9 65    ; LDA #$65 ('e')\n"
        "8D 01 02 ; STA $0201\n"
        "00       ; BRK\n";
    strncpy(code_buffer, default_code, BUFFER_SIZE - 1);

    RefreshProgramList();
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
        show_program_browser_ = true;
        RefreshProgramList();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Program")) {
        show_save_dialog_ = true;
        memset(save_name_buffer_, 0, sizeof(save_name_buffer_));
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        memset(code_buffer, 0, BUFFER_SIZE);
    }

    if (!status_message_.empty()) {
        ImGui::SameLine();
        ImGui::TextUnformatted(status_message_.c_str());
    }

    if (show_program_browser_) {
        RenderProgramBrowser();
    }

    if (show_save_dialog_) {
        RenderSaveDialog();
    }
}

void EditorWidget::SetCode(const std::string& code) {
    strncpy(code_buffer, code.c_str(), BUFFER_SIZE - 1);
    code_buffer[BUFFER_SIZE - 1] = '\0';
}

bool EditorWidget::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        SetCode(buffer.str());
        file.close();
        return true;
    }
    return false;
}

bool EditorWidget::SaveToFile(const std::string& filepath) {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << code_buffer;
        file.close();
        return true;
    }
    return false;
}

void EditorWidget::RefreshProgramList() {
    program_files_.clear();

    std::error_code ec;
    if (!std::filesystem::exists(programs_directory_, ec)) {
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(programs_directory_, ec)) {
        if (ec) {
            break;
        }
        if (!entry.is_regular_file()) {
            continue;
        }
        program_files_.push_back(entry.path().filename().string());
    }

    std::sort(program_files_.begin(), program_files_.end());
}

bool EditorWidget::DeleteProgram(const std::string& filename) {
    std::filesystem::path target = std::filesystem::path(programs_directory_) / filename;
    std::error_code ec;
    bool removed = std::filesystem::remove(target, ec);
    if (!removed || ec) {
        status_message_ = "Failed to delete " + filename;
        return false;
    }
    status_message_ = "Deleted " + filename;
    return true;
}

void EditorWidget::RenderProgramBrowser() {
    ImGui::SetNextWindowSize(ImVec2(420.0f, 320.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Program Browser", &show_program_browser_, ImGuiWindowFlags_NoCollapse)) {
        ImGui::Text("Directory: %s", programs_directory_.c_str());
        if (ImGui::Button("Refresh")) {
            RefreshProgramList();
        }
        ImGui::Separator();

        ImGui::BeginChild("ProgramList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true, ImGuiWindowFlags_HorizontalScrollbar);
        bool refresh_after_loop = false;
        bool close_after_selection = false;
        if (program_files_.empty()) {
            ImGui::TextDisabled("No programs found.");
        } else {
            for (const auto& filename : program_files_) {
                ImGui::PushID(filename.c_str());
                if (ImGui::SmallButton("X")) {
                    if (DeleteProgram(filename)) {
                        refresh_after_loop = true;
                    }
                    ImGui::PopID();
                    continue;
                }
                ImGui::SameLine();
                if (ImGui::Selectable(filename.c_str(), false)) {
                    std::filesystem::path full_path = std::filesystem::path(programs_directory_) / filename;
                    if (LoadFromFile(full_path.string())) {
                        status_message_ = "Loaded " + filename;
                    } else {
                        status_message_ = "Failed to load " + filename;
                    }
                    close_after_selection = true;
                }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();

        if (refresh_after_loop) {
            RefreshProgramList();
        }
        if (close_after_selection) {
            show_program_browser_ = false;
        }

        if (ImGui::Button("Close")) {
            show_program_browser_ = false;
        }
    }
    ImGui::End();
}

void EditorWidget::RenderSaveDialog() {
    ImGui::SetNextWindowSize(ImVec2(360.0f, 160.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Save Program", &show_save_dialog_, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Save to: %s", programs_directory_.c_str());

        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere();
        }

        ImGui::InputText("Filename", save_name_buffer_, IM_ARRAYSIZE(save_name_buffer_));

        const std::string trimmed = TrimString(save_name_buffer_);
        const bool has_invalid_chars = trimmed.find_first_of("/\\") != std::string::npos || trimmed.find("..") != std::string::npos;
        const bool can_save = !trimmed.empty() && !has_invalid_chars;

        if (trimmed.empty()) {
            ImGui::TextDisabled("Enter a file name (e.g. example.txt)");
        } else if (has_invalid_chars) {
            ImGui::TextDisabled("Invalid characters in file name");
        }

        ImGui::BeginDisabled(!can_save);
        bool save_clicked = ImGui::Button("Save");
        ImGui::EndDisabled();
        if (save_clicked && can_save) {
            std::filesystem::create_directories(programs_directory_);
            std::filesystem::path target = std::filesystem::path(programs_directory_) / trimmed;
            if (!target.has_extension()) {
                target.replace_extension(".txt");
            }
            if (SaveToFile(target.string())) {
                status_message_ = "Saved " + target.filename().string();
                RefreshProgramList();
                show_save_dialog_ = false;
            } else {
                status_message_ = "Failed to save " + target.filename().string();
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            show_save_dialog_ = false;
        }
    }
    ImGui::End();
}
