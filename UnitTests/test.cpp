#include "pch.h"
#include "../FluidSimulation/Simulation.h"
#include "SimulationTest.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

glm::vec2 roundVector(const glm::vec2& vector, float factor = 10000000000.f)
{
	glm::vec2 rounded = glm::vec2(vector);
	rounded *= factor;
	rounded = glm::round(rounded);
	rounded /= factor;
	return rounded;
}

float roundFloat(float number, float factor = 10000.f)
{
	float rounded = number * factor;
	rounded = glm::round(rounded);
	rounded /= factor;
	return rounded;
}

glm::mat2 roundMatrix(const glm::mat2& matrix, float factor = 1000.f)
{
	glm::mat2 rounded = glm::mat2(matrix);
	rounded *= factor;
	glm::column(rounded, 0, roundVector(glm::column(rounded, 0)));
	glm::column(rounded, 1, roundVector(glm::column(rounded, 1)));
	rounded /= factor;
	return rounded;
}

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST_F(SimulationTest, NeighborTest)
{
	// Test if neighbors are computed correctly
	for (int i = 0; i < 10000; ++i)
	{
		std::vector<unsigned int> neighbors = simulation.getNeighbors(i, grid);
		for (int j = 0; j < 10000; ++j)
		{
			if (std::find(neighbors.begin(), neighbors.end(), j) == neighbors.end())
			{
				EXPECT_GE(glm::distance(particles[i].position, particles[j].position), simulation.getKernelSupport());
			}
			else
			{
				EXPECT_LT(glm::distance(particles[i].position, particles[j].position), simulation.getKernelSupport());
			}
		}
	}
}

TEST_F(SimulationTest, KernelFunctionTest)
{
	std::vector<unsigned int> neighbors = simulation.getNeighbors(testingParticle, grid);
	float sum = 0;
	for (auto& neighbor : neighbors)
	{
		glm::vec2 xi = glm::vec2(particles[testingParticle].position.x, particles[testingParticle].position.y);
		glm::vec2 xj = glm::vec2(particles[neighbor].position.x, particles[neighbor].position.y);
		sum += simulation.kernelFunction(xi, xj);
	}
	sum = roundFloat(sum);
	EXPECT_EQ(sum, simulation.getFluidDensity() / simulation.getParticleMass());
}

TEST_F(SimulationTest, KernelGradientTest)
{
	glm::vec2 gradient1;
	glm::vec2 gradient2;
	const float pi = 3.14159265358979f;
	const float h = simulation.getParticleSize();
	float alpha = 5.f / (14.f * pi * h * h);
	float beta = -3 * (2 - sqrt(2.f)) * (2 - sqrt(2.f));
	
	gradient1 = roundVector(simulation.kernelGradient(glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f)));
	EXPECT_EQ(gradient1, roundVector(glm::vec2(0.f, 0.f)));

	gradient1 = roundVector(simulation.kernelGradient(glm::vec2(0.f, 0.f), glm::vec2(h, 0.f)));
	gradient2 = roundVector(simulation.kernelGradient(glm::vec2(0.f, 0.f), glm::vec2(-h, 0.f)));
	EXPECT_EQ(gradient1, -gradient2);
	EXPECT_EQ(gradient1, roundVector(glm::vec2(3 * alpha / h, 0.f)));

	gradient1 = roundVector(simulation.kernelGradient(glm::vec2(0.f, 0.f), glm::vec2(0.f, h)));
	gradient2 = roundVector(simulation.kernelGradient(glm::vec2(0.f, 0.f), glm::vec2(0.f, -h)));
	EXPECT_EQ(gradient1, -gradient2);
	EXPECT_EQ(gradient1, roundVector(glm::vec2(0.f, 3 * alpha / h)));

	gradient1 = roundVector(simulation.kernelGradient(glm::vec2(0.f, 0.f), glm::vec2(h, h)));
	gradient2 = roundVector(simulation.kernelGradient(glm::vec2(0.f, 0.f), glm::vec2(-h, -h)));
	EXPECT_EQ(gradient1, -gradient2);
	EXPECT_EQ(gradient1, roundVector(glm::vec2(-1 / (h * sqrt(2.f)) * alpha * beta, -1 / (10 * sqrt(2.f)) * alpha * beta)));

	gradient1 = roundVector(simulation.kernelGradient(glm::vec2(0.f, 0.f), glm::vec2(h, -h)));
	gradient2 = roundVector(simulation.kernelGradient(glm::vec2(0.f, 0.f), glm::vec2(-h, h)));
	EXPECT_EQ(gradient1, -gradient2);
	EXPECT_EQ(gradient1, roundVector(glm::vec2(-1 / (h * sqrt(2.f)) * alpha * beta, 1 / (10 * sqrt(2.f)) * alpha * beta)));

	std::vector<unsigned int> neighbors = simulation.getNeighbors(testingParticle, grid);
	glm::vec2 sumV = glm::vec2(0.f, 0.f);
	glm::mat2 sumM = glm::mat2(0);
	for (auto& neighbor : neighbors)
	{
		glm::vec2 xi = glm::vec2(particles[testingParticle].position.x, particles[testingParticle].position.y);
		glm::vec2 xj = glm::vec2(particles[neighbor].position.x, particles[neighbor].position.y);
		sumV += simulation.kernelGradient(xi, xj);
		sumM += glm::outerProduct(xi - xj, simulation.kernelGradient(xi, xj));
	}
	sumV = roundVector(sumV);
	sumM = roundMatrix(sumM);
	EXPECT_EQ(sumV, glm::vec2(0.f, 0.f));
	EXPECT_EQ(roundFloat(glm::row(sumM, 0).x, 1000), glm::row(-simulation.getFluidDensity() / simulation.getParticleMass() * glm::mat2(1.f), 0).x);
	EXPECT_EQ(glm::row(sumM, 0).y, glm::row(-simulation.getFluidDensity() / simulation.getParticleMass() * glm::mat2(1.f), 0).y);
	EXPECT_EQ(glm::row(sumM, 1).x, glm::row(-simulation.getFluidDensity() / simulation.getParticleMass() * glm::mat2(1.f), 1).x);
	EXPECT_EQ(roundFloat(glm::row(sumM, 1).y, 1000), glm::row(-simulation.getFluidDensity() / simulation.getParticleMass() * glm::mat2(1.f), 1).y);
}

TEST_F(SimulationTest, GridCellIndexTest)
{
	EXPECT_EQ(grid.getCellIndex(glm::vec2(0, 0)), 0);
	EXPECT_EQ(grid.getCellIndex(glm::vec2(10, 10)), 0);
	EXPECT_EQ(grid.getCellIndex(glm::vec2(20, 19)), 1);
	EXPECT_EQ(grid.getCellIndex(glm::vec2(20, 20)), 52);
	EXPECT_EQ(grid.getCellIndex(glm::vec2(19, 20)), 51);
}

TEST_F(SimulationTest, GridCounterTest)
{
	const std::vector<unsigned int> counter = grid.getCounter();
	EXPECT_EQ(counter[0], 0);
	EXPECT_EQ(counter[1], 4);
	EXPECT_EQ(counter[2], 8);
	EXPECT_EQ(counter[51], 200);
	EXPECT_EQ(counter[102], 400);
}

TEST_F(SimulationTest, GridSortedListTest)
{
	const std::vector<unsigned int> sortedList = grid.getSortedList();
	EXPECT_EQ(sortedList.size(), particles.size());
	EXPECT_EQ(grid.getCellIndex(particles[sortedList[0]].position), 0);
	EXPECT_EQ(grid.getCellIndex(particles[sortedList[4]].position), 1);
	EXPECT_EQ(grid.getCellIndex(particles[sortedList[5]].position), 1);
	EXPECT_EQ(grid.getCellIndex(particles[sortedList[100]].position), 25);
	EXPECT_EQ(grid.getCellIndex(particles[sortedList[200]].position), 51);
}