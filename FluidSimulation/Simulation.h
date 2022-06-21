#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Particle.h"
#include "ParticleUniformGrid.h"

class Simulation
{
public:
	/**
	 *	Create new simulation
	 *	@param particleSize the size of a particle
	 *	@param kernelSupport kernel support size, default value: 2 * particleSize
	 *	@param fluidDensity fluid density, default value: 1
	 *	@param viscosity viscosity of the fluid, default value: 1e-6
	 *	@param stiffness stiffness of the fluid, default value: 2000
	 *	@param gravity gravitational constant
	 */
	Simulation(int width, int height, float particleSize, float kernelSupport = -1, float fluidDensity = 1, float viscosity = 1e-6, float gravity = 9.81);

	virtual ~Simulation();

	/**
		Add a new particle to the simulation
		@param position position of the new particle
		@param color color of the new particle
		@param boundary true if particle belongs to the boundary
	*/
	void addParticle(glm::vec2 position, glm::vec3 color, bool boundary);

	/**
	 *	Add a new particle to the simulation
	 *	@param particle the particle which is added to the simulation
	 */
	void addParticle(const Particle particle);

	
	/**
	 *	Get the positions of all particles in the simulation
	 *	@return a vector containing the positions of the particles
	 */
	std::vector<glm::vec2>* getParticlePositions() const;

	/**
	 *	Get all particles in the simulation
	 *	@return a vector containing all particles in the simulation
	 */
	const std::vector<Particle>& getParticles() const;
	
	/**
	 *	Do a simulation step, compute accelerations and change velocity and position of the particles
	 *	@param timeDifference the time which has passed since the last simulation step
	 */
	void performSimulationStep(float timeDifference);

	/**
	 *	kernel function used by the simulation
	 *	@param xi the position of the first particle
	 *	@param xj the position of the second particle, typically the position of a neighboring particle of the first particle
	 *	@return value of the kernel function taking the distance between the two particles divided by the particle size as input
	 */
	float kernelFunction(glm::vec2 xi, glm::vec2 xj) const;

	/**
	 *	kernel function used by the simulation
	 *	@param q the distance between two particles, divided by the particle size
	 *	@return value of the kernel function
	 */
	float kernelFunction(float q) const;

	/**
	 *	kernel gradient used by the simulation
	 *	@param xi the position of the first particle
	 *	@param xj the position of the second particle, typically the position of a neighboring particle of the first particle
	 *	@return gradient of the kernel function taking the distance between the two particles divided by the particle size as input
	 */
	glm::vec2 kernelGradient(glm::vec2 xi, glm::vec2 xj) const;

	/**
	 *	Get all neighbor particles of a certain particle in the simulation
	 *	@param particleIndex index of the particle whose neighbors we are looking for
	 *	@param grid uniform grid where the particles are stored
	 *	@return a vector containing all indices of neighboring particles of the given particle in the simulation
	 */
	std::vector<unsigned int> getNeighbors(unsigned int particleIndex, const ParticleUniformGrid& grid) const;
	
	float getParticleSize() const;

	float getKernelSupport() const;

	float getFluidDensity() const;

	float getParticleMass() const;

	float getViscosity() const;

	float getGravity() const;

	int getWidth() const;

	int getHeight() const;

protected:
	// all particles in the simulation
	std::vector<Particle> particles;

	// the particle size
	float particleSize;

	// the kernel support
	float kernelSupport;

	// the density of the fluid
	float fluidDensity;

	// mass of a particle
	float particleMass;

	// viscosity of the fluid
	float viscosity;

	// gravitational constant
	float gravity;

	// width of the simulation
	int width;

	// height of the simulation
	int height;

	const float  PI_F = 3.14159265358979f;

	/**
	 *	Compute and return a vector that contains vectors of each neighbor of each particle
	 */
	std::vector<std::vector<unsigned int>> createNeighborVector() const;

	/**
	 *	Compute density of each particle
	 */
	void computeDensitiesExplicit(const std::vector<std::vector<unsigned int>>& neighborVector);

	/**
	 *	Compute density of each particle using differential
	 */
	void computeDensitiesDifferential(const std::vector<std::vector<unsigned int>>& neighborVector, float timeDifference);

	/**
	 *	Compute and return pressure of each particle
	 */
	virtual std::vector<float> computePressures(const std::vector<std::vector<unsigned>>& neighborVector, float timeDifference) const;

	/**
	 *	Compute and return non-pressure accelerations
	 */
	std::vector<glm::vec2> computeNonPressureAccelerations(const std::vector<std::vector<unsigned int>>& neighborVector) const;


	/**
	 *	Compute and return pressure acceleration
	 */
	std::vector<glm::vec2> computePressureAccelerations(const std::vector<std::vector<unsigned int>>& neighborVector,
														const std::vector<float>& pressures) const;

	
	/**
	 *	Update velocity
	 */
	void updateVelocity(std::vector<glm::vec2>& acc, float timeDifference);

	/**
	 *	Update position
	 */
	void updatePosition(float timeDifference);

	/**
	 *	Update colors of each particle
	 */
	void updateColor();
};

