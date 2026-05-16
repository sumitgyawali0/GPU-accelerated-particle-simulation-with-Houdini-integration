#include "cuda/memory.h"

#include "cuda/cuda_utils.h"
#include "cuda/device_particle.h"

template <typename T>
DeviceBuffer<T>::DeviceBuffer(DeviceBuffer&& other) noexcept
    : ptr_(other.ptr_), count_(other.count_)
{
    other.ptr_ = nullptr;
    other.count_ = 0;
}

template <typename T>
DeviceBuffer<T>& DeviceBuffer<T>::operator=(DeviceBuffer<T>&& other) noexcept
{
    if (this != &other)
    {
        free();
        ptr_ = other.ptr_;
        count_ = other.count_;
        other.ptr_ = nullptr;
        other.count_ = 0;
    }
    return *this;
}

template <typename T>
void DeviceBuffer<T>::allocate(std::size_t count)
{
    free();
    if (count == 0)
    {
        return;
    }

    CUDA_CHECK(cudaMalloc(&ptr_, count * sizeof(T)));
    count_ = count;
}

template <typename T>
void DeviceBuffer<T>::free()
{
    if (ptr_ != nullptr)
    {
        cudaFree(ptr_);
        ptr_ = nullptr;
    }
    count_ = 0;
}

template <typename T>
void DeviceBuffer<T>::upload(const T* hostData, std::size_t count)
{
    if (count > count_)
    {
        allocate(count);
    }
    if (count == 0)
    {
        return;
    }
    CUDA_CHECK(cudaMemcpy(ptr_, hostData, count * sizeof(T), cudaMemcpyHostToDevice));
}

template <typename T>
void DeviceBuffer<T>::download(T* hostData, std::size_t count) const
{
    if (ptr_ == nullptr || count == 0)
    {
        return;
    }
    CUDA_CHECK(cudaMemcpy(hostData, ptr_, count * sizeof(T), cudaMemcpyDeviceToHost));
}

template class DeviceBuffer<DeviceParticle>;
