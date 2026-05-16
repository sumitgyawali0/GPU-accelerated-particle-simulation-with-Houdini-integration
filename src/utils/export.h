#pragma once

#include <string>
#include <vector>

#include "benchmark/benchmark.h"

bool writeBenchmarkCsv(const std::string& path, const std::vector<BenchmarkRow>& rows);
