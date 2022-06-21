#include "CompressibleSimulation.h"

CompressibleSimulation::CompressibleSimulation(int width, int height, float particleSize, float kernelSupport, float fluidDensity, float viscosity, float gravity, float stiffness)
	: Simulation(width, height, particleSize, kernelSupport, fluidDensity, viscosity, gravity)
{
	this->stiffness = stiffness;
}

std::vector<float> CompressibleSimulation::computePressures(const std::vector<std::vector<unsigned>>& neighborVector, float timeDifference) const
{
	std::vector<float> pressure;
	pressure.resize(particles.size());

#pragma loop(hint_parallel(0))
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].boundary)
		{
			continue;
		}
		pressure[i] = stiffness * (particles[i].density / fluidDensity - 1);
		if (pressure[i] < 0)
		{
			pressure[i] = 0;
		}
	}
	return pressure;
}

float CompressibleSimulation::getStiffness() const
{
	return stiffness;
}