#pragma once

#include <cstddef>

template <typename T>
class DeviceBuffer
{
public:
    DeviceBuffer() = default;
    ~DeviceBuffer() { free(); }

    DeviceBuffer(const DeviceBuffer&) = delete;
    DeviceBuffer& operator=(const DeviceBuffer&) = delete;

    DeviceBuffer(DeviceBuffer&& other) noexcept;
    DeviceBuffer& operator=(DeviceBuffer&& other) noexcept;

    void allocate(std::size_t count);
    void free();

    void upload(const T* hostData, std::size_t count);
    void download(T* hostData, std::size_t count) const;

    T* devicePtr() { return ptr_; }
    const T* devicePtr() const { return ptr_; }
    std::size_t count() const { return count_; }
    std::size_t byteSize() const { return count_ * sizeof(T); }

private:
    T* ptr_ = nullptr;
    std::size_t count_ = 0;
};
