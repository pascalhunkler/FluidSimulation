#include "ParticleUniformGrid.h"
#include <vector>

ParticleUniformGrid::ParticleUniformGrid(float kernelSupport, int width, int height)
{
	this->kernelSupport = kernelSupport;
	this->width = width;
	this->height = height;
	cols = int(ceil(float(width) / kernelSupport)) + 1;
	rows = int(ceil(float(height) / kernelSupport)) + 1;
	this->cellSize = cols * rows + 1;
	counter.resize(cellSize);
}

void ParticleUniformGrid::initializeGrid(const std::vector<Particle>& particles)
{
	for (unsigned int i = 0; i < cellSize; ++i)
	{
		counter.at(i) = 0;
	}
	
	for (auto& particle : particles)
	{
		const unsigned int cellIndex = getCellIndex(particle);
		counter.at(cellIndex) += 1;
	}

	for (unsigned int i = 1; i < counter.size(); ++i)
	{
		counter.at(i) += counter.at(i - 1);
	}

	sortedList.resize(particles.size());
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		const unsigned int cellIndex = getCellIndex(particles[i]);
		sortedList.at(--counter.at(cellIndex)) = i;
	}
}

unsigned int ParticleUniformGrid::getCellIndex(const Particle& particle) const
{
	return getCellIndex(particle.position);
}

unsigned ParticleUniformGrid::getCellIndex(const glm::vec2& pos) const
{
	const unsigned int x = int(pos.x) >= width ? int(static_cast<float>(width) / kernelSupport) : int(pos.x) < 0 ? 0 : int(pos.x / kernelSupport);
	const unsigned int y = int(pos.y) >= height ? int(static_cast<float>(height) / kernelSupport) : int(pos.y) < 0 ? 0 : int(pos.y / kernelSupport);
	return x + cols * y;
}


const std::vector<unsigned int>& ParticleUniformGrid::getCounter() const
{
	return counter;
}

const std::vector<unsigned int>& ParticleUniformGrid::getSortedList() const
{
	return sortedList;
}
