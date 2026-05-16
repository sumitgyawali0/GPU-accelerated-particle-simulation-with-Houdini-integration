#include <cstdlib>
#include <iostream>
#include <string>

#include "benchmark/benchmark.h"
#include "simulation/integrators.h"
#include "simulation/simulation.h"
#include "utils/logger.h"
#include "utils/particle_export.h"

namespace
{

void printUsage()
{
    std::cout << "Usage: particle_sim [options]\n"
              << "  --particles <N>   Particle count (default: 10000)\n"
              << "  --frames <N>      Frame count (default: 120)\n"
              << "  --dt <seconds>    Timestep (default: 1/60)\n"
              << "  --integrator <euler|verlet>  Integration method\n"
              << "  --verlet          Shortcut for --integrator verlet\n"
              << "  --seed <N>        Random seed (default: 42)\n"
              << "  --export-dir <path>  Write per-frame particle files\n"
              << "  --export-format <csv|geo|json>  Export format (default: csv)\n"
#ifdef PARTICLE_SIM_CUDA
              << "  --gpu             Run on GPU (CUDA)\n"
              << "  --validate-gpu    Compare CPU vs GPU results\n"
              << "  --benchmark       Run CPU/GPU benchmark suite\n"
              << "  --benchmark-out <path>  CSV output (default: benchmarks/results.csv)\n"
#endif
              ;
}

} // namespace

int main(int argc, char* argv[])
{
    SimulationConfig config;
#ifdef PARTICLE_SIM_CUDA
    bool validateGpu = false;
    bool runBenchmark = false;
    BenchmarkConfig benchConfig;
#endif

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h")
        {
            printUsage();
            return 0;
        }
        if (arg == "--verlet")
        {
            config.integrator = IntegratorType::Verlet;
            continue;
        }
#ifdef PARTICLE_SIM_CUDA
        if (arg == "--gpu")
        {
            config.backend = SolverBackend::GPU;
            continue;
        }
        if (arg == "--validate-gpu")
        {
            validateGpu = true;
            continue;
        }
        if (arg == "--benchmark")
        {
            runBenchmark = true;
            continue;
        }
#endif
        if (i + 1 >= argc)
        {
            Logger::error("Missing value for " + arg);
            printUsage();
            return 1;
        }

        if (arg == "--particles")
        {
            config.particleCount = std::atoi(argv[++i]);
        }
        else if (arg == "--frames")
        {
            config.totalFrames = std::atoi(argv[++i]);
        }
        else if (arg == "--dt")
        {
            config.dt = static_cast<float>(std::atof(argv[++i]));
        }
        else if (arg == "--seed")
        {
            config.randomSeed = static_cast<unsigned int>(std::atoi(argv[++i]));
        }
        else if (arg == "--integrator")
        {
            IntegratorType parsed;
            if (!parseIntegrator(argv[++i], parsed))
            {
                Logger::error("Unknown integrator (use euler or verlet)");
                return 1;
            }
            config.integrator = parsed;
        }
        else if (arg == "--export-dir")
        {
            config.exportDirectory = argv[++i];
            config.exportFrames = true;
        }
        else if (arg == "--export-format")
        {
            if (!parseExportFormat(argv[++i], config.exportFormat))
            {
                Logger::error("Unknown export format (use csv, geo, or json)");
                return 1;
            }
        }
#ifdef PARTICLE_SIM_CUDA
        else if (arg == "--benchmark-out")
        {
            benchConfig.outputPath = argv[++i];
        }
#endif
        else
        {
            Logger::error("Unknown argument: " + arg);
            printUsage();
            return 1;
        }
    }

#ifdef PARTICLE_SIM_CUDA
    if (validateGpu)
    {
        Logger::info("Validating GPU solver against CPU reference...");
        return Simulation::validateGpu(config) ? 0 : 1;
    }

    if (runBenchmark)
    {
        benchConfig.simulation = config;
        benchConfig.simulation.totalFrames =
            config.totalFrames > 0 ? config.totalFrames : 120;
        BenchmarkSuite suite(benchConfig);
        return suite.run() ? 0 : 1;
    }
#endif

    const char* backend =
#ifdef PARTICLE_SIM_CUDA
        config.backend == SolverBackend::GPU ? "GPU" :
#endif
                                             "CPU";

    Logger::info(std::string("CUDA Coursework — particle simulation (") + backend + ", " +
                 integratorName(config.integrator) + ")");
    Logger::info("Particles: " + std::to_string(config.particleCount) +
                 ", frames: " + std::to_string(config.totalFrames) +
                 ", dt: " + std::to_string(config.dt));

    Simulation simulation(config);
    simulation.initialize();
    simulation.run();

    if (!simulation.particles().empty())
    {
        const Particle& p = simulation.particles().front();
        Logger::info("Sample particle[0] pos=(" + std::to_string(p.position.x) + ", " +
                     std::to_string(p.position.y) + ", " + std::to_string(p.position.z) +
                     ") vel=(" + std::to_string(p.velocity.x) + ", " +
                     std::to_string(p.velocity.y) + ", " + std::to_string(p.velocity.z) + ")");
    }

    return 0;
}
