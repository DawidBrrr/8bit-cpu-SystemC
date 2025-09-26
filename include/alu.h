#pragma once
#include <systemc.h>
#include <iostream>


// ALU 
SC_MODULE(alu) {
    sc_in<sc_uint<8>> a;
    sc_in<sc_uint<8>> b;
    sc_in<sc_uint<8>> carry_in; // wejście carry (np. z rejestru statusu)
    sc_in<sc_uint<4>> op; // kod operacji 4 bity
    sc_out<sc_uint<8>> result;
    sc_out<bool> carry;
    sc_out<bool> zero;
    sc_out<bool> negative;
    sc_out<bool> overflow;

    void process() {
        sc_uint<8> res = 0;
        bool c = false, v = false, n = false, z = false;
        switch(op.read()) {
            case 0x0: // ADC
            {
                sc_uint<9> tmp = (sc_uint<9>)a.read() + (sc_uint<9>)b.read() + (carry_in.read() & 1);
                res = tmp.range(7,0);
                c = tmp[8];
                v = (~(a.read() ^ b.read()) & (a.read() ^ res)) & 0x80;
                /*
                std::cout << "ALU DEBUG: ADC a=" << std::hex << (int)a.read() 
                          << " b=" << (int)b.read() << " carry=" << (int)carry_in.read()
                          << " tmp=" << (int)tmp << " res=" << (int)res << std::endl;
                */
                break;
            }
            case 0x1: // SBC
            {
                sc_uint<9> tmp = (sc_uint<9>)a.read() - (sc_uint<9>)b.read() - (1 - (carry_in.read() & 1));
                res = tmp.range(7,0);
                c = !tmp[8];
                v = ((a.read() ^ b.read()) & (a.read() ^ res)) & 0x80;
                break;
            }
            case 0x2: res = a.read() & b.read(); break; // AND
            case 0x3: res = a.read() | b.read(); break; // ORA
            case 0x4: res = a.read() ^ b.read(); break; // EOR
            case 0x5: res = a.read() + 1; break; // INC
            case 0x6: res = a.read() - 1; break; // DEC
            case 0x7: res = a.read() << 1; c = a.read()[7]; break; // ASL
            case 0x8: res = a.read() >> 1; c = a.read()[0]; break; // LSR
            case 0x9: res = (a.read() << 1) | (carry_in.read() & 1); c = a.read()[7]; break; // ROL
            case 0xA: res = (a.read() >> 1) | ((carry_in.read() & 1) << 7); c = a.read()[0]; break; // ROR
            case 0xB: res = a.read(); break; // MOV (LDA, LDX, LDY)
            default: res = 0;
        }
        n = res[7];
        z = (res == 0);
        result.write(res);
        carry.write(c);
        zero.write(z);
        negative.write(n);
        overflow.write(v);
    }

    SC_CTOR(alu) {
        SC_METHOD(process);
        sensitive << a << b << carry_in << op;
    }
};
