#pragma once

#include <string>
#include <vector>

#include "simulation/simulation.h"

struct BenchmarkRow
{
    int particleCount = 0;
    std::string backend;
    std::string integrator;
    int frames = 0;

    double frameTimeMs = 0.0;
    double kernelTimeMs = 0.0;
    double transferTimeMs = 0.0;
    double totalSeconds = 0.0;
    float fps = 0.0f;
    float speedup = 0.0f;
};

struct BenchmarkConfig
{
    SimulationConfig simulation;
    std::vector<int> particleCounts{10000, 100000, 1000000};
    int warmupFrames = 10;
    std::string outputPath = "benchmarks/results.csv";
};

class BenchmarkSuite
{
public:
    explicit BenchmarkSuite(BenchmarkConfig config);

    bool run();
    const std::vector<BenchmarkRow>& results() const { return results_; }

private:
    BenchmarkRow runCpu(int particleCount);
#ifdef PARTICLE_SIM_CUDA
    BenchmarkRow runGpu(int particleCount);
#endif

    BenchmarkConfig config_;
    std::vector<BenchmarkRow> results_;
};
