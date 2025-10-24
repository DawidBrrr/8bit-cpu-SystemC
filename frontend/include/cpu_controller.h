#pragma once
#include <systemc.h>
#include "cpu.h"
#include <string>
#include <thread>
#include <atomic>

class CPUController {
public:
    CPUController();
    ~CPUController();
    
    void LoadProgram(const std::string& code);
    void Start();
    void Stop();
    void Reset();
    void Step();
    
    bool IsRunning() const { return is_running; }
    
    // Get current CPU state
    void GetCPUState(uint8_t& a, uint8_t& x, uint8_t& y, uint8_t& s, uint8_t& p, uint16_t& pc);
    
private:
    cpu* cpu_instance;
    sc_clock* clk;
    sc_signal<bool> reset;
    
    std::atomic<bool> is_running;
    std::thread* sim_thread;
    
    void SimulationThread();
    void InitializeSystemC();
    void CleanupSystemC();
};
