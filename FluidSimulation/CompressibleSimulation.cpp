#include "CompressibleSimulation.h"

CompressibleSimulation::CompressibleSimulation(int width, int height, float particleSize, float kernelSupport, float fluidDensity, float viscosity, float gravity, float stiffness)
	: Simulation(width, height, particleSize, kernelSupport, fluidDensity, viscosity, gravity)
{
	this->stiffness = stiffness;
}

void CompressibleSimulation::computePressures(const std::vector<std::vector<unsigned>>& neighborVector, float timeDifference)
{
#pragma loop(hint_parallel(0))
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].boundary)
		{
			continue;
		}
		particles[i].pressure = stiffness * (particles[i].density / fluidDensity - 1);
		if (particles[i].pressure < 0)
		{
			particles[i].pressure = 0;
		}
	}
}

float CompressibleSimulation::getStiffness() const
{
	return stiffness;
}