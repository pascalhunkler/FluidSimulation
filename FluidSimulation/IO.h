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
	IO();
	void decide_parameters(SimulationScenario& scenario, PressureComputationMethod& method, float& particleSize, float& viscosity, float& gravity, float& stiffness, float& timeStep);
	void save_picture(char* picture_data, int width, int height);
	void print_average_density(const std::vector<Particle>& particles) const;
};
