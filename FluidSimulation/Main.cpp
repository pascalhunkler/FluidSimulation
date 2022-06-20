#include "GUI.h"
#include "Simulation.h"
#include <glm/glm.hpp>
#include <vector>
#include <queue>
#include <random>
#include <ctime>
#include <iostream>
#include "IO.h"

enum class SimulationScenario { breakingDam, leakyDam, droppingFluid, flowingFluid, restingFluid, last };

void createSimulationScenario(Simulation& simulation, const SimulationScenario environment)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0.0f, 1.0f);
	
	const float particleSize = simulation.getParticleSize();
	const int width = simulation.getWidth();
	const int height = simulation.getHeight();
	
	// Add boundary particles
	for (int x = 0; x < 3 * int(particleSize); x += int(particleSize))
	{
		for (int y = 0; y <= height; y += int(particleSize))
		{
			simulation.addParticle(glm::vec2(x, y), glm::vec3(0.5f, 0.5f, 0.5f), true);
			simulation.addParticle(glm::vec2(width - x, y), glm::vec3(0.5f, 0.5f, 0.5f), true);
		}
	}

	for (int x = 3 * int(particleSize); x <= width - 3 * int(particleSize); x += int(particleSize))
	{
		for (int y = 0; y < 3 * int(particleSize); y += int(particleSize))
		{
			simulation.addParticle(glm::vec2(x, y), glm::vec3(0.5f, 0.5f, 0.5f), true);
		}
	}
	
	switch(environment)
	{
	case SimulationScenario::leakyDam:
		for (int x = width / 2 + (width / 2) % int(particleSize); x < width / 2 + (width / 2) % int(particleSize) + 6 * particleSize; x += int(particleSize))
		{
			for (int y = 3 * int(particleSize); y < height / 6 - 2 * int(particleSize); y += int(particleSize))
			{
				simulation.addParticle(glm::vec2(x, y), glm::vec3(0.5f, 0.5f, 0.5f), true);
			}

			for (int y = height / 6 + 3 * int(particleSize); y <= height; y += int(particleSize))
			{
				simulation.addParticle(glm::vec2(x, y), glm::vec3(0.5f, 0.5f, 0.5f), true);
			}
		}
	case SimulationScenario::breakingDam:
		for (int x = 3 * int(particleSize); x < width / 2; x += int(particleSize))
		{
			for (int y = 3 * int(particleSize); y < height / 2; y += int(particleSize))
			{
				simulation.addParticle(glm::vec2(x, y), glm::vec3(dist(mt), dist(mt), dist(mt)), false);
			}
		}
		break;

	case SimulationScenario::droppingFluid:
		for (int x = width / 3; x <= 2 * width / 3; x += int(particleSize))
		{
			for (int y = 3 * int(particleSize); y <= 2 * height / 3; y += int(particleSize))
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
		for (int x = 3 * int(particleSize); x <= width / 2; x += int(particleSize))
		{
			for (int i = 0; i < 25; ++i)
			{
				bool boundary = i < 3 ? true : false;
				simulation.addParticle(glm::vec2(x, int(3 * particleSize + width / 4 - x / 2 + i * particleSize)), 
								       glm::vec3(0.5f, 0.5f, 0.5f), boundary);
			}
		}
		break;

	case SimulationScenario::restingFluid:
		for (int x = 3 * int(particleSize); x <= width - 3 * int(particleSize); x += int(particleSize))
		{
			for (int y = 3 * int(particleSize); y < height / 4; y += int(particleSize))
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

	// Let the user decide what scenario to simulate
	for (int i = 0; i < static_cast<int>(SimulationScenario::last); ++i)
	{
		std::cout << i << '\t';
		switch (static_cast<SimulationScenario>(i))
		{
		
		case SimulationScenario::breakingDam:
			std::cout << "breaking dam" << std::endl;
			break;

		case SimulationScenario::leakyDam:
			std::cout << "leaky dam" << std::endl;
			break;
			
		case SimulationScenario::droppingFluid:
			std::cout << "dropping fluid" << std::endl;
			break;

		case SimulationScenario::flowingFluid:
			std::cout << "flowing fluid" << std::endl;
			break;

		case SimulationScenario::restingFluid:
			std::cout << "resting fluid" << std::endl;
			break;
			
		default:
			break;
		}
	}
	int scenario;
	std::cin >> scenario;

	IO io;

	// choose first scenario if user gives invalid input
	if (scenario < 0 || scenario >= static_cast<int>(SimulationScenario::last))
	{
		scenario = 0;
	}

	// Create GUI and simulation
	const int width = 1200;
	const int height = 750;
	const float particleSize = 8;
	GUI gui(width, height, particleSize);
	Simulation simulation(width, height, particleSize, 2*particleSize, 1, 200, 1E+6, 9.81);

	createSimulationScenario(simulation, static_cast<SimulationScenario>(scenario));

	const float timeDifference = 0.08;
	while(gui.update())
	{
		// Repeat this as long as the window isn't closed

		// only one simulation step per draw
		simulation.performSimulationStep(timeDifference);
		
		// Get the particle positions in the simulation and draw them
		gui.draw(simulation.getParticles());

		char* picture_data = gui.get_picture_data();
		io.save_picture(picture_data, width, height);
		delete[] picture_data;
	}

	return 0;
}