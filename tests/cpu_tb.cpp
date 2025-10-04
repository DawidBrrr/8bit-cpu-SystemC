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
        uint8_t mem_val = cpu_i->memory_i->mem[0x0500]; // Target address $0500 + X(0x01) = $0501
        bool test_passed = (mem_val == 0x44);

        std::cout << "Expected Mem[0x0500]: 0x44, Got Mem[0x0500]: 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)mem_val << std::endl;
        
        check_result("STA ($30,X)", test_passed);
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
