#pragma once
#include "Simulation.h"
class IncompressibleSimulation :
    public Simulation
{
public:
    IncompressibleSimulation(int width, int height, float particleSize, float kernelSupport, float fluidDensity, float viscosity, float gravity);
private:
    // compute pressures solving a linear system
	std::vector<float> computePressures(const std::vector<std::vector<unsigned>>& neighborVector, float timeDifference) const override;
};
