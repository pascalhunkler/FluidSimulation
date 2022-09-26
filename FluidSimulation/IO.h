#pragma once
#include <string>
#include <vector>
#include "Particle.h"

enum class SimulationScenario { breakingDam, leakyDam, droppingFluid, flowingFluid, restingFluid, last };
enum class PressureComputationMethod { incompressible, compressible };

class IO
{
private:
	std::string folder_name;
	int pictures;
public:
	IO(const IO& io);
	IO();
	void decide_parameters(SimulationScenario& scenario, int& width, int& height, int& fluid_depth, float& particle_size,
						   PressureComputationMethod& method, float& max_error, float& stiffness, float& viscosity, float& gravity, float& timeStep);
	void save_picture(char* picture_data, int width, int height);
	void print_average_density(float average_density) const;
	void print_cfl_condition(const std::vector<Particle>& particles, float timeStep, float particleSize) const;
	void print_iterations(int iterations) const;
};
