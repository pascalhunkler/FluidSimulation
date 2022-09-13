#pragma once
#include "Simulation.h"
class CompressibleSimulation :
    public Simulation
{
public:
    CompressibleSimulation(int width, int height, float particleSize, float kernelSupport, float fluidDensity, float viscosity, float gravity, float stiffness);
    float getStiffness() const;
private:
    // compute pressures with a state equation
    void computePressures(const std::vector<std::vector<unsigned>>& neighborVector, float timeDifference) override;

    // stiffness for the pressure acceleration computation
    float stiffness;
};

