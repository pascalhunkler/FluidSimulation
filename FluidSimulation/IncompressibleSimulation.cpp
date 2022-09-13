#include "IncompressibleSimulation.h"

IncompressibleSimulation::IncompressibleSimulation(int width, int height, float particleSize, float kernelSupport, float fluidDensity, float viscosity, float gravity)
	: Simulation(width, height, particleSize, kernelSupport, fluidDensity, viscosity, gravity)
{
	
}


std::vector<float> IncompressibleSimulation::computePressures(const std::vector<std::vector<unsigned>>& neighborVector, float timeDifference) const
{
	std::vector<float> pressure;
	pressure.resize(particles.size());
	std::vector<float> diagonal;
	diagonal.resize(particles.size());
	std::vector<float> source;
	source.resize(particles.size());

	#pragma loop(hint_parallel(0))
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].boundary)
		{
			continue;
		}
		// compute diagonal a_ii and source s_i and initialize pressure to 0
		float d = 0;
		float s = 0;
		for (auto& j : neighborVector[i])
		{
			glm::vec2 nabla_w_ij = kernelGradient(particles[i].position, particles[j].position);
			d += glm::dot(nabla_w_ij, nabla_w_ij);
			s += glm::dot(particles[i].velocity - particles[j].velocity, nabla_w_ij);
		}
		d *= -timeDifference * timeDifference * particleMass * particleMass / (particles[i].density * particles[i].density);
		diagonal[i] = d;
		s *= -particleMass * timeDifference;
		s += fluidDensity - particles[i].density;
		source[i] = s;
		pressure[i] = 0;
	}

	float error;
	int amountParticles = 0;
	std::vector<glm::vec2> acc;
	do
	{
		error = 0;
		acc = computePressureAccelerations(neighborVector, pressure);

		#pragma loop(hint_parallel(0))
		for (unsigned int i = 0; i < particles.size(); ++i)
		{
			if (particles[i].boundary)
			{
				continue;
			}
			float laplacian = 0;
			for (auto& j : neighborVector[i])
			{
				glm::vec2 nabla_w_ij = kernelGradient(particles[i].position, particles[j].position);
				laplacian += glm::dot(acc[i] - acc[j], nabla_w_ij);
			}
			laplacian *= particleMass * timeDifference * timeDifference;

			pressure[i] += (0.5f / diagonal[i]) * (source[i] - laplacian);
			if (pressure[i] < 0)
			{
				pressure[i] = 0;
			}
			error += glm::abs((laplacian - source[i]) / fluidDensity);
			amountParticles++;
		}
		error /= float(amountParticles);
	} while (error >= 0.001);

	return pressure;
}