#include <systemc.h>
#include <iostream>
#include <iomanip>
#include "cpu.h"
#include "cpu_defs.h"

// Testbench for CPU instruction testing
SC_MODULE(cpu_tb) {
    sc_signal<bool> clk;
    sc_signal<bool> reset;
    cpu* cpu_i;

    // Test statistics
    int tests_passed = 0;
    int tests_failed = 0;

    // Helper function to load a simple instruction sequence into memory
    void load_instruction(uint16_t addr, uint8_t* bytes, size_t length) {
        for (size_t i = 0; i < length; ++i) {
            cpu_i->memory_i->mem[addr + i] = bytes[i];
        }
    }

    // Helper function to reset the CPU
    void reset_cpu() {
        reset.write(true);
        wait(20, SC_NS);
        reset.write(false);
        wait(10, SC_NS);
    }

    // Helper function to run CPU for N clock cycles
    void run_cycles(int cycles) {
        for (int i = 0; i < cycles; ++i) {
            wait(10, SC_NS);  // One full clock cycle (5ns low + 5ns high)
        }
    }

    // Helper function to check test results
    void check_result(const std::string& test_name, bool passed) {
        if (passed) {
            std::cout << "[PASS] " << test_name << std::endl;
            tests_passed++;
        } else {
            std::cout << "[FAIL] " << test_name << std::endl;
            tests_failed++;
        }
    }

    // Clock generator thread
    void clock_gen() {
        while (true) {
            clk.write(false);
            wait(5, SC_NS);
            clk.write(true);
            wait(5, SC_NS);
        }
    }

    // ===== TEST CASES =====

    // Test LDA immediate (0xA9)
    void test_lda_immediate() {
        std::cout << "\n=== Testing LDA #imm (0xA9) ===" << std::endl;
        
        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        
        // Load instruction: LDA #0x42
        uint8_t program[] = {0xA9, 0x42, 0x00};  // LDA #0x42, BRK
        load_instruction(0x0000, program, 3);
        
        // Reset and run
        reset_cpu();
        run_cycles(10);  // Give enough cycles for instruction to execute
        
        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool test_passed = (reg_a == 0x42);
        
        std::cout << "Expected A: 0x42, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        
        check_result("LDA #0x42", test_passed);
    }

    // Test LDA zero page (0xA5)
    void test_lda_zeropage() {
        std::cout << "\n=== Testing LDA zp (0xA5) ===" << std::endl;
        
        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        
        // Setup: Put test value in zero page
        cpu_i->memory_i->mem[0x10] = 0x55;  // Value at zero page address 0x10
        
        // Load instruction: LDA $10
        uint8_t program[] = {0xA5, 0x10, 0x00};  // LDA $10, BRK
        load_instruction(0x0000, program, 3);
        
        // Reset and run
        reset_cpu();
        run_cycles(10);
        
        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool test_passed = (reg_a == 0x55);
        
        std::cout << "Expected A: 0x55, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        
        check_result("LDA $10", test_passed);
    }

    // Test LDA absolute (0xAD)
    void test_lda_absolute() {
        std::cout << "\n=== Testing LDA abs (0xAD) ===" << std::endl;
        
        // Clear memory
        for (int i = 0; i < 300; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        
        // Setup: Put test value at absolute address
        cpu_i->memory_i->mem[0x0200] = 0x77;  // Value at address 0x0200
        
        // Load instruction: LDA $0200
        uint8_t program[] = {0xAD, 0x00, 0x02, 0x00};  // LDA $0200 (little-endian), BRK
        load_instruction(0x0000, program, 4);
        
        // Reset and run
        reset_cpu();
        run_cycles(15);
        
        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool test_passed = (reg_a == 0x77);
        
        std::cout << "Expected A: 0x77, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        
        check_result("LDA $0200", test_passed);
    }

    // Test LDA with zero flag
    void test_lda_zero_flag() {
        std::cout << "\n=== Testing LDA Zero Flag (0xA9) ===" << std::endl;
        
        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        
        // Load instruction: LDA #0x00 (should set zero flag)
        uint8_t program[] = {0xA9, 0x00, 0x00};  // LDA #0x00, BRK
        load_instruction(0x0000, program, 3);
        
        // Reset and run
        reset_cpu();
        run_cycles(10);
        
        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;  // Z flag is bit 1
        bool test_passed = (reg_a == 0x00) && zero_flag;
        
        std::cout << "Expected A: 0x00, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Expected Zero Flag: 1, Got Zero Flag: " << std::dec << (int)zero_flag << std::endl;
        
        check_result("LDA #0x00 (Zero Flag)", test_passed);
    }

    // Test LDA with negative flag
    void test_lda_negative_flag() {
        std::cout << "\n=== Testing LDA Negative Flag (0xA9) ===" << std::endl;
        
        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        
        // Load instruction: LDA #0x80 (should set negative flag, bit 7 set)
        uint8_t program[] = {0xA9, 0x80, 0x00};  // LDA #0x80, BRK
        load_instruction(0x0000, program, 3);
        
        // Reset and run
        reset_cpu();
        run_cycles(10);
        
        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool negative_flag = (reg_p & 0x80) != 0;  // N flag is bit 7
        bool test_passed = (reg_a == 0x80) && negative_flag;
        
        std::cout << "Expected A: 0x80, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Expected Negative Flag: 1, Got Negative Flag: " << std::dec << (int)negative_flag << std::endl;
        
        check_result("LDA #0x80 (Negative Flag)", test_passed);
    }

    // Test LDX immediate (0xA2)
    void test_ldx_immediate() {
        std::cout << "\n=== Testing LDX Immediate (0xA2) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Load instruction: LDX #0x33
        uint8_t program[] = {0xA2, 0x33, 0x00};  // LDX #0x33, BRK
        load_instruction(0x0000, program, 3);

        // Reset and run
        reset_cpu();
        run_cycles(10);  

        // Check results
        uint8_t reg_x = cpu_i->regfile_i->X;
        bool test_passed = (reg_x == 0x33);

        std::cout << "Expected X: 0x33, Got X: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_x << std::endl;
        
        check_result("LDX #0x33", test_passed);
    }

    // Test LDY immediate (0xA0)
    void test_ldy_immediate() {
        std::cout << "\n=== Testing LDY Immediate (0xA0) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Load instruction: LDY #0x44
        uint8_t program[] = {0xA0, 0x44, 0x00};  // LDY #0x44, BRK
        load_instruction(0x0000, program, 3);

        // Reset and run
        reset_cpu();
        run_cycles(10);  

        // Check results
        uint8_t reg_y = cpu_i->regfile_i->Y;
        bool test_passed = (reg_y == 0x44);

        std::cout << "Expected Y: 0x44, Got Y: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_y << std::endl;
        
        check_result("LDY #0x44", test_passed);
    }

    // Test STA zero page (0x85)
    void test_sta_zp(){
        std::cout << "\n=== Testing STA Zero Page (0x85) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDA #0x99; STA $20
        uint8_t program[] = {0xA9, 0x99, 0x85, 0x20, 0x00};  // LDA #0x99, STA $20, BRK
        load_instruction(0x0000, program, 5);

        // Reset and run
        reset_cpu();
        run_cycles(15);  

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x20];
        bool test_passed = (mem_val == 0x99);

        std::cout << "Expected Mem[0x20]: 0x99, Got Mem[0x20]: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)mem_val << std::endl;
        
        check_result("STA $20", test_passed);
    }

    // Test STA zp,X (0x95)
    void test_sta_zp_x(){
        std::cout << "\n=== Testing STA Zero Page,X (0x95) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDX #0x05; LDA #0x88; STA $20,X
        uint8_t program[] = {0xA2, 0x05, 0xA9, 0x88, 0x95, 0x20, 0x00};  // LDX #0x05, LDA #0x88, STA $20,X, BRK
        load_instruction(0x0000, program, 7);

        // Reset and run
        reset_cpu();
        run_cycles(25);  

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x25]; // 0x20 + X(0x05) = 0x25
        bool test_passed = (mem_val == 0x88);

        std::cout << "Expected Mem[0x25]: 0x88, Got Mem[0x25]: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)mem_val << std::endl;
        
        check_result("STA $20,X", test_passed);
    }
    //Test STA abs (0x8D)
    void test_sta_abs(){
        std::cout << "\n=== Testing STA Absolute (0x8D) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 600; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDA #0x77; STA $0300
        uint8_t program[] = {0xA9, 0x77, 0x8D, 0x00, 0x03, 0x00};  // LDA #0x77, STA $0300, BRK
        load_instruction(0x0000, program, 6);

        // Reset and run
        reset_cpu();
        run_cycles(20);  

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x0300];
        bool test_passed = (mem_val == 0x77);

        std::cout << "Expected Mem[0x0300]: 0x77, Got Mem[0x0300]: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)mem_val << std::endl;
        
        check_result("STA $0300", test_passed);
    }
    // Test STA abs,X (0x9D)
    void test_sta_abs_x(){
        std::cout << "\n=== Testing STA Absolute,X (0x9D) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 600; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDX #0x04; LDA #0x66; STA $0300,X
        uint8_t program[] = {0xA2, 0x04, 0xA9, 0x66, 0x9D, 0x00, 0x03, 0x00};  // LDX #0x04, LDA #0x66, STA $0300,X, BRK
        load_instruction(0x0000, program, 8);

        // Reset and run
        reset_cpu();
        run_cycles(30);  

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x0304]; // 0x0300 + X(0x04) = 0x0304
        bool test_passed = (mem_val == 0x66);

        std::cout << "Expected Mem[0x0304]: 0x66, Got Mem[0x0304]: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)mem_val << std::endl;
        
        check_result("STA $0300,X", test_passed);
    }
    
    // Test STA abs, Y (0x99)
    void test_sta_abs_y(){
        std::cout << "\n=== Testing STA Absolute,Y (0x99) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 600; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDY #0x03; LDA #0x55; STA $0400,Y
        uint8_t program[] = {0xA0, 0x03, 0xA9, 0x55, 0x99, 0x00, 0x04, 0x00};  // LDY #0x03, LDA #0x55, STA $0400,Y, BRK
        load_instruction(0x0000, program, 8);

        // Reset and run
        reset_cpu();
        run_cycles(30);  

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x0403]; // 0x0400 + Y(0x03) = 0x0403
        bool test_passed = (mem_val == 0x55);

        std::cout << "Expected Mem[0x0403]: 0x55, Got Mem[0x0403]: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)mem_val << std::endl;
        
        check_result("STA $0400,Y", test_passed);
    }

    // Test STA (ind, X) 
    void test_sta_ind_x(){
        std::cout << "\n=== Testing STA (ind,X) (0x81) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 600; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Setup indirect address
        cpu_i->memory_i->mem[0x31] = 0x00; // Low byte of target address
        cpu_i->memory_i->mem[0x32] = 0x05; // High byte of target address
        // Target address is $0500

        // Load instruction: LDX #0x01; LDA #0x44; STA ($30,X)
        uint8_t program[] = {0xA2, 0x01, 0xA9, 0x44, 0x81, 0x30, 0x00};  // LDX #0x01, LDA #0x44, STA ($30,X), BRK
        load_instruction(0x0000, program, 7);

        // Reset and run
        reset_cpu();
        run_cycles(40);  

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x0500]; // Target address $0500 
        bool test_passed = (mem_val == 0x44);

        std::cout << "Expected Mem[0x0500]: 0x44, Got Mem[0x0500]: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)mem_val << std::endl;
        
        check_result("STA ($30,X)", test_passed);
    }
    //Test STA (ind), Y (0x91)
    void test_sta_ind_y(){
        std::cout << "\n=== Testing STA (ind),Y (0x91) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 600; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Setup indirect address
        cpu_i->memory_i->mem[0x40] = 0x00; // Low byte of target address
        cpu_i->memory_i->mem[0x41] = 0x05; // High byte of target address
        // Target address is $0500

        // Load instruction: LDY #0x02; LDA #0x33; STA ($40),Y
        uint8_t program[] = {0xA0, 0x02, 0xA9, 0x33, 0x91, 0x40, 0x00};  // LDY #0x02, LDA #0x33, STA ($40),Y, BRK
        load_instruction(0x0000, program, 7);

        // Reset and run
        reset_cpu();
        run_cycles(40);  

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x0502]; // Target address $0500 + Y(0x02) = $0502
        bool test_passed = (mem_val == 0x33);

        std::cout << "Expected Mem[0x0502]: 0x33, Got Mem[0x0502]: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)mem_val << std::endl;
        
        check_result("STA ($40),Y", test_passed);
    }

    // Test STX zp
    void test_stx_zp(){
        std::cout << "\n=== Testing STX Zero Page (0x86) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDX #0x12; STX $30
        uint8_t program[] = {0xA2, 0x12, 0x86, 0x30, 0x00};  // LDX #0x12, STX $30, BRK
        load_instruction(0x0000, program, 5);

        // Reset and run
        reset_cpu();
        run_cycles(15);  

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x30];
        bool test_passed = (mem_val == 0x12);

        std::cout << "Expected Mem[0x30]: 0x12, Got Mem[0x30]: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)mem_val << std::endl;
        
        check_result("STX $30", test_passed);
    }
    //Test STX zp, Y
    void test_stx_zp_y(){
        std::cout << "\n=== Testing STX Zero Page,Y (0x96) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDY #0x03; LDX #0x34; STX $30,Y
        uint8_t program[] = {0xA0, 0x03, 0xA2, 0x34, 0x96, 0x30, 0x00};  // LDY #0x03, LDX #0x34, STX $30,Y, BRK
        load_instruction(0x0000, program, 7);

        // Reset and run
        reset_cpu();
        run_cycles(25);  

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x33]; // 0x30 + Y(0x03) = 0x33
        bool test_passed = (mem_val == 0x34);

        std::cout << "Expected Mem[0x33]: 0x34, Got Mem[0x33]: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)mem_val << std::endl;
        
        check_result("STX $30,Y", test_passed);
    }
    // Test STY zp
    void test_sty_zp(){
        std::cout << "\n=== Testing STY Zero Page (0x84) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDY #0x56; STY $40
        uint8_t program[] = {0xA0, 0x56, 0x84, 0x40, 0x00};  // LDY #0x56, STY $40, BRK
        load_instruction(0x0000, program, 5);

        // Reset and run
        reset_cpu();
        run_cycles(15);  

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x40];
        bool test_passed = (mem_val == 0x56);

        std::cout << "Expected Mem[0x40]: 0x56, Got Mem[0x40]: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)mem_val << std::endl;
        
        check_result("STY $40", test_passed);
    }
    // Test STY zp, X
    void test_sty_zp_x(){
        std::cout << "\n=== Testing STY Zero Page,X (0x94) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDX #0x02; LDY #0x78; STY $40,X
        uint8_t program[] = {0xA2, 0x02, 0xA0, 0x78, 0x94, 0x40, 0x00};  // LDX #0x02, LDY #0x78, STY $40,X, BRK
        load_instruction(0x0000, program, 7);

        // Reset and run
        reset_cpu();
        run_cycles(25);  

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x42]; // 0x40 + X(0x02) = 0x42
        bool test_passed = (mem_val == 0x78);

        std::cout << "Expected Mem[0x42]: 0x78, Got Mem[0x42]: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)mem_val << std::endl;
        
        check_result("STY $40,X", test_passed);
    }

    // Test TAX (0xAA)
    void test_tax() {
        std::cout << "\n=== Testing TAX (0xAA) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDA #0x7F; TAX
        uint8_t program[] = {0xA9, 0x7F, 0xAA, 0x00};  // LDA #0x7F, TAX, BRK
        load_instruction(0x0000, program, 4);

        // Reset and run
        reset_cpu();
        run_cycles(15);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        uint8_t reg_x = cpu_i->regfile_i->X;
        bool test_passed = (reg_a == 0x7F) && (reg_x == 0x7F);

        std::cout << "Expected A: 0x7F, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Expected X: 0x7F, Got X: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_x << std::endl;
        
        check_result("TAX", test_passed);
    }

    // TEST TAY (0xA8)
    void test_tay() {
        std::cout << "\n=== Testing TAY (0xA8) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDA #0x3C; TAY
        uint8_t program[] = {0xA9, 0x3C, 0xA8, 0x00};  // LDA #0x3C, TAY, BRK
        load_instruction(0x0000, program, 4);

        // Reset and run
        reset_cpu();
        run_cycles(15);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        uint8_t reg_y = cpu_i->regfile_i->Y;
        bool test_passed = (reg_a == 0x3C) && (reg_y == 0x3C);

        std::cout << "Expected A: 0x3C, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Expected Y: 0x3C, Got Y: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_y << std::endl;
        
        check_result("TAY", test_passed);        
    }
    // TEST TSX (0xBA)
    void test_tsx() {
        std::cout << "\n=== Testing TSX (0xBA) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Reset first to get initial Stack Pointer value
        reset_cpu();
        uint8_t initial_s = cpu_i->regfile_i->S;  // Get initial Stack Pointer value (should be 0xFF)

        // Load instruction: LDX #0x50; TSX
        uint8_t program[] = {0xA2, 0x50, 0xBA, 0x00};  // LDX #0x50, TSX, BRK
        load_instruction(0x0000, program, 4);

        // Reset and run
        reset_cpu();
        run_cycles(15);  

        // Check results
        uint8_t reg_x = cpu_i->regfile_i->X;
        uint8_t reg_s = cpu_i->regfile_i->S;
        
        // TSX should transfer Stack Pointer to X register
        bool test_passed = (reg_x == initial_s) && (reg_s == initial_s);

        std::cout << "Expected X: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)initial_s << ", Got X: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_x << std::endl;
        std::cout << "Expected S: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)initial_s << ", Got S: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_s << std::endl;
        
        check_result("TSX", test_passed);        
    }
    //Test TXA (0x8A)
    void test_txa() {
        std::cout << "\n=== Testing TXA (0x8A) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDX #0x25; TXA
        uint8_t program[] = {0xA2, 0x25, 0x8A, 0x00};  // LDX #0x25, TXA, BRK
        load_instruction(0x0000, program, 4);

        // Reset and run
        reset_cpu();
        run_cycles(15);  

        // Check results
        uint8_t reg_x = cpu_i->regfile_i->X;
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool test_passed = (reg_x == 0x25) && (reg_a == 0x25);

        std::cout << "Expected X: 0x25, Got X: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_x << std::endl;
        std::cout << "Expected A: 0x25, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        
        check_result("TXA", test_passed);        
    }
    //Test TXS (0x9A)
    void test_txs() {
        std::cout << "\n=== Testing TXS (0x9A) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDX #0x60; TXS
        uint8_t program[] = {0xA2, 0x60, 0x9A, 0x00};  // LDX #0x60, TXS, BRK
        load_instruction(0x0000, program, 4);

        // Reset and run
        reset_cpu();
        run_cycles(15);  

        // Check results
        uint8_t reg_x = cpu_i->regfile_i->X;
        uint8_t reg_s = cpu_i->regfile_i->S;
        bool test_passed = (reg_x == 0x60) && (reg_s == 0x60);

        std::cout << "Expected X: 0x60, Got X: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_x << std::endl;
        std::cout << "Expected S: 0x60, Got S: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_s << std::endl;
        
        check_result("TXS", test_passed);        
    }
    //Test TYA (0x98)
    void test_tya() {
        std::cout << "\n=== Testing TYA (0x98) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDY #0x45; TYA
        uint8_t program[] = {0xA0, 0x45, 0x98, 0x00};  // LDY #0x45, TYA, BRK
        load_instruction(0x0000, program, 4);

        // Reset and run
        reset_cpu();
        run_cycles(15);  

        // Check results
        uint8_t reg_y = cpu_i->regfile_i->Y;
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool test_passed = (reg_y == 0x45) && (reg_a == 0x45);

        std::cout << "Expected Y: 0x45, Got Y: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_y << std::endl;
        std::cout << "Expected A: 0x45, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        
        check_result("TYA", test_passed);        
    }

    // Test PHA
    void test_pha(){
        std::cout << "\n=== Testing PHA (0x48) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 512; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        } 
        // Load instruction: LDA #0xAB; PHA
        uint8_t program[] = {0xA9, 0xAB, 0x48, 0x00};  // LDA #0xAB, PHA, BRK
        load_instruction(0x0000, program, 4);

        // Reset and run
        reset_cpu();
        run_cycles(20);

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        uint8_t reg_s = cpu_i->regfile_i->S;
        uint16_t sp_address = 0x0100 + reg_s + 1; // Stack grows down, so value is at S+1
        uint8_t mem_val = cpu_i->memory_i->mem[sp_address];
        bool test_passed = (reg_a == 0xAB) && (mem_val == 0xAB);
        std::cout << "Expected A: 0xAB, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Expected Mem[0x" << std::hex << std::setw(2) << std::setfill('0') << (int)sp_address 
                  << "]: 0xAB, Got Mem[0x" << std::hex << std::setw(2) << std::setfill('0') << (int)sp_address 
                  << "]: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)mem_val << std::endl;
        check_result("PHA", test_passed);
    }

    // Test PHP
    void test_php(){
        std::cout << "\n=== Testing PHP (0x08) ===" << std::endl;

        for (int i = 0; i < 512; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        uint8_t program[] = {0xA2, 0xFD, 0x9A, 0xA9, 0xAB, 0x08, 0x00};  // LDX #$FD, TXS, LDA #$AB, PHP, BRK
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(40);

        uint8_t reg_p = cpu_i->regfile_i->P;
        uint8_t reg_s = cpu_i->regfile_i->S;
        uint16_t sp_address = 0x0100 + reg_s + 1;
        uint8_t mem_val = cpu_i->memory_i->mem[sp_address];
        uint8_t expected_pushed = reg_p | 0x30;
        bool test_passed = (mem_val == expected_pushed) && (reg_s == 0xFC);

        std::cout << "Expected pushed P: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)expected_pushed << ", Got Mem[0x" << std::setw(4) << std::setfill('0')
                  << sp_address << "]: 0x" << std::setw(2) << std::setfill('0') << (int)mem_val << std::endl;
        std::cout << "Expected SP: 0xFC, Got SP: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)reg_s << std::endl;

        check_result("PHP", test_passed);
    }

    // Test PLA
    void test_pla(){
        std::cout << "\n=== Testing PLA (0x68) ===" << std::endl;

        for (int i = 0; i < 512; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        uint8_t program[] = {0xA2, 0xFD, 0x9A, 0xA9, 0x55, 0x48, 0xA9, 0x00, 0x68, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(50);

        uint8_t reg_a = cpu_i->regfile_i->A;
        uint8_t reg_s = cpu_i->regfile_i->S;
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool test_passed = (reg_a == 0x55) && (reg_s == 0xFD) && !zero_flag && !negative_flag;

        std::cout << "Expected A: 0x55, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)reg_a << std::endl;
        std::cout << "Expected SP: 0xFD, Got SP: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)reg_s << std::endl;
        std::cout << "Z flag cleared? " << (zero_flag ? "no" : "yes")
                  << ", N flag cleared? " << (negative_flag ? "no" : "yes") << std::endl;

        check_result("PLA", test_passed);
    }

    // Test PLP
    void test_plp(){
        std::cout << "\n=== Testing PLP (0x28) ===" << std::endl;

        for (int i = 0; i < 512; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        uint8_t program[] = {0xA2, 0xFE, 0x9A, 0xA9, 0x80, 0x08, 0xA9, 0x00, 0x28, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(50);

        uint8_t reg_p = cpu_i->regfile_i->P;
        uint8_t reg_s = cpu_i->regfile_i->S;
        uint16_t popped_addr = 0x0100 + reg_s;
        uint8_t stacked_value = cpu_i->memory_i->mem[popped_addr];
        uint8_t expected_p = stacked_value | 0x20;
        bool test_passed = (reg_p == expected_p) && (reg_s == 0xFE);

        std::cout << "Expected restored P: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)expected_p << ", Got P: 0x" << std::setw(2) << std::setfill('0')
                  << (int)reg_p << std::endl;
        std::cout << "Expected SP: 0xFE, Got SP: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)reg_s << std::endl;

        check_result("PLP", test_passed);
    }

    // Test AND imm (0x29)
    void test_and_imm(){
        std::cout << "\n=== Testing AND Immediate (0x29) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDA #0xF1; AND #0x0F
        uint8_t program[] = {0xA9, 0xF1, 0x29, 0x0F, 0x00};  // LDA #0xF1, AND #0x0F, BRK
        load_instruction(0x0000, program, 5);

        // Reset and run
        reset_cpu();
        run_cycles(20);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) == 0;
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool test_passed = (reg_a == 0x01) && zero_flag && !negative_flag;

        std::cout << "Expected A: 0x01, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag cleared? " << (negative_flag ? "no" : "yes") << std::endl;

        check_result("AND #$0F", test_passed);
    }
    //Test AND zp
    void test_and_zp(){
        std::cout << "\n=== Testing AND Zero Page (0x25) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Set memory location $10 to 0x3C
        cpu_i->memory_i->mem[0x10] = 0x3C;

        // Load instruction: LDA #0xF1; AND $10
        uint8_t program[] = {0xA9, 0xF1, 0x25, 0x10, 0x00};  // LDA #0xF1, AND $10, BRK
        load_instruction(0x0000, program, 5);

        // Reset and run
        reset_cpu();
        run_cycles(20);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) == 0;
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool test_passed = (reg_a == 0x30) && zero_flag && !negative_flag;

        std::cout << "Expected A: 0x30, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag cleared? " << (negative_flag ? "no" : "yes") << std::endl;

        check_result("AND $10", test_passed);
    }
    //Test AND abs
    void test_and_abs(){
        std::cout << "\n=== Testing AND Absolute (0x2D) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 600; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Set memory location $1234 to 0x5A
        cpu_i->memory_i->mem[0x1234] = 0x5A;

        // Load instruction: LDA #0xFF; AND $1234
        uint8_t program[] = {0xA9, 0xFF, 0x2D, 0x34, 0x12, 0x00};  // LDA #0xFF, AND $1234, BRK
        load_instruction(0x0000, program, 6);

        // Reset and run
        reset_cpu();
        run_cycles(25);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) == 0;
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool test_passed = (reg_a == 0x5A) && zero_flag && !negative_flag;

        std::cout << "Expected A: 0x5A, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag cleared? " << (negative_flag ? "no" : "yes") << std::endl;

        check_result("AND $1234", test_passed);
    }
    //Test ORA imm
    void test_ora_imm(){
        std::cout << "\n=== Testing ORA Immediate (0x09) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDA #0x0F; ORA #0xF0
        uint8_t program[] = {0xA9, 0x0F, 0x09, 0xF0, 0x00};  // LDA #0x0F, ORA #0xF0, BRK
        load_instruction(0x0000, program, 5);

        // Reset and run
        reset_cpu();
        run_cycles(20);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;  // true if Z flag is set
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool test_passed = (reg_a == 0xFF) && !zero_flag && negative_flag;  // Z should be clear (0), N should be set (1)

        std::cout << "Expected A: 0xFF, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("ORA #$F0", test_passed);
    }
    //Test EOR imm (0x49)
    void test_eor_imm(){
        std::cout << "\n=== Testing EOR Immediate (0x49) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDA #0xFF; EOR #0x0F
        uint8_t program[] = {0xA9, 0xFF, 0x49, 0x0F, 0x00};  // LDA #0xFF, EOR #0x0F, BRK
        load_instruction(0x0000, program, 5);

        // Reset and run
        reset_cpu();
        run_cycles(20);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;  // true if Z flag is set
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool test_passed = (reg_a == 0xF0) && !zero_flag && negative_flag;  // Z should be clear (0), N should be set (1)

        std::cout << "Expected A: 0xF0, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("EOR #$0F", test_passed);
    }

    //Test ADC imm (0x69)
    void test_adc_imm(){
        std::cout << "\n=== Testing ADC Immediate (0x69) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDA #0x14; ADC #0x27
        uint8_t program[] = {0xA9, 0x14, 0x69, 0x27, 0x00};  // LDA #0x14, ADC #0x27, BRK
        load_instruction(0x0000, program, 5);

        // Reset and run
        reset_cpu();
        run_cycles(20);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;  // true if Z flag is set
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool carry_flag = (cpu_i->regfile_i->P & 0x01) != 0;
        bool overflow_flag = (cpu_i->regfile_i->P & 0x40) != 0;
        bool test_passed = (reg_a == 0x3B) && !zero_flag && !negative_flag && !carry_flag && !overflow_flag;

        std::cout << "Expected A: 0x3B, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no") 
                  << ", V flag set? " << (overflow_flag ? "yes" : "no") << std::endl;

        check_result("ADC #$27", test_passed);


    }
    //Test ADC zp (0x65)
    void test_adc_zp(){
        std::cout << "\n=== Testing ADC Zero Page (0x65) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Set memory location $20 to 0x30
        cpu_i->memory_i->mem[0x20] = 0x30;

        // Load instruction: LDA #0x10; ADC $20
        uint8_t program[] = {0xA9, 0x10, 0x65, 0x20, 0x00};  // LDA #0x10, ADC $20, BRK
        load_instruction(0x0000, program, 5);

        // Reset and run
        reset_cpu();
        run_cycles(20);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;  // true if Z flag is set
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool carry_flag = (cpu_i->regfile_i->P & 0x01) != 0;
        bool overflow_flag = (cpu_i->regfile_i->P & 0x40) != 0;
        bool test_passed = (reg_a == 0x40) && !zero_flag && !negative_flag && !carry_flag && !overflow_flag;

        std::cout << "Expected A: 0x40, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no") 
                  << ", V flag set? " << (overflow_flag ? "yes" : "no") << std::endl;

        check_result("ADC $20", test_passed);
    }
    //Test ADC abs (0x6D)
    void test_adc_abs(){
        std::cout << "\n=== Testing ADC Absolute (0x6D) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 600; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Set memory location $2000 to 0x50
        cpu_i->memory_i->mem[0x2000] = 0x50;

        // Load instruction: LDA #0x20; ADC $2000
        uint8_t program[] = {0xA9, 0x20, 0x6D, 0x00, 0x20, 0x00};  // LDA #0x20, ADC $2000, BRK
        load_instruction(0x0000, program, 6);

        // Reset and run
        reset_cpu();
        run_cycles(25);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;  // true if Z flag is set
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool carry_flag = (cpu_i->regfile_i->P & 0x01) != 0;
        bool overflow_flag = (cpu_i->regfile_i->P & 0x40) != 0;
        bool test_passed = (reg_a == 0x70) && !zero_flag && !negative_flag && !carry_flag && !overflow_flag;

        std::cout << "Expected A: 0x70, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no") 
                  << ", V flag set? " << (overflow_flag ? "yes" : "no") << std::endl;

        check_result("ADC $2000", test_passed);
    }
    //Test ADC (ind,X) (0x61)
    void test_adc_ind_x(){
        std::cout << "\n=== Testing ADC (Indirect,X) (0x61) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 600; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Set up indirect address
        cpu_i->memory_i->mem[0x31] = 0x00;      // Low byte of effective address
        cpu_i->memory_i->mem[0x32] = 0x20;      // High byte of effective address
        cpu_i->memory_i->mem[0x2000] = 0x25;    // Value at effective address

        // Load instruction: LDX #$01; STX $30; LDA #$10; ADC ($30,X)
        uint8_t program[] = {0xA2, 0x01, 0x86, 0x30, 0xA9, 0x10, 0x61, 0x30, 0x00}; // LDX #$01, STX $30, LDA #$10, ADC ($30,X), BRK
        load_instruction(0x0000, program, sizeof(program));

        // Reset and run
        reset_cpu();
        run_cycles(40);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;  // true if Z flag is set
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool carry_flag = (cpu_i->regfile_i->P & 0x01) != 0;
        bool overflow_flag = (cpu_i->regfile_i->P & 0x40) != 0;
        bool test_passed = (reg_a == 0x35) && !zero_flag && !negative_flag && !carry_flag && !overflow_flag;

        std::cout << "Expected A: 0x35, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no") 
                  << ", V flag set? " << (overflow_flag ? "yes" : "no") << std::endl;
        check_result("ADC ($30,X)", test_passed);
    }
    //Test SBC #imm (0xE9)
    void test_sbc_imm(){
        std::cout << "\n=== Testing SBC Immediate (0xE9) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        // Load instruction: LDA #0x50; SEC; SBC #0x20
        uint8_t program[] = {0xA9, 0x50, 0x38, 0xE9, 0x20, 0x00};  // LDA #0x50, SEC, SBC #0x20, BRK
        load_instruction(0x0000, program, 6);

        // Reset and run
        reset_cpu();
        run_cycles(25);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;  // true if Z flag is set
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool carry_flag = (cpu_i->regfile_i->P & 0x01) != 0;
        bool overflow_flag = (cpu_i->regfile_i->P & 0x40) != 0;
        bool test_passed = (reg_a == 0x30) && !zero_flag && !negative_flag && carry_flag && !overflow_flag;

        std::cout << "Expected A: 0x30, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no") 
                  << ", V flag set? " << (overflow_flag ? "yes" : "no") << std::endl;

        check_result("SBC #$20", test_passed);
    }
    //Test SNC zp (0xE5)
    void test_sbc_zp(){
        std::cout << "\n=== Testing SBC Zero Page (0xE5) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Set memory location $40 to 0x15
        cpu_i->memory_i->mem[0x40] = 0x15;

        // Load instruction: LDA #0x30; SEC; SBC $40
        uint8_t program[] = {0xA9, 0x30, 0x38, 0xE5, 0x40, 0x00};  // LDA #0x30, SEC, SBC $40, BRK
        load_instruction(0x0000, program, 6);

        // Reset and run
        reset_cpu();
        run_cycles(25);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;  // true if Z flag is set
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool carry_flag = (cpu_i->regfile_i->P & 0x01) != 0;
        bool overflow_flag = (cpu_i->regfile_i->P & 0x40) != 0;
        bool test_passed = (reg_a == 0x1B) && !zero_flag && !negative_flag && carry_flag && !overflow_flag;

        std::cout << "Expected A: 0x1B, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no") 
                  << ", V flag set? " << (overflow_flag ? "yes" : "no") << std::endl;

        check_result("SBC $40", test_passed);
    }
    //Test SBC abs (0xED)
    void test_sbc_abs(){
        std::cout << "\n=== Testing SBC Absolute (0xED) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 600; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Set memory location $3000 to 0x25
        cpu_i->memory_i->mem[0x3000] = 0x25;

        // Load instruction: LDA #0x50; SEC; SBC $3000
        uint8_t program[] = {0xA9, 0x50, 0x38, 0xED, 0x00, 0x30, 0x00};  // LDA #0x50, SEC, SBC $3000, BRK
        load_instruction(0x0000, program, 7);

        // Reset and run
        reset_cpu();
        run_cycles(30);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;  // true if Z flag is set
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool carry_flag = (cpu_i->regfile_i->P & 0x01) != 0;
        bool overflow_flag = (cpu_i->regfile_i->P & 0x40) != 0;
        bool test_passed = (reg_a == 0x2B) && !zero_flag && !negative_flag && carry_flag && !overflow_flag;

        std::cout << "Expected A: 0x2B, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no") 
                  << ", V flag set? " << (overflow_flag ? "yes" : "no") << std::endl;

        check_result("SBC $3000", test_passed);
    }
    //Test SBC (ind,X) (0xE1)
    void test_sbc_ind_x(){
        std::cout << "\n=== Testing SBC (Indirect,X) (0xE1) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 600; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Set up indirect address
        cpu_i->memory_i->mem[0x41] = 0x00;      // Low byte of effective address
        cpu_i->memory_i->mem[0x42] = 0x30;      // High byte of effective address
        cpu_i->memory_i->mem[0x3000] = 0x10;    // Value at effective address

        // Load instruction: LDX #$01; STX $40; LDA #$20; SEC; SBC ($40,X)
        uint8_t program[] = {0xA2, 0x01, 0x86, 0x40, 0xA9, 0x20, 0xE1, 0x40, 0x00}; // LDX #$01, STX $40, LDA #$20, SEC, SBC ($40,X), BRK
        load_instruction(0x0000, program, sizeof(program));

        // Reset and run
        reset_cpu();
        run_cycles(40);  

        // Check results
        uint8_t reg_a = cpu_i->regfile_i->A;
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;  // true if Z flag is set
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool carry_flag = (cpu_i->regfile_i->P & 0x01) != 0;
        bool overflow_flag = (cpu_i->regfile_i->P & 0x40) != 0;
        bool test_passed = (reg_a == 0x10) && !zero_flag && !negative_flag && carry_flag && !overflow_flag;

        std::cout << "Expected A: 0x10, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)reg_a << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no") 
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no") 
                  << ", V flag set? " << (overflow_flag ? "yes" : "no") << std::endl;

        check_result("SBC ($40,X)", test_passed);
    }

    //Test DEC zp (0xC6)
    void test_dec_zp(){
        std::cout << "\n=== Testing DEC Zero Page (0xC6) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Set memory location $20 to 0x01
        cpu_i->memory_i->mem[0x20] = 0x01;

        // Load instruction: DEC $20
        uint8_t program[] = {0xC6, 0x20, 0x00};  // DEC $20, BRK
        load_instruction(0x0000, program, sizeof(program));

        // Reset and run
        reset_cpu();
        run_cycles(20);

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x20];
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool test_passed = (mem_val == 0x00) && zero_flag && !negative_flag;

        std::cout << "Expected Mem[0x20]: 0x00, Got Mem[0x20]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)mem_val << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag cleared? " << (negative_flag ? "no" : "yes") << std::endl;

        check_result("DEC $20", test_passed);
    }

    //Test DEC abs (0xCE)
    void test_dec_abs(){
        std::cout << "\n=== Testing DEC Absolute (0xCE) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 600; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Set memory location $3000 to 0x00
        cpu_i->memory_i->mem[0x3000] = 0x00;

        // Load instruction: DEC $3000
        uint8_t program[] = {0xCE, 0x00, 0x30, 0x00};  // DEC $3000, BRK
        load_instruction(0x0000, program, sizeof(program));

        // Reset and run
        reset_cpu();
        run_cycles(25);

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x3000];
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool test_passed = (mem_val == 0xFF) && !zero_flag && negative_flag;

        std::cout << "Expected Mem[0x3000]: 0xFF, Got Mem[0x3000]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)mem_val << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("DEC $3000", test_passed);
    }

    //Test INC zp (0xE6)
    void test_inc_zp(){
        std::cout << "\n=== Testing INC Zero Page (0xE6) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 100; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Set memory location $10 to 0x7F
        cpu_i->memory_i->mem[0x10] = 0x7F;

        // Load instruction: INC $10
        uint8_t program[] = {0xE6, 0x10, 0x00};  // INC $10, BRK
        load_instruction(0x0000, program, sizeof(program));

        // Reset and run
        reset_cpu();
        run_cycles(20);

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x10];
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool test_passed = (mem_val == 0x80) && !zero_flag && negative_flag;

        std::cout << "Expected Mem[0x10]: 0x80, Got Mem[0x10]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)mem_val << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("INC $10", test_passed);
    }

    //Test INC abs (0xEE)
    void test_inc_abs(){
        std::cout << "\n=== Testing INC Absolute (0xEE) ===" << std::endl;

        // Clear memory
        for (int i = 0; i < 600; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        // Set memory location $2000 to 0xFF
        cpu_i->memory_i->mem[0x2000] = 0xFF;

        // Load instruction: INC $2000
        uint8_t program[] = {0xEE, 0x00, 0x20, 0x00};  // INC $2000, BRK
        load_instruction(0x0000, program, sizeof(program));

        // Reset and run
        reset_cpu();
        run_cycles(25);

        // Check results
        uint8_t mem_val = cpu_i->memory_i->mem[0x2000];
        bool zero_flag = (cpu_i->regfile_i->P & 0x02) != 0;
        bool negative_flag = (cpu_i->regfile_i->P & 0x80) != 0;
        bool test_passed = (mem_val == 0x00) && zero_flag && !negative_flag;

        std::cout << "Expected Mem[0x2000]: 0x00, Got Mem[0x2000]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)mem_val << std::endl;
        std::cout << "Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag cleared? " << (negative_flag ? "no" : "yes") << std::endl;

        check_result("INC $2000", test_passed);
    }

    void test_asl_a(){
        std::cout << "\n=== Testing ASL A (0x0A) ===" << std::endl;

        for (int i = 0; i < 64; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        uint8_t program[] = {0xA9, 0x81, 0x0A, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(20);

        uint8_t reg_a = cpu_i->regfile_i->A;
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool carry_flag = (reg_p & 0x01) != 0;
        bool test_passed = (reg_a == 0x02) && !zero_flag && !negative_flag && carry_flag;

        std::cout << "Expected A: 0x02, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)reg_a << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no")
                  << ", Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("ASL A", test_passed);
    }

    void test_asl_zp(){
        std::cout << "\n=== Testing ASL Zero Page (0x06) ===" << std::endl;

        for (int i = 0; i < 128; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        cpu_i->memory_i->mem[0x20] = 0xC0;

        uint8_t program[] = {0x06, 0x20, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(20);

        uint8_t mem_val = cpu_i->memory_i->mem[0x20];
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool carry_flag = (reg_p & 0x01) != 0;
        bool test_passed = (mem_val == 0x80) && !zero_flag && negative_flag && carry_flag;

        std::cout << "Expected Mem[0x20]: 0x80, Got Mem[0x20]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)mem_val << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no")
                  << ", Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("ASL $20", test_passed);
    }

    void test_asl_abs(){
        std::cout << "\n=== Testing ASL Absolute (0x0E) ===" << std::endl;

        for (int i = 0; i < 1024; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        cpu_i->memory_i->mem[0x1234] = 0x40;

        uint8_t program[] = {0x0E, 0x34, 0x12, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(25);

        uint8_t mem_val = cpu_i->memory_i->mem[0x1234];
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool carry_flag = (reg_p & 0x01) != 0;
        bool test_passed = (mem_val == 0x80) && !zero_flag && negative_flag && !carry_flag;

        std::cout << "Expected Mem[0x1234]: 0x80, Got Mem[0x1234]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)mem_val << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no")
                  << ", Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("ASL $1234", test_passed);
    }

    void test_lsr_a(){
        std::cout << "\n=== Testing LSR A (0x4A) ===" << std::endl;

        for (int i = 0; i < 64; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        uint8_t program[] = {0xA9, 0x03, 0x4A, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(20);

        uint8_t reg_a = cpu_i->regfile_i->A;
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool carry_flag = (reg_p & 0x01) != 0;
        bool test_passed = (reg_a == 0x01) && !zero_flag && !negative_flag && carry_flag;

        std::cout << "Expected A: 0x01, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)reg_a << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no")
                  << ", Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("LSR A", test_passed);
    }

    void test_lsr_zp(){
        std::cout << "\n=== Testing LSR Zero Page (0x46) ===" << std::endl;

        for (int i = 0; i < 128; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        cpu_i->memory_i->mem[0x21] = 0x02;

        uint8_t program[] = {0x46, 0x21, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(20);

        uint8_t mem_val = cpu_i->memory_i->mem[0x21];
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool carry_flag = (reg_p & 0x01) != 0;
        bool test_passed = (mem_val == 0x01) && !zero_flag && !negative_flag && !carry_flag;

        std::cout << "Expected Mem[0x21]: 0x01, Got Mem[0x21]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)mem_val << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no")
                  << ", Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("LSR $21", test_passed);
    }

    void test_lsr_abs(){
        std::cout << "\n=== Testing LSR Absolute (0x4E) ===" << std::endl;

        for (int i = 0; i < 1024; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        cpu_i->memory_i->mem[0x2345] = 0x01;

        uint8_t program[] = {0x4E, 0x45, 0x23, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(25);

        uint8_t mem_val = cpu_i->memory_i->mem[0x2345];
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool carry_flag = (reg_p & 0x01) != 0;
        bool test_passed = (mem_val == 0x00) && zero_flag && !negative_flag && carry_flag;

        std::cout << "Expected Mem[0x2345]: 0x00, Got Mem[0x2345]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)mem_val << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no")
                  << ", Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("LSR $2345", test_passed);
    }

    void test_rol_a(){
        std::cout << "\n=== Testing ROL A (0x2A) ===" << std::endl;

        for (int i = 0; i < 64; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        uint8_t program[] = {0xA9, 0x80, 0x38, 0x2A, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(25);

        uint8_t reg_a = cpu_i->regfile_i->A;
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool carry_flag = (reg_p & 0x01) != 0;
        bool test_passed = (reg_a == 0x01) && !zero_flag && !negative_flag && carry_flag;

        std::cout << "Expected A: 0x01, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)reg_a << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no")
                  << ", Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("ROL A", test_passed);
    }

    void test_rol_zp(){
        std::cout << "\n=== Testing ROL Zero Page (0x26) ===" << std::endl;

        for (int i = 0; i < 256; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        cpu_i->memory_i->mem[0x30] = 0x40;

        uint8_t program[] = {0x18, 0x26, 0x30, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(25);

        uint8_t mem_val = cpu_i->memory_i->mem[0x30];
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool carry_flag = (reg_p & 0x01) != 0;
        bool test_passed = (mem_val == 0x80) && !zero_flag && negative_flag && !carry_flag;

        std::cout << "Expected Mem[0x30]: 0x80, Got Mem[0x30]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)mem_val << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no")
                  << ", Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("ROL $30", test_passed);
    }

    void test_rol_abs(){
        std::cout << "\n=== Testing ROL Absolute (0x2E) ===" << std::endl;

        for (int i = 0; i < 2048; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        cpu_i->memory_i->mem[0x4000] = 0x80;

        uint8_t program[] = {0x38, 0x2E, 0x00, 0x40, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(30);

        uint8_t mem_val = cpu_i->memory_i->mem[0x4000];
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool carry_flag = (reg_p & 0x01) != 0;
        bool test_passed = (mem_val == 0x01) && !zero_flag && !negative_flag && carry_flag;

        std::cout << "Expected Mem[0x4000]: 0x01, Got Mem[0x4000]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)mem_val << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no")
                  << ", Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("ROL $4000", test_passed);
    }

    void test_ror_a(){
        std::cout << "\n=== Testing ROR A (0x6A) ===" << std::endl;

        for (int i = 0; i < 64; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        uint8_t program[] = {0xA9, 0x01, 0x38, 0x6A, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(25);

        uint8_t reg_a = cpu_i->regfile_i->A;
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool carry_flag = (reg_p & 0x01) != 0;
        bool test_passed = (reg_a == 0x80) && !zero_flag && negative_flag && carry_flag;

        std::cout << "Expected A: 0x80, Got A: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)reg_a << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no")
                  << ", Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("ROR A", test_passed);
    }

    void test_ror_zp(){
        std::cout << "\n=== Testing ROR Zero Page (0x66) ===" << std::endl;

        for (int i = 0; i < 256; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        cpu_i->memory_i->mem[0x40] = 0x02;

        uint8_t program[] = {0x18, 0x66, 0x40, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(25);

        uint8_t mem_val = cpu_i->memory_i->mem[0x40];
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool carry_flag = (reg_p & 0x01) != 0;
        bool test_passed = (mem_val == 0x01) && !zero_flag && !negative_flag && !carry_flag;

        std::cout << "Expected Mem[0x40]: 0x01, Got Mem[0x40]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)mem_val << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no")
                  << ", Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("ROR $40", test_passed);
    }

    void test_ror_abs(){
        std::cout << "\n=== Testing ROR Absolute (0x6E) ===" << std::endl;

        for (int i = 0; i < 4096; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }
        cpu_i->memory_i->mem[0x5000] = 0x01;

        uint8_t program[] = {0x38, 0x6E, 0x00, 0x50, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        reset_cpu();
        run_cycles(30);

        uint8_t mem_val = cpu_i->memory_i->mem[0x5000];
        uint8_t reg_p = cpu_i->regfile_i->P;
        bool zero_flag = (reg_p & 0x02) != 0;
        bool negative_flag = (reg_p & 0x80) != 0;
        bool carry_flag = (reg_p & 0x01) != 0;
        bool test_passed = (mem_val == 0x80) && !zero_flag && negative_flag && carry_flag;

        std::cout << "Expected Mem[0x5000]: 0x80, Got Mem[0x5000]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)mem_val << std::endl;
        std::cout << "C flag set? " << (carry_flag ? "yes" : "no")
                  << ", Z flag set? " << (zero_flag ? "yes" : "no")
                  << ", N flag set? " << (negative_flag ? "yes" : "no") << std::endl;

        check_result("ROR $5000", test_passed);
    }

    void test_jmp_abs(){
        std::cout << "\n=== Testing JMP Absolute (0x4C) ===" << std::endl;

        for (int i = 0; i < 0x0200; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        uint8_t program[] = {0x4C, 0x10, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        cpu_i->memory_i->mem[0x0010] = 0x00;

        reset_cpu();
        run_cycles(20);

        uint16_t pc = cpu_i->pc_val.to_uint();
        bool test_passed = (pc == 0x0010);

        std::cout << "Expected PC: 0x0010, Got PC: 0x" << std::hex << std::setw(4) << std::setfill('0')
                  << pc << std::dec << std::endl;

        check_result("JMP $0010", test_passed);
    }

    void test_jmp_ind(){
        std::cout << "\n=== Testing JMP Indirect (0x6C) ===" << std::endl;

        for (int i = 0; i < 0x0300; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        uint8_t program[] = {0x6C, 0x30, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        cpu_i->memory_i->mem[0x0030] = 0x20;
        cpu_i->memory_i->mem[0x0031] = 0x01;
        cpu_i->memory_i->mem[0x0120] = 0x00;

        reset_cpu();
        run_cycles(25);

        uint16_t pc = cpu_i->pc_val.to_uint();
        bool test_passed = (pc == 0x0120);

        std::cout << "Expected PC: 0x0120, Got PC: 0x" << std::hex << std::setw(4) << std::setfill('0')
                  << pc << std::dec << std::endl;

        check_result("JMP ($0030)", test_passed);
    }

    void test_jsr_abs(){
        std::cout << "\n=== Testing JSR Absolute (0x20) ===" << std::endl;

        for (int i = 0; i < 0x0200; ++i) {
            cpu_i->memory_i->mem[i] = 0x00;
        }

        uint8_t program[] = {0x20, 0x10, 0x00};
        load_instruction(0x0000, program, sizeof(program));

        cpu_i->memory_i->mem[0x0010] = 0x00;

        reset_cpu();
        run_cycles(35);

        uint16_t pc = cpu_i->pc_val.to_uint();
        uint8_t sp = cpu_i->regfile_i->S;
        uint8_t stack_high = cpu_i->memory_i->mem[0x01FF];
        uint8_t stack_low = cpu_i->memory_i->mem[0x01FE];

        bool pc_ok = (pc == 0x0010);
        bool sp_ok = (sp == 0xFD);
        bool stack_ok = (stack_high == 0x00) && (stack_low == 0x02);
        bool test_passed = pc_ok && sp_ok && stack_ok;

        std::cout << "Expected PC: 0x0010, Got PC: 0x" << std::hex << std::setw(4) << std::setfill('0')
                  << pc << std::dec << std::endl;
        std::cout << "Expected SP: 0xFD, Got SP: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)sp << std::dec << std::endl;
        std::cout << "Stack[0x01FF]: 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)stack_high << ", Stack[0x01FE]: 0x" << std::setw(2) << std::setfill('0')
                  << (int)stack_low << std::dec << std::endl;

        check_result("JSR $0010", test_passed);
    }

    // Main test runner
    void run_tests() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "   CPU Instruction Test Suite" << std::endl;
        std::cout << "========================================" << std::endl;

        // Run all test cases
        test_lda_immediate();
        test_lda_zeropage();
        test_lda_absolute();
        test_lda_zero_flag();
        test_lda_negative_flag();
        test_ldx_immediate();
        test_ldy_immediate();
        test_sta_zp();
        test_sta_zp_x();
        test_sta_abs();
        test_sta_abs_x();
        test_sta_abs_y();
        test_sta_ind_x();
        test_sta_ind_y();
        test_stx_zp();
        test_stx_zp_y();
        test_sty_zp();
        test_sty_zp_x();
        test_tax();
        test_tay();
        test_tsx();
        test_txa();
        test_txs();
        test_tya();
        test_pha();
        test_php();
        test_pla();
        test_plp();
        test_and_imm();
        test_and_zp();
        test_and_abs();
        test_ora_imm();
        test_eor_imm();
        test_adc_imm();
        test_adc_zp();
        test_adc_abs();
        test_adc_ind_x();
        test_sbc_imm();
        test_sbc_zp();
        test_sbc_abs();
        test_sbc_ind_x();
        test_dec_zp();
        test_dec_abs();
        test_inc_zp();
        test_inc_abs();
        test_asl_a();
        test_asl_zp();
        test_asl_abs();
        test_lsr_a();
        test_lsr_zp();
        test_lsr_abs();
        test_rol_a();
        test_rol_zp();
        test_rol_abs();
        test_ror_a();
        test_ror_zp();
        test_ror_abs();
        test_jmp_abs();
        test_jmp_ind();
        test_jsr_abs();
        

        

        // Print summary
        std::cout << "\n========================================" << std::endl;
        std::cout << "   Test Summary" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Tests Passed: " << std::dec << tests_passed << std::endl;
        std::cout << "Tests Failed: " << std::dec << tests_failed << std::endl;
        std::cout << "Total Tests:  " << std::dec << (tests_passed + tests_failed) << std::endl;
        std::cout << "========================================" << std::endl;

        sc_stop();
    }

    SC_CTOR(cpu_tb) {
        // Instantiate CPU
        cpu_i = new cpu("cpu_i");
        cpu_i->clk(clk);
        cpu_i->reset(reset);

        // Start threads
        SC_THREAD(clock_gen);
        SC_THREAD(run_tests);
    }

    ~cpu_tb() {
        delete cpu_i;
    }
};

int sc_main(int argc, char* argv[]) {
    cpu_tb tb("cpu_tb");
    sc_start();
    return 0;
}
