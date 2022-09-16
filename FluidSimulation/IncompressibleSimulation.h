#pragma once
#include "IO.h"
#include "Simulation.h"
class IncompressibleSimulation :
    public Simulation
{
public:
    IncompressibleSimulation(int width, int height, float particleSize, float fluidDensity, float viscosity, float gravity, IO* io);
private:
    // compute pressures solving a linear system
	void computePressures(const std::vector<std::vector<unsigned>>& neighborVector, float timeDifference) override;
};

