#include "particle.h"
#include "grid.h"
#include "gl2d.h"

Particle::Particle(glm::vec2 partPos, glm::vec4 partCol, std::vector<glm::vec4> colors) : position(partPos), color(partCol) {
	for (int i = 0; i < colors.size(); i++) {
		if (color == colors[i]) {
			colorID = i;
		}
	}
}


void Particle::render(gl2d::Renderer2D& renderer) {
	renderer.renderCircleOutline(position, radius, color, 0.5f, 5);
}

void Particle::getForce(float range, float attFactor, Particle& otherParticle) {
	float distance = glm::distance(position, otherParticle.position) / range;
	glm::vec2 direction = glm::normalize(otherParticle.position - position);
	const float beta = 0.3f;
	if (distance < beta) {
		totalforce += (distance / beta - 1.0f) * direction * range;
	}
	else if (beta < distance && distance < 1.0f) {
		totalforce += attFactor * (1.0f - std::abs(2.0f * distance - 1.0f - beta) / (1.0f - beta)) * direction * range;
	}
}

void Particle::step(float fricHalfLife, float deltatime, Grid* grid) {
	velocity = sqrt(deltatime / fricHalfLife) * velocity + totalforce * deltatime;
	position += velocity * deltatime;

	Cell* newCell = grid->getCell(position);
	if (newCell != ownerCell) {
		grid->removeParticle(this);
		grid->addParticle(this, newCell);
	}

	totalforce = glm::vec2(0.0f);
}