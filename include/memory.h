#pragma once
#include <systemc.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
using namespace std;


// Pamięć RAM 64KB (16-bitowa przestrzeń adresowa) jak w 6502
SC_MODULE(memory) {
    sc_in<bool> clk;
    sc_in<bool> we; // write enable
    sc_in<sc_uint<16>> addr; // adres 16-bitowy
    sc_in<sc_uint<8>> w_data; // dane do zapisu
    sc_out<sc_uint<8>> r_data; // dane odczytane

    sc_uint<8> mem[65536]; // 64KB pamięci
    
    // Osobny plik dla portów I/O
    static ofstream io_output;
    static bool io_file_opened;

    void process() {
        if (we.read()) {
            sc_uint<16> address = addr.read();
            sc_uint<8> data = w_data.read();
            
            // Sprawdź czy to zapis do specjalnych adresów I/O
            if (address == 0xFF00) {
                // I/O PORT 0: Wyświetl wartość jako liczbę dziesiętną
                string output = "PORT 0 (DEC): " + to_string((int)data);
                cout << "*** OUTPUT " << output << " ***" << endl;
                write_to_io_file(to_string((int)data));
            }
            else if (address == 0xFF01) {
                // I/O PORT 1: Wyświetl wartość jako hex
                char hex_str[20];
                char hex_strw[20];
                sprintf(hex_str, "PORT 1 (HEX): 0x%02x", (int)data);
                sprintf(hex_strw, "0x%02x", (int)data);
                string output = hex_str;
                cout << "*** OUTPUT " << output << " ***" << endl;
                write_to_io_file(string(hex_strw));
            }
            else if (address == 0xFF02) {
                // I/O PORT 2: Wyświetl jako znak ASCII
                string output = "PORT 2 (CHR): '" + string(1, (char)data) + "'";
                cout << "*** OUTPUT " << output << " ***" << endl;
                write_to_io_file(string(1, (char)data));
            }
            else if (address == 0xFF03) {
                // I/O PORT 3: Wyświetl jako liczbę binarną
                string binary = "";
                for (int i = 7; i >= 0; i--) {
                    binary += ((data >> i) & 1) ? "1" : "0";
                }
                string output = "PORT 3 (BIN): " + binary;
                cout << "*** OUTPUT " << output << " ***" << endl;
                write_to_io_file(binary);
            }
            else {
                // Normalny zapis do pamięci
                mem[address] = data;
                cout << "MEMORY WRITE: addr=0x" << hex << address 
                     << " data=0x" << data << dec << endl;
            }
        } else {
            // Odczyt z pamięci (tylko gdy nie zapisujemy)
            r_data.write(mem[addr.read()]);
            cout << "MEMORY READ: addr=0x" << hex << addr.read() 
                 << " data=0x" << mem[addr.read()] << dec << endl;
        }
    }
    
    void write_to_io_file(const string& output) {
        if (!io_file_opened) {
            io_output.open("../output/io_output.txt", ios::out);
            io_file_opened = true;
            if (io_output.is_open()) {
                io_output << "=== CPU 6502 - Output Ports Log ===" << endl;
                io_output << "Format: [TIME] PORT_MESSAGE" << endl;
                io_output << "=====================================" << endl;
            }
        }
        
        if (io_output.is_open()) {
            // Dodaj timestamp (uproszczony)
            static int counter = 0;
            //io_output << "[" << setfill('0') << setw(4) << ++counter << "] " << output << endl;
            io_output << output;
            io_output.flush(); // Natychmiastowy zapis
        }
        
    }

    SC_CTOR(memory) {
        SC_METHOD(process);
        sensitive << clk.pos();
    }
    
    ~memory() {
        if (io_output.is_open()) {
            io_output.close();
        }
    }
};
