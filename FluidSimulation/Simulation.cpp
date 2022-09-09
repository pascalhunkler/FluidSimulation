#include "Simulation.h"
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>
#include <array>

Simulation::Simulation(int width, int height, float particleSize, float kernelSupport, float fluidDensity, float viscosity, float gravity)
{
	this->width = width;
	this->height = height;
	particles = std::vector<Particle>();
	this->particleSize = particleSize;
	if (kernelSupport == -1)
	{
		kernelSupport = 2 * particleSize;
	}
	this->kernelSupport = kernelSupport;
	this->fluidDensity = fluidDensity;
	this->particleMass = fluidDensity * particleSize * particleSize;
	this->viscosity = viscosity;
	this->gravity = gravity;
}

Simulation::~Simulation() = default;


void Simulation::addParticle(glm::vec2 position, glm::vec3 color, bool boundary)
{
	particles.push_back({position, color, boundary});
}

void Simulation::addParticle(const Particle particle)
{
	particles.push_back(particle);
}


std::vector<glm::vec2>* Simulation::getParticlePositions() const
{
	auto* particlePositions = new std::vector<glm::vec2>();
	for (auto& particle : particles)
	{
		// Iterate through each particle and add its position to the position vector
		particlePositions->push_back(particle.position);
	}
	return particlePositions;
}

const std::vector<Particle>& Simulation::getParticles() const
{
	return particles;
}

void Simulation::performSimulationStep(float timeDifference)
{
	// Do neighbor search
	std::vector<std::vector<unsigned int>> neighbors = createNeighborVector();

	// compute density of each particle
	computeDensitiesExplicit(neighbors);

	// compute non pressure accelerations
	std::vector<glm::vec2> accNonP = computeNonPressureAccelerations(neighbors);

	// update position and velocity of each particle
	updateVelocity(accNonP, timeDifference);

	// compute density of each particle
	//computeDensitiesDifferential(neighbors, timeDifference);

	// compute pressure of each particle
	std::vector<float> pressure = computePressures(neighbors, timeDifference);
	
	// compute pressure accelerations
	std::vector<glm::vec2> accP = computePressureAccelerations(neighbors, pressure);

	// update position and velocity of each particle
	updateVelocity(accP, timeDifference);
	updatePosition(timeDifference);

	// update color of each non boundary particle
	updateColor(timeDifference);
}

std::vector<float> Simulation::computePressures(const std::vector<std::vector<unsigned>>& neighborVector, float timeDifference) const
{
	// This function is virtual and thus will be overridden, so just return a 0 vector.
	std::vector<float> pressure;
	pressure.resize(particles.size());
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		pressure[i] = 0;
	}
	return pressure;
}


std::vector<unsigned int> Simulation::getNeighbors(unsigned int particleIndex, const ParticleUniformGrid& grid) const
{
	std::vector<unsigned int> neighbors;
	neighbors.reserve(20);
	std::vector<unsigned int> possibleNeighbors;
	possibleNeighbors.reserve(20);

	std::vector<unsigned int> counter = grid.getCounter();
	std::vector<unsigned int> sortedList = grid.getSortedList();
	std::array<glm::vec2, 9> directions = {
		glm::vec2(0.f, 0.f),
		glm::vec2(kernelSupport, 0.f),
		glm::vec2(0.f, kernelSupport),
		glm::vec2(-kernelSupport, 0.f),
		glm::vec2(0.f, -kernelSupport),
		glm::vec2(kernelSupport, kernelSupport),
		glm::vec2(-kernelSupport, kernelSupport),
		glm::vec2(-kernelSupport, -kernelSupport),
		glm::vec2(kernelSupport, -kernelSupport),
	};

	// find all particles which could possibly be a neighbor
	for (auto& direction : directions)
	{
		glm::vec2 pos = particles[particleIndex].position + direction;
		if (!(0.f <= pos.x && pos.x < float(width) + kernelSupport - fmod(width, kernelSupport) && 
			  0.f <= pos.y && pos.y < float(height) + kernelSupport - fmod(height, kernelSupport)))
		{
			continue;
		}
		const unsigned int cellIndex = grid.getCellIndex(pos);
		for (unsigned int i = counter[cellIndex]; i < counter[cellIndex + 1]; ++i)
		{
			possibleNeighbors.push_back(sortedList[i]);
		}
	}

	// check if possible neighboring particles are indeed neighbors
	for (auto& possibleNeighbor : possibleNeighbors)
	{
		if (glm::distance(particles[particleIndex].position, particles[possibleNeighbor].position) < kernelSupport)
		{
			neighbors.push_back(possibleNeighbor);
		}
	}
	return neighbors;
}

std::vector<std::vector<unsigned int>> Simulation::createNeighborVector() const
{
	ParticleUniformGrid grid(kernelSupport, width, height);
	grid.initializeGrid(particles);
	std::vector<std::vector<unsigned int>> neighbors;
	
	#pragma loop(hint_parallel(0))
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].boundary)
		{
			neighbors.push_back(std::vector<unsigned int>{i});
		}
		else
		{
			neighbors.push_back(getNeighbors(i, grid));
		}
	}
	return neighbors;
}

void Simulation::computeDensitiesExplicit(const std::vector<std::vector<unsigned int>>& neighborVector)
{
	float averageDensity = 0;
	int amountFluidParticles = 0;
	
	#pragma loop(hint_parallel(0))
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].boundary)
		{
			continue;
		}
		float d = 0;
		for (auto& j : neighborVector[i])
		{
			d += kernelFunction(particles[i].position, particles[j].position);
		}
		d *= particleMass;
		particles[i].density = d;

		amountFluidParticles++;
		averageDensity += d;
	}
	averageDensity /= float(amountFluidParticles);
}

void Simulation::computeDensitiesDifferential(const std::vector<std::vector<unsigned>>& neighborVector, float timeDifference)
{
	float averageDensity = 0;
	int amountFluidParticles = 0;
	
	#pragma loop(hint_parallel(0))
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].boundary)
		{
			continue;
		}
		float d = 0;
		for (auto& j : neighborVector[i])
		{
			d += glm::dot(particles[i].velocity - particles[j].velocity, kernelGradient(particles[i].position, particles[j].position));
		}
		d *= particleMass * timeDifference;
		particles[i].density += d;

		amountFluidParticles++;
		averageDensity += particles[i].density;
	}
	averageDensity /= float(amountFluidParticles);
	//std::cout << averageDensity << std::endl;
}

std::vector<glm::vec2> Simulation::computeNonPressureAccelerations(const std::vector<std::vector<unsigned>>& neighborVector) const
{
	// compute accelerations
	std::vector<glm::vec2> acc;
	acc.reserve(particles.size());

	#pragma loop(hint_parallel(0))
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].boundary)
		{
			acc.push_back(glm::vec2(0.f, 0.f));
			continue;
		}
		glm::vec2 acc_g = glm::vec2(0.f, -gravity);
		glm::vec2 acc_v = glm::vec2(0.f, 0.f);

		// compute viscosity acceleration
		for (auto& j : neighborVector[i])
		{
			float factor = glm::dot(particles[i].velocity - particles[j].velocity, particles[i].position - particles[j].position);
			factor /= glm::dot(particles[i].position - particles[j].position, particles[i].position - particles[j].position) + 0.01f * particleSize * particleSize;
			if (particles[j].boundary)
			{
				factor *= 1 / particles[i].density;
			}
			else
			{
				factor *= 1 / particles[j].density;
			}

			acc_v += factor * kernelGradient(particles[i].position, particles[j].position);
		}
		acc_v *= 2 * viscosity * particleMass;
		acc.push_back(acc_g + acc_v);
	}
	return acc;
}

std::vector<glm::vec2> Simulation::computePressureAccelerations(const std::vector<std::vector<unsigned>>& neighborVector, const std::vector<float>& pressures) const
{
	std::vector<glm::vec2> acc;
	acc.reserve(particles.size());

	#pragma loop(hint_parallel(0))
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].boundary)
		{
			acc.push_back(glm::vec2(0.f, 0.f));
			continue;
		}
		glm::vec2 acc_p = glm::vec2(0.f, 0.f);

		// compute pressure acceleration
		for (auto& j : neighborVector[i])
		{
			float factor;
			if (particles[j].boundary)
			{
				factor = pressures[i] / (particles[i].density * particles[i].density);
				factor += pressures[i] / (fluidDensity * fluidDensity);
			}
			else
			{
				factor = pressures[i] / (particles[i].density * particles[i].density);
				factor += pressures[j] / (particles[j].density * particles[j].density);
			}
			acc_p -= factor * kernelGradient(particles[i].position, particles[j].position);
		}
		acc_p *= particleMass;
		acc.push_back(acc_p);
	}
	return acc;
}



float Simulation::kernelFunction(glm::vec2 xi, glm::vec2 xj) const
{
	return kernelFunction(glm::distance(xi, xj) / particleSize);
}


float Simulation::kernelFunction(float q) const
{
	float t1 = glm::max(1 - q, 0.f);
	float t2 = glm::max(2 - q, 0.f);
	float sigma = 5.f / (14.f * PI_F * particleSize * particleSize);
	return sigma * (t2 * t2 * t2 - 4 * t1 * t1 * t1);
}

glm::vec2 Simulation::kernelGradient(glm::vec2 xi, glm::vec2 xj) const
{
	float q = glm::distance(xi, xj) / particleSize;
	if (q == 0)
	{
		return glm::vec2(0.f, 0.f);
	}
	
	float t1 = glm::max(1 - q, 0.f);
	float t2 = glm::max(2 - q, 0.f);
	float sigma = 5.f / (14.f * PI_F * particleSize * particleSize);
	return sigma * (xi - xj) / (q * particleSize * particleSize) * (-3 * t2 * t2 + 12 * t1 * t1);
}

void Simulation::updateVelocity(std::vector<glm::vec2>& acc, float timeDifference)
{
	// update the velocity of all particles not belonging to the boundary
	#pragma loop(hint_parallel(0))
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].boundary)
		{
			continue;
		}

		particles[i].velocity += timeDifference * acc[i];
	}
}

void Simulation::updatePosition(float timeDifference)
{
	// update the position of all particles not belonging to the boundary
	#pragma loop(hint_parallel(0))
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].boundary)
		{
			continue;
		}

		particles[i].position += timeDifference * particles[i].velocity;
	}
}


void Simulation::updateColor(float timeDifference)
{
	#pragma loop(hint_parallel(0))
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].boundary)
		{
			continue;
		}

		// change color according to the speed of the particle
		float speed = glm::length(particles[i].velocity);
		const float maxSpeed = this->particleSize / timeDifference;
		float red, green, blue;
		if (speed < maxSpeed / 2)
		{
			red = 0.f;
			green = glm::sin(PI_F / maxSpeed * speed) * glm::sin(PI_F / maxSpeed * speed);
			blue = glm::cos(PI_F / maxSpeed * speed) * glm::cos(PI_F / maxSpeed * speed);
			particles[i].color = glm::vec3(red, green, blue);
		}
		else if (speed < maxSpeed)
		{
			red = glm::cos(PI_F / maxSpeed * speed) * glm::cos(PI_F / maxSpeed * speed);
			green = glm::sin(PI_F / maxSpeed * speed) * glm::sin(PI_F / maxSpeed * speed);
			blue = 0.f;
			particles[i].color = glm::vec3(red, green, blue);
		}
		else
		{
			particles[i].color = glm::vec3(1.f, 0.f, 0.f);
		}
	}
}



float Simulation::getFluidDensity() const
{
	return fluidDensity;
}

float Simulation::getKernelSupport() const
{
	return kernelSupport;
}

float Simulation::getParticleMass() const
{
	return particleMass;
}

float Simulation::getParticleSize() const
{
	return particleSize;
}

float Simulation::getViscosity() const
{
	return viscosity;
}

float Simulation::getGravity() const
{
	return gravity;
}

int Simulation::getWidth() const
{
	return width;
}

int Simulation::getHeight() const
{
	return height;
}
