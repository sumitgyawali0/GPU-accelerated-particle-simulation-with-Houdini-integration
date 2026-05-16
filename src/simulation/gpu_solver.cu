#include "simulation/gpu_solver.h"

#include <vector>

#include "cuda/cuda_utils.h"
#include "cuda/device_particle.h"
#include "cuda/kernels.h"
#include "cuda/memory.h"

namespace
{

DeviceParticle toDevice(const Particle& host)
{
    DeviceParticle device{};
    device.px = host.position.x;
    device.py = host.position.y;
    device.pz = host.position.z;
    device.vx = host.velocity.x;
    device.vy = host.velocity.y;
    device.vz = host.velocity.z;
    device.mass = host.mass;
    device.prevPx = host.previousPosition.x;
    device.prevPy = host.previousPosition.y;
    device.prevPz = host.previousPosition.z;
    return device;
}

void toHost(const DeviceParticle& device, Particle& host)
{
    host.position = {device.px, device.py, device.pz};
    host.velocity = {device.vx, device.vy, device.vz};
    host.mass = device.mass;
    host.previousPosition = {device.prevPx, device.prevPy, device.prevPz};
}

} // namespace

struct GpuSolver::Impl
{
    DeviceBuffer<DeviceParticle> particles;
    std::vector<DeviceParticle> hostStaging;
};

GpuSolver::GpuSolver() : impl_(std::make_unique<Impl>()) {}

GpuSolver::~GpuSolver() = default;

void GpuSolver::upload(const std::vector<Particle>& hostParticles)
{
    impl_->hostStaging.resize(hostParticles.size());
    for (std::size_t i = 0; i < hostParticles.size(); ++i)
    {
        impl_->hostStaging[i] = toDevice(hostParticles[i]);
    }

    impl_->particles.upload(impl_->hostStaging.data(), impl_->hostStaging.size());
}

void GpuSolver::step(float dt, const Vec3& gravity, IntegratorType integrator,
                     bool syncAfterLaunch)
{
    launchUpdateParticles(impl_->particles.devicePtr(),
                          static_cast<int>(impl_->particles.count()), dt, gravity.y,
                          integrator, syncAfterLaunch);
}

void GpuSolver::download(std::vector<Particle>& hostParticles)
{
    if (impl_->hostStaging.size() != hostParticles.size())
    {
        impl_->hostStaging.resize(hostParticles.size());
    }

    impl_->particles.download(impl_->hostStaging.data(), impl_->hostStaging.size());

    for (std::size_t i = 0; i < hostParticles.size(); ++i)
    {
        toHost(impl_->hostStaging[i], hostParticles[i]);
    }
}

std::size_t GpuSolver::particleCount() const
{
    return impl_->particles.count();
}
