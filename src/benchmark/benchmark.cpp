#include "benchmark/benchmark.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <random>
#include <vector>

#include "simulation/cpu_solver.h"
#include "simulation/integrators.h"
#include "utils/export.h"
#include "utils/logger.h"
#include "utils/timer.h"

#ifdef PARTICLE_SIM_CUDA
#include "cuda/cuda_timer.h"
#include "cuda/cuda_utils.h"
#include "simulation/gpu_solver.h"
#endif

namespace
{

void initializeParticles(std::vector<Particle>& particles, const SimulationConfig& config)
{
    std::mt19937 rng(config.randomSeed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for (auto& particle : particles)
    {
        particle.position = {-2.0f + dist(rng) * 4.0f, dist(rng) * 4.0f, -2.0f + dist(rng) * 4.0f};
        particle.velocity = {-0.5f + dist(rng), -0.5f + dist(rng), -0.5f + dist(rng)};
        particle.previousPosition = particle.position - particle.velocity * config.dt;
        particle.mass = 1.0f;
        particle.force = {};
    }
}

} // namespace

BenchmarkSuite::BenchmarkSuite(BenchmarkConfig config) : config_(std::move(config)) {}

BenchmarkRow BenchmarkSuite::runCpu(int particleCount)
{
    BenchmarkRow row;
    row.particleCount = particleCount;
    row.backend = "cpu";
    row.integrator = integratorName(config_.simulation.integrator);
    row.frames = config_.simulation.totalFrames;

    std::vector<Particle> particles(static_cast<size_t>(particleCount));
    initializeParticles(particles, config_.simulation);

    CpuSolver solver;

    for (int i = 0; i < config_.warmupFrames; ++i)
    {
        solver.step(particles, config_.simulation.dt, config_.simulation.gravity,
                    config_.simulation.integrator);
    }

    Timer timer;
    timer.start();

    for (int frame = 0; frame < config_.simulation.totalFrames; ++frame)
    {
        solver.step(particles, config_.simulation.dt, config_.simulation.gravity,
                    config_.simulation.integrator);
    }

    row.totalSeconds = timer.elapsedSeconds();
    row.frameTimeMs = (row.totalSeconds * 1000.0) / static_cast<double>(row.frames);
    row.kernelTimeMs = row.frameTimeMs;
    row.transferTimeMs = 0.0;
    row.fps = static_cast<float>(row.frames / row.totalSeconds);

    return row;
}

#ifdef PARTICLE_SIM_CUDA
BenchmarkRow BenchmarkSuite::runGpu(int particleCount)
{
    BenchmarkRow row;
    row.particleCount = particleCount;
    row.backend = "gpu";
    row.integrator = integratorName(config_.simulation.integrator);
    row.frames = config_.simulation.totalFrames;

    std::vector<Particle> particles(static_cast<size_t>(particleCount));
    initializeParticles(particles, config_.simulation);

    GpuSolver solver;

    Timer uploadTimer;
    uploadTimer.start();
    solver.upload(particles);
    row.transferTimeMs = uploadTimer.elapsedSeconds() * 1000.0;

    for (int i = 0; i < config_.warmupFrames; ++i)
    {
        solver.step(config_.simulation.dt, config_.simulation.gravity, config_.simulation.integrator,
                    true);
    }

    CudaTimer kernelTimer;
    kernelTimer.start();
    for (int frame = 0; frame < config_.simulation.totalFrames; ++frame)
    {
        solver.step(config_.simulation.dt, config_.simulation.gravity, config_.simulation.integrator,
                    false);
    }
    CUDA_CHECK(cudaDeviceSynchronize());
    row.kernelTimeMs = static_cast<double>(kernelTimer.elapsedMs());

    Timer downloadTimer;
    downloadTimer.start();
    solver.download(particles);
    row.transferTimeMs += downloadTimer.elapsedSeconds() * 1000.0;

    row.totalSeconds = (row.kernelTimeMs + row.transferTimeMs) / 1000.0;
    row.frameTimeMs = row.kernelTimeMs / static_cast<double>(row.frames);
    row.fps = static_cast<float>(row.frames / (row.kernelTimeMs / 1000.0));

    return row;
}
#endif

bool BenchmarkSuite::run()
{
    results_.clear();

#ifdef PARTICLE_SIM_CUDA
    CUDA_CHECK(cudaFree(0));
#endif

    Logger::info("Benchmark suite — integrator: " +
                 std::string(integratorName(config_.simulation.integrator)));
    Logger::info("Frames: " + std::to_string(config_.simulation.totalFrames) +
                 ", warmup: " + std::to_string(config_.warmupFrames));

    for (int count : config_.particleCounts)
    {
        Logger::info("--- " + std::to_string(count) + " particles ---");

        BenchmarkRow cpuRow = runCpu(count);
        results_.push_back(cpuRow);

        Logger::info("CPU  frame: " + std::to_string(cpuRow.frameTimeMs) + " ms, FPS: " +
                     std::to_string(cpuRow.fps));

#ifdef PARTICLE_SIM_CUDA
        BenchmarkRow gpuRow = runGpu(count);
        gpuRow.speedup = (cpuRow.frameTimeMs > 0.0)
                             ? static_cast<float>(cpuRow.frameTimeMs / gpuRow.frameTimeMs)
                             : 0.0f;
        results_.push_back(gpuRow);

        Logger::info("GPU  kernel: " + std::to_string(gpuRow.kernelTimeMs) + " ms total, " +
                     std::to_string(gpuRow.frameTimeMs) + " ms/frame");
        Logger::info("GPU  transfer: " + std::to_string(gpuRow.transferTimeMs) + " ms");
        Logger::info("GPU  speedup: " + std::to_string(gpuRow.speedup) + "x vs CPU kernel");
#else
        (void)count;
#endif
    }

    const std::filesystem::path outputPath(config_.outputPath);
    if (outputPath.has_parent_path())
    {
        std::filesystem::create_directories(outputPath.parent_path());
    }

    if (!writeBenchmarkCsv(config_.outputPath, results_))
    {
        Logger::error("Failed to write " + config_.outputPath);
        return false;
    }

    Logger::info("Wrote " + config_.outputPath);
    return true;
}
