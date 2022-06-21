#pragma once
#include <string>

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
};
