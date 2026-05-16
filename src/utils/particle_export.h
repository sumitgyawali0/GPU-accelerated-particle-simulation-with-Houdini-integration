#pragma once

#include <string>
#include <vector>

#include "simulation/particle.h"

enum class ExportFormat
{
    Csv,
    Geo,
    Json
};

bool ensureExportDirectory(const std::string& directory);

bool exportParticlesFrame(const std::string& directory, int frame,
                          const std::vector<Particle>& particles, ExportFormat format);

bool parseExportFormat(const std::string& value, ExportFormat& out);

const char* exportFormatName(ExportFormat format);
