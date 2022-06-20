#include "pch.h"
#include "SimulationTest.h"
#include "../FluidSimulation/Simulation.h"
#include "../FluidSimulation/ParticleUniformGrid.h"
#include <glm/glm.hpp>

void SimulationTest::SetUp()
{
	for (int i = 0; i < 1000; i+=10)
	{
		for (int j = 0; j < 1000; j+=10)
		{
			simulation.addParticle(glm::vec2(i, j), glm::vec3(0.5f, 0.5f, 0.5f), false);
		}
	}
	particles = simulation.getParticles();
	grid = ParticleUniformGrid(20, 1000, 1000);
	grid.initializeGrid(particles);
}
