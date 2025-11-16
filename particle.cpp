#include "particle.h"
#include "grid.h"

Particle::Particle(glm::dvec2 partPos, glm::vec4 partCol, std::vector<glm::vec4> colors) : position(partPos), color(partCol) {
	for (int i = 0; i < colors.size(); i++) {
		if (color == colors[i]) {
			colorID = i;
		}
	}
}
Particle& Particle::operator=(const Particle& other) {
	if (this == &other) return *this;

	position = other.position;
	velocity = other.velocity;
	color = other.color;
	colorID = other.colorID;
	ownerCell = other.ownerCell;
	cellIndex = other.cellIndex;

	totalforce = other.totalforce;

	return *this;
}

void Particle::render(gl2d::Renderer2D& renderer) {
	renderer.renderCircleOutline(position, radius, color, 1, 200);
}

void Particle::getForce(float range, float attFactor, Particle& otherParticle) {
	double distance = glm::distance(position, otherParticle.position) / range;
	glm::dvec2 direction = glm::normalize(otherParticle.position - position);
	const float beta = 0.3;
	if (distance < beta) {
		totalforce += (distance / beta - 1) * direction * (double)range;
	}
	else if (beta < distance && distance < 1) {
		totalforce += attFactor * (1 - std::abs(2 * distance - 1 - beta) / (1 - beta)) * direction * (double)range;
	}
}

void Particle::getForce(float attFactor, Particle& otherParticle) {
	int gridSize = 67;
	getForce(gridSize, attFactor, otherParticle);
}

void Particle::step(float fricHalfLife, float deltatime, Grid* grid) {
	/*if (position.x > 803) {
		position.x = -3;
	}
	if (position.x < -3) {
		position.x = 803;
	}
	if (position.y > 803) {
		position.y = -3;
	}
	if (position.y < -3) {
		position.y = 803;
	}*/

	velocity = std::pow(0.5, deltatime / fricHalfLife) * velocity + totalforce * (double)deltatime;
	position += velocity * (double)deltatime;

	Cell* newCell = grid->getCell(position);
	if (newCell != ownerCell) {
		grid->removeParticle(this);
		grid->addParticle(this, newCell);
	}

	totalforce = glm::dvec2(0);
}