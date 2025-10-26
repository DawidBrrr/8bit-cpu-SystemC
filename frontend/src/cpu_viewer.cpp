#include "cpu_viewer.h"

CPUViewer::CPUViewer() 
    : reg_A(0), reg_X(0), reg_Y(0), reg_S(0xFF), reg_P(0x34), reg_PC(0),
      flag_C(false), flag_Z(false), flag_I(false), flag_D(false), 
      flag_B(false), flag_V(false), flag_N(false) {
}

void CPUViewer::Render() {
    ImGui::BeginChild("CPUViewer", ImVec2(0, 0), true);
    
    ImGui::Text("CPU State");
    ImGui::Separator();
    
    RenderRegisters();
    ImGui::Spacing();
    ImGui::Separator();
    RenderFlags();
    ImGui::Spacing();
    ImGui::Separator();
    RenderIOWindow();
    
    ImGui::EndChild();
}

void CPUViewer::RenderRegisters() {
    ImGui::Text("Registers:");
    ImGui::Spacing();
    
    // Program Counter
    ImGui::Text("PC: $%04X", reg_PC);
    ImGui::Separator();
    
    // Main registers
    ImGui::Text("A:  $%02X  (%3d)", reg_A, reg_A);
    ImGui::Text("X:  $%02X  (%3d)", reg_X, reg_X);
    ImGui::Text("Y:  $%02X  (%3d)", reg_Y, reg_Y);
    ImGui::Separator();
    
    // Stack pointer
    ImGui::Text("SP: $%02X  (%3d)", reg_S, reg_S);
    ImGui::Text("P:  $%02X", reg_P);
}

void CPUViewer::RenderFlags() {
    ImGui::Text("Status Flags:");
    ImGui::Spacing();
    
    // Display flags with colors
    ImVec4 active_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);   // Green
    ImVec4 inactive_color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Gray
    
    ImGui::TextColored(flag_N ? active_color : inactive_color, "N"); ImGui::SameLine();
    ImGui::TextColored(flag_V ? active_color : inactive_color, "V"); ImGui::SameLine();
    ImGui::Text("-"); ImGui::SameLine();
    ImGui::TextColored(flag_B ? active_color : inactive_color, "B"); ImGui::SameLine();
    ImGui::TextColored(flag_D ? active_color : inactive_color, "D"); ImGui::SameLine();
    ImGui::TextColored(flag_I ? active_color : inactive_color, "I"); ImGui::SameLine();
    ImGui::TextColored(flag_Z ? active_color : inactive_color, "Z"); ImGui::SameLine();
    ImGui::TextColored(flag_C ? active_color : inactive_color, "C");
    
    ImGui::Spacing();
    ImGui::Text("N: Negative    V: Overflow");
    ImGui::Text("B: Break       D: Decimal");
    ImGui::Text("I: Interrupt   Z: Zero");
    ImGui::Text("C: Carry");
}

void CPUViewer::UpdateState(uint8_t a, uint8_t x, uint8_t y, uint8_t s, uint8_t p, uint16_t pc) {
    reg_A = a;
    reg_X = x;
    reg_Y = y;
    reg_S = s;
    reg_P = p;
    reg_PC = pc;
    
    // Extract flags from P register
    flag_C = (p & 0x01) != 0;
    flag_Z = (p & 0x02) != 0;
    flag_I = (p & 0x04) != 0;
    flag_D = (p & 0x08) != 0;
    flag_B = (p & 0x10) != 0;
    flag_V = (p & 0x40) != 0;
    flag_N = (p & 0x80) != 0;
}

void CPUViewer::UpdateFlags(bool c, bool z, bool i, bool d, bool b, bool v, bool n) {
    flag_C = c;
    flag_Z = z;
    flag_I = i;
    flag_D = d;
    flag_B = b;
    flag_V = v;
    flag_N = n;
}

void CPUViewer::AppendIO(const std::string& text) {
    if (text.empty()) {
        return;
    }
    io_log.append(text);
    if (io_log.size() > 4096) {
        io_log.erase(0, io_log.size() - 4096);
    }
}

void CPUViewer::ClearIO() {
    io_log.clear();
}

void CPUViewer::RenderIOWindow() {
    ImGui::Text("I/O Output");
    ImGui::Separator();

    ImGui::BeginChild("IOOutput", ImVec2(0, 120), true);
    bool auto_scroll = ImGui::GetScrollY() >= ImGui::GetScrollMaxY();
    if (io_log.empty()) {
        ImGui::TextDisabled("<no output yet>");
    } else {
        ImGui::TextUnformatted(io_log.c_str());
    }
    if (auto_scroll) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
}
