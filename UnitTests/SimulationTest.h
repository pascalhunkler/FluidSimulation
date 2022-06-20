#pragma once
#include "../FluidSimulation/Simulation.h"
#include <vector>

class SimulationTest : public ::testing::Test
{
protected:
	Simulation simulation = Simulation(1000, 1000, 10);
	std::vector<Particle> particles;
	unsigned int testingParticle = 50 * 100 + 50;
	ParticleUniformGrid grid = ParticleUniformGrid(simulation.getKernelSupport(), 1000, 1000);
	void SetUp() override;
};

