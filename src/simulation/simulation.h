#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "simulation/cpu_solver.h"
#include "simulation/particle.h"
#include "simulation/solver_backend.h"
#include "utils/particle_export.h"

#ifdef PARTICLE_SIM_CUDA
#include "simulation/gpu_solver.h"
#endif

struct SimulationConfig
{
    int particleCount = 10000;
    int totalFrames = 120;
    float dt = 1.0f / 60.0f;
    Vec3 gravity{0.0f, -9.81f, 0.0f};
    IntegratorType integrator = IntegratorType::Euler;
    SolverBackend backend = SolverBackend::CPU;
    unsigned int randomSeed = 42;
    bool downloadEachFrame = false;

    std::string exportDirectory;
    ExportFormat exportFormat = ExportFormat::Csv;
    bool exportFrames = false;
};

struct SimulationStats
{
    double totalSeconds = 0.0;
    double averageFrameMs = 0.0;
    float fps = 0.0f;
};

class Simulation
{
public:
    explicit Simulation(SimulationConfig config);

    void initialize();
    void run();
    const std::vector<Particle>& particles() const { return particles_; }
    const SimulationStats& stats() const { return stats_; }

    static bool validateGpu(const SimulationConfig& config, float positionTolerance = 1e-3f);

private:
    void runCpu();
    void runGpu();
    void exportFrame(int frame);

    SimulationConfig config_;
    SimulationStats stats_;
    std::vector<Particle> particles_;
    CpuSolver cpuSolver_;

#ifdef PARTICLE_SIM_CUDA
    std::unique_ptr<GpuSolver> gpuSolver_;
#endif
};
