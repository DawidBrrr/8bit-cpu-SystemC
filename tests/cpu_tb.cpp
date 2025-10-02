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

        // TODO: Add more instruction tests here
        // test_ldx_immediate();
        // test_ldy_immediate();
        // test_sta_zeropage();
        // test_adc_immediate();
        // etc...

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
