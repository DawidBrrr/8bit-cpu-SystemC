#pragma once
#include <systemc.h>
#include <iostream>
using namespace std;


// Regfile 6502: A, X, Y, S (stos), P (status)
SC_MODULE(regfile) {
    sc_in<bool> clk;
    sc_in<bool> we; // write enable
    sc_in<sc_uint<3>> w_addr; // 0: A, 1: X, 2: Y, 3: S, 4: P
    sc_in<sc_uint<8>> w_data;
    sc_in<sc_uint<3>> r_addr; // wybór rejestru do odczytu
    sc_out<sc_uint<8>> r_data;
    sc_in<bool> set_flags; // czy ustawiać flagi Z i N
    sc_in<bool> zero;      // flaga zero z ALU
    sc_in<bool> negative;  // flaga negative z ALU
    
    // Sygnały do bezpośredniej kontroli flag P
    sc_in<bool> set_carry;         // ustaw flagę Carry
    sc_in<bool> clear_carry;       // wyczyść flagę Carry
    sc_in<bool> set_interrupt;     // ustaw flagę Interrupt Disable
    sc_in<bool> clear_interrupt;   // wyczyść flagę Interrupt Disable
    sc_in<bool> set_decimal;       // ustaw flagę Decimal Mode
    sc_in<bool> clear_decimal;     // wyczyść flagę Decimal Mode
    sc_in<bool> clear_overflow;    // wyczyść flagę Overflow

    // Rejestry 6502
    sc_uint<8> A; // akumulator
    sc_uint<8> X; // rejestr X
    sc_uint<8> Y; // rejestr Y
    sc_uint<8> S; // stos (Stack Pointer)
    sc_uint<8> P; // status (Processor Status)
    
    // Śledzenie poprzedniego stanu WE
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
            // Ustaw flagi Z i N jeśli set_flags
            if (set_flags.read()) {
                P = (P & ~0x82) | (zero.read() ? 0x02 : 0) | (negative.read() ? 0x80 : 0);
            }
        }
        
        // Obsługa bezpośredniej kontroli flag P
        if (set_carry.read()) {
            P |= 0x01;  // ustaw bit 0 (Carry)
            std::cout << "REGFILE: Set Carry flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        if (clear_carry.read()) {
            P &= ~0x01; // wyczyść bit 0 (Carry)
            std::cout << "REGFILE: Clear Carry flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        if (set_interrupt.read()) {
            P |= 0x04;  // ustaw bit 2 (Interrupt Disable)
            std::cout << "REGFILE: Set Interrupt flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        if (clear_interrupt.read()) {
            P &= ~0x04; // wyczyść bit 2 (Interrupt Disable)
            std::cout << "REGFILE: Clear Interrupt flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        if (set_decimal.read()) {
            P |= 0x08;  // ustaw bit 3 (Decimal Mode)
            std::cout << "REGFILE: Set Decimal flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        if (clear_decimal.read()) {
            P &= ~0x08; // wyczyść bit 3 (Decimal Mode)
            std::cout << "REGFILE: Clear Decimal flag, P = 0x" << std::hex << (int)P << std::endl;
        }
        if (clear_overflow.read()) {
            P &= ~0x40; // wyczyść bit 6 (Overflow)
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
