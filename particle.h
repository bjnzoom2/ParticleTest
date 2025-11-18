#pragma once
#include "gl2d.h"

struct Cell;
class Grid;

class Particle {
public:
	Particle() = default;
	Particle(glm::vec2 partPos, glm::vec4 partCol, std::vector<glm::vec4> colors);

	glm::vec2 position = {};
	glm::vec2 velocity = {};
	glm::vec4 color;
	int colorID;
	Cell* ownerCell = nullptr;
	int cellIndex = -1;

	const float radius = 2.0f;

	glm::vec2 totalforce = {};

	void render(gl2d::Renderer2D& renderer);

	void getForce(float range, float attFactor, Particle& otherParticle);

	void step(float fricHalfLife, float deltatime, Grid* grid);
};