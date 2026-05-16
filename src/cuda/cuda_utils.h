#pragma once

#include <cuda_runtime.h>

#include <stdexcept>
#include <string>

inline void cudaCheck(cudaError_t result, const char* file, int line)
{
    if (result != cudaSuccess)
    {
        throw std::runtime_error(std::string("CUDA error at ") + file + ":" + std::to_string(line) +
                                 " — " + cudaGetErrorString(result));
    }
}

#define CUDA_CHECK(call) cudaCheck((call), __FILE__, __LINE__)

inline int blockCount(int elementCount, int blockSize)
{
    return (elementCount + blockSize - 1) / blockSize;
}
