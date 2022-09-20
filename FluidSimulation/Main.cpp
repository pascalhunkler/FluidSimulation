#include "GUI.h"
#include "Simulation.h"
#include "IncompressibleSimulation.h"
#include "CompressibleSimulation.h"
#include <glm/glm.hpp>
#include <vector>
#include <queue>
#include <random>
#include <ctime>
#include <iostream>
#include "IO.h"



void createSimulationScenario(Simulation& simulation, const SimulationScenario environment, const int fluid_depth)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0.0f, 1.0f);
	
	const float particle_size = simulation.getParticleSize();
	const int width = simulation.getWidth();
	const int height = simulation.getHeight();
	
	// Add boundary particles
	for (int x = 0; x < 3 * int(particle_size); x += int(particle_size))
	{
		for (int y = 0; y <= height; y += int(particle_size))
		{
			simulation.addParticle(glm::vec2(x, y), glm::vec3(0.5f, 0.5f, 0.5f), true);
			simulation.addParticle(glm::vec2(width - x, y), glm::vec3(0.5f, 0.5f, 0.5f), true);
		}
	}

	for (int x = 3 * int(particle_size); x <= width - 3 * int(particle_size); x += int(particle_size))
	{
		for (int y = 0; y < 3 * int(particle_size); y += int(particle_size))
		{
			simulation.addParticle(glm::vec2(x, y), glm::vec3(0.5f, 0.5f, 0.5f), true);
		}
	}
	
	switch(environment)
	{
	case SimulationScenario::leakyDam:
		for (int x = width / 2 + (width / 2) % int(particle_size); x < width / 2 + (width / 2) % int(particle_size) + 6 * particle_size; x += int(particle_size))
		{
			for (int y = 3 * int(particle_size); y < height / 6; y += int(particle_size))
			{
				simulation.addParticle(glm::vec2(x, y), glm::vec3(0.5f, 0.5f, 0.5f), true);
			}

			for (int y = height / 6 + 5 * int(particle_size); y <= height; y += int(particle_size))
			{
				simulation.addParticle(glm::vec2(x, y), glm::vec3(0.5f, 0.5f, 0.5f), true);
			}
		}
	case SimulationScenario::breakingDam:
		for (int x = 3 * int(particle_size); x < width / 2; x += int(particle_size))
		{
			for (int y = 3 * int(particle_size); y < (3 + fluid_depth) * int(particle_size); y += int(particle_size))
			{
				simulation.addParticle(glm::vec2(x, y), glm::vec3(dist(mt), dist(mt), dist(mt)), false);
			}
		}
		break;

	case SimulationScenario::droppingFluid:
		for (int x = width / 3; x <= 2 * width / 3; x += int(particle_size))
		{
			for (int y = 3 * int(particle_size); y <= (height / 3) + fluid_depth * int(particle_size); y += int(particle_size))
			{
				if (y <= height / 3)
				{
					simulation.addParticle(glm::vec2(x, y), glm::vec3(0.5f, 0.5f, 0.5f), true);
				}
				else
				{
					simulation.addParticle(glm::vec2(x, y), glm::vec3(dist(mt), dist(mt), dist(mt)), false);
				}
			}
		}
		break;

	case SimulationScenario::flowingFluid:
		for (int x = 3 * int(particle_size); x <= width / 2; x += int(particle_size))
		{
			for (int i = 0; i < fluid_depth + 3; ++i)
			{
				bool boundary = i < 3 ? true : false;
				simulation.addParticle(glm::vec2(x, int(3 * particle_size + width / 4 - x / 2 + i * particle_size)), 
								       glm::vec3(0.5f, 0.5f, 0.5f), boundary);
			}
		}
		break;

	case SimulationScenario::restingFluid:
		for (int x = 3 * int(particle_size); x <= width - 3 * int(particle_size); x += int(particle_size))
		{
			for (int y = 3 * int(particle_size); y < (3 + fluid_depth) * int(particle_size); y += int(particle_size))
			{
				simulation.addParticle(glm::vec2(x, y), glm::vec3(dist(mt), dist(mt), dist(mt)), false);
			}
		}
		break;
		
	default:
		break;
	}
}

int main(int argc, char* argv[])
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0.0f, 1.0f);

	SimulationScenario scenario;
	int fluid_depth;
	PressureComputationMethod method;
	float particle_size, viscosity, gravity, stiffness, timeStep;
	IO* io = new IO();
	io->decide_parameters(scenario, fluid_depth, method, particle_size, viscosity, gravity, stiffness, timeStep);

	// Create GUI and simulation
	const int width = 200;
	const int height = 900;
	GUI gui(width, height, particle_size);
	Simulation* simulation;
	switch (method)
	{
	case PressureComputationMethod::compressible:
		simulation = new CompressibleSimulation(width, height, particle_size, 1, viscosity, gravity, io, stiffness);
		break;
	case PressureComputationMethod::incompressible:
	default:
		simulation = new IncompressibleSimulation(width, height, particle_size, 1, viscosity, gravity, io);
		break;
	}

	createSimulationScenario(*simulation, scenario, fluid_depth);

	while(gui.update())
	{
		// Repeat this as long as the window isn't closed

		// only one simulation step per draw
		simulation->performSimulationStep(timeStep);
		
		// Get the particle positions in the simulation and draw them
		gui.draw(simulation->getParticles());

		char* picture_data = gui.get_picture_data();
		io->save_picture(picture_data, width, height);
		delete[] picture_data;

		io->print_cfl_condition(simulation->getParticles(), timeStep, particle_size);
	}
	
	delete simulation;
	delete io;
	return 0;
}