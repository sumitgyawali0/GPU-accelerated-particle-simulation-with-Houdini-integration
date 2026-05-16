#include "utils/particle_export.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace
{

std::string frameFilename(int frame, const char* extension)
{
    std::ostringstream name;
    name << "particles_" << std::setw(4) << std::setfill('0') << frame << '.' << extension;
    return name.str();
}

void writeCsv(std::ostream& out, const std::vector<Particle>& particles)
{
    out << "px,py,pz,vx,vy,vz,mass,Cd_r,Cd_g,Cd_b\n";
    out << std::fixed << std::setprecision(6);

    for (const Particle& particle : particles)
    {
        out << particle.position.x << ',' << particle.position.y << ',' << particle.position.z
            << ',' << particle.velocity.x << ',' << particle.velocity.y << ','
            << particle.velocity.z << ',' << particle.mass << ',' << particle.color.x << ','
            << particle.color.y << ',' << particle.color.z << '\n';
    }
}

void writeGeo(std::ostream& out, const std::vector<Particle>& particles)
{
    const std::size_t count = particles.size();

    out << "PGEOMETRY V5\n";
    out << "NPoints " << count << " NPrims 0\n";
    out << "NPointGroups 0 NPrimGroups 0\n";
    out << "NPointAttrib 4 NVertexAttrib 0 NPrimAttrib 0 NAttrib 0\n";
    out << "PointAttrib\n";
    out << "mass 1 float 0\n";
    out << "v 3 vector 0\n";
    out << "Cd 3 vector 1\n";
    out << "up 3 vector 0\n";
    out << count << " 3 float 1\n";
    out << std::fixed << std::setprecision(6);

    for (const Particle& particle : particles)
    {
        out << particle.position.x << ' ' << particle.position.y << ' ' << particle.position.z
            << ' ' << particle.velocity.x << ' ' << particle.velocity.y << ' '
            << particle.velocity.z << ' ' << particle.mass << ' ' << particle.color.x << ' '
            << particle.color.y << ' ' << particle.color.z << " 0 0 1\n";
    }
}

void writeJson(std::ostream& out, int frame, const std::vector<Particle>& particles)
{
    out << std::fixed << std::setprecision(6);
    out << "{\n  \"frame\": " << frame << ",\n  \"count\": " << particles.size()
        << ",\n  \"particles\": [\n";

    for (std::size_t i = 0; i < particles.size(); ++i)
    {
        const Particle& p = particles[i];
        out << "    {\"P\": [" << p.position.x << ", " << p.position.y << ", " << p.position.z
            << "], \"v\": [" << p.velocity.x << ", " << p.velocity.y << ", " << p.velocity.z
            << "], \"mass\": " << p.mass << ", \"Cd\": [" << p.color.x << ", " << p.color.y
            << ", " << p.color.z << "]}";
        if (i + 1 < particles.size())
        {
            out << ',';
        }
        out << '\n';
    }

    out << "  ]\n}\n";
}

} // namespace

bool ensureExportDirectory(const std::string& directory)
{
    std::error_code error;
    std::filesystem::create_directories(directory, error);
    return !error;
}

bool parseExportFormat(const std::string& value, ExportFormat& out)
{
    if (value == "csv")
    {
        out = ExportFormat::Csv;
        return true;
    }
    if (value == "geo")
    {
        out = ExportFormat::Geo;
        return true;
    }
    if (value == "json")
    {
        out = ExportFormat::Json;
        return true;
    }
    return false;
}

const char* exportFormatName(ExportFormat format)
{
    switch (format)
    {
    case ExportFormat::Geo:
        return "geo";
    case ExportFormat::Json:
        return "json";
    case ExportFormat::Csv:
    default:
        return "csv";
    }
}

bool exportParticlesFrame(const std::string& directory, int frame,
                          const std::vector<Particle>& particles, ExportFormat format)
{
    if (!ensureExportDirectory(directory))
    {
        return false;
    }

    const char* extension = exportFormatName(format);
    const std::filesystem::path path =
        std::filesystem::path(directory) / frameFilename(frame, extension);

    std::ofstream file(path);
    if (!file)
    {
        return false;
    }

    switch (format)
    {
    case ExportFormat::Geo:
        writeGeo(file, particles);
        break;
    case ExportFormat::Json:
        writeJson(file, frame, particles);
        break;
    case ExportFormat::Csv:
    default:
        writeCsv(file, particles);
        break;
    }

    return true;
}
