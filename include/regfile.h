#pragma once
#include <systemc.h>
#include <iostream>
using namespace std;


// Regfile : A, X, Y, S (stos), P (status)
SC_MODULE(regfile) {
    sc_in<bool> clk;
    sc_in<bool> we; // write enable
    sc_in<sc_uint<3>> w_addr; // 0: A, 1: X, 2: Y, 3: S, 4: P
    sc_in<sc_uint<8>> w_data;
    sc_in<sc_uint<3>> r_addr; // Choose which register to read
    sc_out<sc_uint<8>> r_data;
    sc_in<bool> set_flags; // Should flags Z and N be set based on last write
    sc_in<bool> zero;      // Zero flag from ALU
    sc_in<bool> negative;  // Negative flag from ALU
    
    // Signals for direct control of P flags
    sc_in<bool> set_carry;         // set Carry flag
    sc_in<bool> clear_carry;       // clear Carry flag
    sc_in<bool> set_interrupt;     // set Interrupt Disable flag
    sc_in<bool> clear_interrupt;   // clear Interrupt Disable flag
    sc_in<bool> set_decimal;       // set Decimal Mode flag
    sc_in<bool> clear_decimal;     // clear Decimal Mode flag
    sc_in<bool> clear_overflow;    // clear Overflow flag

    // Rejestry 6502
    sc_uint<8> A; // accumulator
    sc_uint<8> X; // X register
    sc_uint<8> Y; // Y register
    sc_uint<8> S; // stack (Stack Pointer)
    sc_uint<8> P; // status (Processor Status)
    
    // Internal signal to remember previous we state
    bool prev_we;

    void process() {
        if (we.read()) {
            switch(w_addr.read()) {
                case 0: A = w_data.read(); std::cout << "REGFILE: A = 0x" << std::hex << (int)A << std::endl; break;
                case 1: X = w_data.read(); std::cout << "REGFILE: X = 0x" << std::hex << (int)X << std::endl; break;
                case 2: Y = w_data.read(); std::cout << "REGFILE: Y = 0x" << std::hex << (int)Y << std::endl; break;
                case 3: S = w_data.read(); std::cout << "REGFILE: S = 0x" << std::hex << (int)S << std::endl; break;
                case 4: P = w_data.read(); std::cout << "REGFILE: P = 0x" << std::hex << (int)P << std::endl; break;
                default: break;
            }
            // Set Z and N flags if set_flags
            if (set_flags.read()) {
                P = (P & ~0x82) | (zero.read() ? 0x02 : 0) | (negative.read() ? 0x80 : 0);
            }
        }
        
        // Signals for direct control of P flags
        if (set_carry.read()) {
            P |= 0x01;  // set bit 0 (Carry)
            std::cout << "REGFILE: Set Carry flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        if (clear_carry.read()) {
            P &= ~0x01; // clear bit 0 (Carry)
            std::cout << "REGFILE: Clear Carry flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        if (set_interrupt.read()) {
            P |= 0x04;  // set bit 2 (Interrupt Disable)
            std::cout << "REGFILE: Set Interrupt flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        if (clear_interrupt.read()) {
            P &= ~0x04; // clear bit 2 (Interrupt Disable)
            std::cout << "REGFILE: Clear Interrupt flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        if (set_decimal.read()) {
            P |= 0x08;  // set bit 3 (Decimal Mode)
            std::cout << "REGFILE: Set Decimal flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        if (clear_decimal.read()) {
            P &= ~0x08; // clear bit 3 (Decimal Mode)
            std::cout << "REGFILE: Clear Decimal flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        if (clear_overflow.read()) {
            P &= ~0x40; // clear bit 6 (Overflow)
            std::cout << "REGFILE: Clear Overflow flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        
        switch(r_addr.read()) {
            case 0: r_data.write(A); break;
            case 1: r_data.write(X); break;
            case 2: r_data.write(Y); break;
            case 3: r_data.write(S); break;
            case 4: r_data.write(P); break;
            default: r_data.write(0);
        }
    }

    SC_CTOR(regfile) : A(0), X(0), Y(0), S(0xFF), P(0x20) {
        SC_METHOD(process);
        sensitive << clk.pos() << set_carry << clear_carry << set_interrupt << clear_interrupt 
                  << set_decimal << clear_decimal << clear_overflow;
    }
};
