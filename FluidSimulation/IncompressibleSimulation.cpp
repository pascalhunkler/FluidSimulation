#include "IncompressibleSimulation.h"

IncompressibleSimulation::IncompressibleSimulation(int width, int height, float particleSize, float kernelSupport, float fluidDensity, float viscosity, float gravity)
	: Simulation(width, height, particleSize, kernelSupport, fluidDensity, viscosity, gravity)
{
	
}


void IncompressibleSimulation::computePressures(const std::vector<std::vector<unsigned>>& neighborVector, float timeDifference)
{
	std::vector<glm::vec2> d_diagonal;
	d_diagonal.resize(particles.size());
	std::vector<float> a_diagonal;
	a_diagonal.resize(particles.size());
	std::vector<float> density_advected;
	density_advected.resize(particles.size());

	#pragma loop(hint_parallel(0))
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		d_diagonal[i] = glm::vec2(0, 0);
		if (particles[i].boundary)
		{
			continue;
		}
		for (auto& j : neighborVector[i])
		{
			glm::vec2 nabla_w_ij = kernelGradient(particles[i].position, particles[j].position);
			d_diagonal[i] += nabla_w_ij;
		}
		d_diagonal[i] *= -timeDifference * timeDifference * particleMass / (particles[i].density * particles[i].density);
	}

	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		density_advected[i] = 0;
		a_diagonal[i] = 0;
		if (particles[i].boundary)
		{
			continue;
		}
		for (auto& j : neighborVector[i])
		{
			glm::vec2 nabla_w_ij = kernelGradient(particles[i].position, particles[j].position);
			glm::vec2 nabla_w_ji = kernelGradient(particles[j].position, particles[i].position);
			density_advected[i] += glm::dot(particles[i].velocity - particles[j].velocity, nabla_w_ij);
			glm::vec2 d_ji = nabla_w_ji;
			d_ji /= particles[i].density;
			d_ji *= -timeDifference * timeDifference * particleMass;
			a_diagonal[i] += glm::dot((d_diagonal[i] - d_ji), nabla_w_ij);
		}
		a_diagonal[i] *= particleMass;

		density_advected[i] *= timeDifference * particleMass;
		density_advected[i] += particles[i].density;

		particles[i].pressure /= 2;
	}

	float error;
	int amountParticles;
	do
	{
		error = 0.f;
		amountParticles = 0;
		std::vector<glm::vec2> sum_d_ij_p_j;
		sum_d_ij_p_j.resize(particles.size());
#pragma loop(hint_parallel(0))
		for (unsigned int i = 0; i < particles.size(); ++i)
		{
			sum_d_ij_p_j[0] = glm::vec2(0, 0);
			if (particles[i].boundary)
			{
				continue;
			}
			for (auto& j : neighborVector[i])
			{
				glm::vec2 nabla_w_ij = kernelGradient(particles[i].position, particles[j].position);
				if (particles[j].boundary)
				{
					sum_d_ij_p_j[i] += -(particles[i].pressure / (fluidDensity * fluidDensity)) * nabla_w_ij;
				}
				else
				{
					sum_d_ij_p_j[i] += -(particles[j].pressure / (particles[j].density * particles[j].density)) * nabla_w_ij;
				}
				sum_d_ij_p_j[i] *= timeDifference * timeDifference * particleMass;
			}
		}

		for (unsigned int i = 0; i < particles.size(); ++i)
		{
			float value = 0;
			if (particles[i].boundary)
			{
				continue;
			}
			for (auto& j : neighborVector[i])
			{
				glm::vec2 nabla_w_ij = kernelGradient(particles[i].position, particles[j].position);
				glm::vec2 d_ji_p_i = particleMass / (particles[i].density * particles[i].density) * nabla_w_ij;
				value = glm::dot(sum_d_ij_p_j[i] - d_diagonal[j] - (sum_d_ij_p_j[j] - d_ji_p_i), nabla_w_ij);
			}
			value *= particleMass;
			float old_pressure = particles[i].pressure;
			particles[i].pressure = particles[i].pressure / 2 + (0.5f / a_diagonal[i]) * (fluidDensity - density_advected[i] - value);
			if (particles[i].pressure < 0)
			{
				particles[i].pressure = 0;
			}
			error += glm::abs((particles[i].pressure - 0.5f * old_pressure) * 2 * a_diagonal[i]);
			++amountParticles;
		}
		error /= float(amountParticles);
	} while (error >= 0.001f);
	
	/*
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
		particles[i].pressure /= 2;
	}

	float error;
	int amountParticles;
	std::vector<glm::vec2> acc;
	do
	{
		error = 0;
		amountParticles = 0;
		acc = computePressureAccelerations(neighborVector);

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

			particles[i].pressure += (0.5f / diagonal[i]) * (source[i] - laplacian);
			if (particles[i].pressure < 0)
			{
				particles[i].pressure = 0;
			}
			error += glm::abs((laplacian - source[i]) / fluidDensity);
			amountParticles++;
		}
		error /= float(amountParticles);
	} while (error >= 0.001);
	*/
}