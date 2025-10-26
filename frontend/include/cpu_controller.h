#pragma once
#include <systemc.h>
#include "cpu.h"
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include <condition_variable>

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
    std::string ConsumeIOBuffer();
    
private:
    cpu* cpu_instance;
    sc_clock* clk;
    sc_signal<bool> reset;
    
    std::atomic<bool> is_running;
    std::thread sim_thread;
    std::atomic<bool> thread_shutdown;
    bool systemc_initialized;
    int pending_cycles;

    std::mutex sim_mutex;
    std::condition_variable sim_cv;
    std::condition_variable step_done_cv;

    std::string io_buffer;
    
    void SimulationThread();
    void InitializeSystemC();
    void CleanupSystemC();
    void EnsureSimulationThread();
    void RunCycles(int cycles);
    std::vector<uint8_t> ParseProgram(const std::string& code);
};
