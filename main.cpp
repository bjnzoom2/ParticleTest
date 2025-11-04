#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gl2d.h"
#include "particle.h"

struct simData {
	std::vector<Particle> particles = {};
	std::vector<std::vector<float>> attFactorMat = { {1, 0},
													 {0, 1} };

	std::vector<glm::vec4> colors = {
		Colors_Red, Colors_Orange
	};
};

gl2d::Renderer2D renderer;
simData data;

unsigned int windowWidth = 800;
unsigned int windowHeight = 800;

bool gameLogic(GLFWwindow* window, float deltatime) {
	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT);
	renderer.updateWindowMetrics(windowWidth, windowHeight);
	renderer.clearScreen({ 0.0f, 0.0f, 0.0f, 0.0f });

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	for (int i = 0; i < data.particles.size(); i++) {
		data.particles[i].render(renderer);
		for (int j = 0; j < data.particles.size(); j++) {
			if (i == j) {
				continue;
			}
			data.particles[i].getForce(100, data.attFactorMat[data.particles[i].colorID][data.particles[j].colorID], data.particles[j]);
		}
	}

	for (int i = 0; i < data.particles.size(); i++) {
		data.particles[i].step(1, deltatime);
	}

	renderer.flush();

	glfwSwapBuffers(window);
	glfwPollEvents();

	return true;
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Particles", NULL, NULL);

	if (!window) {
		std::cerr << "Failed to load window\n";
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	for (unsigned int i = 0; i < 100; i++) {
		glm::vec2 vector;
		glm::vec4 color;
		int colorNum = rand() % data.colors.size();
		for (unsigned int j = 0; j < 2; j++) {
			vector[j] = rand() % 801;

			for (int k = 0; k < data.colors.size(); k++) {
				if (colorNum == k) {
					color = data.colors[k];
				}
			}
		}
		Particle particle(vector, color, data.colors);
		data.particles.push_back(particle);
	}

	gl2d::init();
	renderer.create();

	float lastframe = 0;
	float deltatime;

	while (!glfwWindowShouldClose(window)) {
		float currentframe = glfwGetTime();
		deltatime = currentframe - lastframe;
		lastframe = currentframe;

		gameLogic(window, deltatime);
	}

	renderer.cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}