#include "gl2d.h"

class Particle {
public:
	Particle(glm::dvec2 partPos, glm::vec4 partCol, std::vector<glm::vec4> colors) : position(partPos), color(partCol) {
		for (int i = 0; i < colors.size(); i++) {
			if (color == colors[i]) {
				colorID = i;
			}
		}
	};

	glm::dvec2 position = { 400, 400 };
	glm::dvec2 velocity = {};
	glm::vec4 color;
	int colorID;

	const float radius = 3;

	glm::dvec2 totalforce = {};

	void render(gl2d::Renderer2D& renderer) {
		renderer.renderCircleOutline(position, radius, color, 5, 200);
	}

	void getForce(float range, float attFactor, Particle& otherParticle) {
		double distance = glm::distance(position, otherParticle.position) / range;
		glm::dvec2 direction = glm::normalize(otherParticle.position - position);
		const float beta = 0.3;
		if (distance < beta) {
			totalforce += (distance / beta - 1) * direction * 800.0;
		}
		else if (beta < distance && distance < 1) {
			totalforce += attFactor * (1 - std::abs(2 * distance - 1 - beta) / (1 - beta)) * direction * 800.0;
		}
	}

	void step(float fricHalfLife, float deltatime) {
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

		totalforce = glm::dvec2(0);
	}
};