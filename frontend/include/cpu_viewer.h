#pragma once
#include "imgui.h"
#include <cstdint>

class CPUViewer {
public:
    CPUViewer();
    void Render();
    
    // Update CPU state from simulator
    void UpdateState(uint8_t a, uint8_t x, uint8_t y, uint8_t s, uint8_t p, uint16_t pc);
    void UpdateFlags(bool c, bool z, bool i, bool d, bool b, bool v, bool n);
    
private:
    // CPU Registers
    uint8_t reg_A;
    uint8_t reg_X;
    uint8_t reg_Y;
    uint8_t reg_S;  // Stack Pointer
    uint8_t reg_P;  // Status Register
    uint16_t reg_PC; // Program Counter
    
    // Status flags (extracted from P register)
    bool flag_C; // Carry
    bool flag_Z; // Zero
    bool flag_I; // Interrupt Disable
    bool flag_D; // Decimal Mode
    bool flag_B; // Break
    bool flag_V; // Overflow
    bool flag_N; // Negative
    
    void RenderRegisters();
    void RenderFlags();
};
