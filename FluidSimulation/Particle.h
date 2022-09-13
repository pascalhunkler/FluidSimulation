#pragma once
#include <glm/glm.hpp>

struct Particle
{
	glm::vec2 position;
	glm::vec3 color;
	bool boundary;
	glm::vec2 velocity = glm::vec2(0.f, 0.f);
	float density = 0;
	float pressure = 0;
};
