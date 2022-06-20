#pragma once
#include <vector>
#include "Particle.h"

class ParticleUniformGrid
{
public:
	/**
	 *	initialize the member variables
	 *	@param kernelSupport the kernel support of the simulation, determines the size of a grid cell
	 *	@param width the width of the simulation space
	 *	@param height the height of the simulation space
	 */
	ParticleUniformGrid(float kernelSupport, int width, int height);

	/**
	 *	set the counter and sortedList so it can work properly for the given particles
	 *	@param particles the particles whose indices are saved in the sorted list
	 */
	void initializeGrid(const std::vector<Particle>& particles);
	
	/**
	 *	@return counter member variable which contains indexes for the sorted list
	 */
	const std::vector<unsigned int>& getCounter() const;
	
	/**
	 *	@return sortedList member variable which contains indexes for the particles
	 */
	const std::vector<unsigned int>& getSortedList() const;

	/**
	 *	Get the index of the cell where the particle is located
	 *	@param particle the particle for which we want to get the index of the cell where it is located
	 *	@return the index of the cell where the particle is located
	 */
	unsigned int getCellIndex(const Particle& particle) const;

	/**
	 *	Get the index of the cell where the particle is located
	 *	@param pos the position of the particle for which we want to get the index of the cell where it is located
	 *	@return the index of the cell where the particle is located
	 */
	unsigned int getCellIndex(const glm::vec2& pos) const;

private:
	std::vector<unsigned int> counter;
	std::vector<unsigned int> sortedList;
	float kernelSupport;
	int rows;
	int cols;
	int width;
	int height;
	unsigned int cellSize;
};
