#include "simulation/simulation.h"

#include <algorithm>
#include <cmath>
#include <random>

#include "utils/logger.h"
#include "utils/timer.h"

#ifdef PARTICLE_SIM_CUDA
#include "simulation/gpu_solver.h"
#endif

namespace
{

float randomRange(std::mt19937& rng, float minVal, float maxVal)
{
    std::uniform_real_distribution<float> dist(minVal, maxVal);
    return dist(rng);
}

void initializeParticles(std::vector<Particle>& particles, const SimulationConfig& config)
{
    std::mt19937 rng(config.randomSeed);
    for (auto& particle : particles)
    {
        particle.position = {randomRange(rng, -2.0f, 2.0f), randomRange(rng, 0.0f, 4.0f),
                             randomRange(rng, -2.0f, 2.0f)};
        particle.velocity = {randomRange(rng, -0.5f, 0.5f), randomRange(rng, -0.5f, 0.5f),
                             randomRange(rng, -0.5f, 0.5f)};
        particle.previousPosition = particle.position - particle.velocity * config.dt;
        particle.mass = 1.0f;
        particle.force = {};

        const float speed = length(particle.velocity);
        const float t = std::min(speed / 10.0f, 1.0f);
        particle.color = {t, t, 1.0f};
    }
}

float maxPositionError(const std::vector<Particle>& a, const std::vector<Particle>& b)
{
    float maxError = 0.0f;
    const std::size_t count = std::min(a.size(), b.size());
    for (std::size_t i = 0; i < count; ++i)
    {
        const Vec3 delta = a[i].position - b[i].position;
        maxError = std::max(maxError, length(delta));
    }
    return maxError;
}

} // namespace

Simulation::Simulation(SimulationConfig config) : config_(std::move(config))
{
#ifdef PARTICLE_SIM_CUDA
    if (config_.backend == SolverBackend::GPU)
    {
        gpuSolver_ = std::make_unique<GpuSolver>();
    }
    if (config_.exportFrames)
    {
        config_.downloadEachFrame = true;
    }
#endif

#if !defined(PARTICLE_SIM_CUDA)
    if (config_.exportFrames && config_.backend == SolverBackend::GPU)
    {
        Logger::warn("GPU export requested but CUDA not built — using CPU");
        config_.backend = SolverBackend::CPU;
    }
#endif
}

void Simulation::initialize()
{
    particles_.resize(static_cast<size_t>(config_.particleCount));
    initializeParticles(particles_, config_);

    const char* backendName =
#ifdef PARTICLE_SIM_CUDA
        config_.backend == SolverBackend::GPU ? "GPU" :
#endif
                                              "CPU";

    Logger::info("Initialized " + std::to_string(particles_.size()) + " particles (" +
                 backendName + ")");

    if (config_.exportFrames)
    {
        exportFrame(0);
        Logger::info("Exporting frames to: " + config_.exportDirectory + " (" +
                     exportFormatName(config_.exportFormat) + ")");
    }
}

void Simulation::exportFrame(int frame)
{
    if (!config_.exportFrames)
    {
        return;
    }

    if (!exportParticlesFrame(config_.exportDirectory, frame, particles_, config_.exportFormat))
    {
        Logger::error("Failed to export frame " + std::to_string(frame));
    }
}

void Simulation::runCpu()
{
    for (int frame = 0; frame < config_.totalFrames; ++frame)
    {
        cpuSolver_.step(particles_, config_.dt, config_.gravity, config_.integrator);
        exportFrame(frame + 1);
    }
}

void Simulation::runGpu()
{
#ifdef PARTICLE_SIM_CUDA
    if (!gpuSolver_)
    {
        Logger::error("GPU solver not available — rebuild with BUILD_CUDA=ON");
        return;
    }

    gpuSolver_->upload(particles_);

    for (int frame = 0; frame < config_.totalFrames; ++frame)
    {
        gpuSolver_->step(config_.dt, config_.gravity, config_.integrator);
        if (config_.downloadEachFrame || config_.exportFrames)
        {
            gpuSolver_->download(particles_);
            exportFrame(frame + 1);
        }
    }

    if (!config_.downloadEachFrame && !config_.exportFrames)
    {
        gpuSolver_->download(particles_);
    }
#else
    Logger::error("CUDA support not compiled in");
#endif
}

void Simulation::run()
{
    Timer timer;
    timer.start();

#ifdef PARTICLE_SIM_CUDA
    if (config_.backend == SolverBackend::GPU)
    {
        runGpu();
    }
    else
#endif
    {
        runCpu();
    }

    stats_.totalSeconds = timer.elapsedSeconds();
    stats_.averageFrameMs =
        (stats_.totalSeconds * 1000.0) / static_cast<double>(config_.totalFrames);
    stats_.fps = static_cast<float>(config_.totalFrames / stats_.totalSeconds);

    Logger::info("Simulation complete: " + std::to_string(config_.totalFrames) + " frames in " +
                 std::to_string(stats_.totalSeconds) + " s");
    Logger::info("Average frame: " + std::to_string(stats_.averageFrameMs) + " ms (" +
                 std::to_string(stats_.fps) + " FPS)");
}

bool Simulation::validateGpu(const SimulationConfig& config, float positionTolerance)
{
#ifndef PARTICLE_SIM_CUDA
    Logger::error("CUDA support not compiled in — cannot validate GPU");
    (void)config;
    (void)positionTolerance;
    return false;
#else
    SimulationConfig cpuConfig = config;
    cpuConfig.backend = SolverBackend::CPU;
    cpuConfig.exportFrames = false;

    SimulationConfig gpuConfig = config;
    gpuConfig.backend = SolverBackend::GPU;
    gpuConfig.downloadEachFrame = false;
    gpuConfig.exportFrames = false;

    Simulation cpuSim(cpuConfig);
    Simulation gpuSim(gpuConfig);

    cpuSim.initialize();
    gpuSim.initialize();

    cpuSim.run();
    gpuSim.run();

    const float error = maxPositionError(cpuSim.particles(), gpuSim.particles());
    Logger::info("CPU vs GPU max position error: " + std::to_string(error));

    if (error <= positionTolerance)
    {
        Logger::info("GPU validation passed (tolerance " + std::to_string(positionTolerance) +
                     ")");
        return true;
    }

    Logger::error("GPU validation failed (tolerance " + std::to_string(positionTolerance) + ")");
    return false;
#endif
}
