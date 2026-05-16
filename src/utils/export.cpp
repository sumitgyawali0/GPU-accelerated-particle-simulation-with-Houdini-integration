#include "utils/export.h"

#include <fstream>

bool writeBenchmarkCsv(const std::string& path, const std::vector<BenchmarkRow>& rows)
{
    std::ofstream file(path);
    if (!file)
    {
        return false;
    }

    file << "particles,backend,integrator,frames,frame_ms,kernel_ms,transfer_ms,total_s,fps,speedup\n";

    for (const BenchmarkRow& row : rows)
    {
        file << row.particleCount << ',' << row.backend << ',' << row.integrator << ','
             << row.frames << ',' << row.frameTimeMs << ',' << row.kernelTimeMs << ','
             << row.transferTimeMs << ',' << row.totalSeconds << ',' << row.fps << ','
             << row.speedup << '\n';
    }

    return true;
}
