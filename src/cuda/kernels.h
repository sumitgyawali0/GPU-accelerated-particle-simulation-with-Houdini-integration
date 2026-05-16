#pragma once

#include "cuda/device_particle.h"
#include "simulation/integrators.h"

void launchUpdateParticles(DeviceParticle* deviceParticles, int count, float dt, float gravityY,
                           IntegratorType integrator, bool syncAfterLaunch = true);
