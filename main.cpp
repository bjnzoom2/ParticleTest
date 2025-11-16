#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gl2d.h"
#include "grid.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

unsigned int windowWidth = 800;
unsigned int windowHeight = 800;
unsigned int worldWidth = 40000;
unsigned int worldHeight = 40000;

struct simData {
	int range = 100;
	int gridSize = 67;
	int numParticles = 1000;
	float forcefactor = 1;
	std::vector<Particle> particles = {};
	std::unique_ptr<Grid> grid;
	std::vector<std::vector<float>> attFactorMat = { {1, 0, 0, 0},
													 {0, 1, 0, 0},
													 {0, 0, 1, 0},
													 {0, 0, 0, 1} };

	std::vector<glm::vec4> colors = {
		Colors_Red, Colors_Orange, Colors_Yellow, Colors_Green
	};
};

gl2d::Renderer2D renderer;
simData data;

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
	}

	for (int i = 0; i < data.grid->getGrid().size(); i++) {
		int x = i % data.grid->getGridX();
		int y = i / data.grid->getGridX();

		Cell& cell = data.grid->getGrid()[i];

		for (int j = 0; j < cell.particles.size(); j++) {
			Particle* particle = cell.particles[j];
			for (int k = j + 1; k < cell.particles.size(); k++) {
				particle->getForce(data.range, data.attFactorMat[particle->colorID][cell.particles[k]->colorID] * data.forcefactor, *cell.particles[k]);
			}
			Cell* neighCell;
			if (x > 0) {
				neighCell = data.grid->getCell(x - 1, y);
				for (int k = 0; k < neighCell->particles.size(); k++) {
					particle->getForce(data.range, data.attFactorMat[particle->colorID][neighCell->particles[k]->colorID] * data.forcefactor, *neighCell->particles[k]);
				}
				if (y > 0) {
					neighCell = data.grid->getCell(x - 1, y - 1);
					for (int k = 0; k < neighCell->particles.size(); k++) {
						particle->getForce(data.range, data.attFactorMat[particle->colorID][neighCell->particles[k]->colorID] * data.forcefactor, *neighCell->particles[k]);
					}
				}
				if (y < data.grid->getGridY() - 1) {
					neighCell = data.grid->getCell(x - 1, y + 1);
					for (int k = 0; k < neighCell->particles.size(); k++) {
						particle->getForce(data.range, data.attFactorMat[particle->colorID][neighCell->particles[k]->colorID] * data.forcefactor, *neighCell->particles[k]);
					}
				}
			}
			if (y > 0) {
				neighCell = data.grid->getCell(x, y - 1);
				for (int k = 0; k < neighCell->particles.size(); k++) {
					particle->getForce(data.range, data.attFactorMat[particle->colorID][neighCell->particles[k]->colorID] * data.forcefactor, *neighCell->particles[k]);
				}
			}
		}
	}

	for (int i = 0; i < data.particles.size(); i++) {
		data.particles[i].step(1, deltatime, data.grid.get());
	}

	renderer.flush();

	int fps = 1 / deltatime;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");

	ImGui::Text("FPS: %d", fps);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	glfwPollEvents();

	return true;
}

int main() {
	srand(time(NULL));

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

	data.grid = std::make_unique<Grid>(worldWidth, worldHeight, data.gridSize);
	data.particles.reserve(data.numParticles);

	for (int i = 0; i < data.attFactorMat.size(); i++) {
		for (int j = 0; j < data.attFactorMat[i].size(); j++) {
			data.attFactorMat[i][j] = float(rand() % 201) / 100 - 1;
			std::cout << data.attFactorMat[i][j] << ' ';
		}
		std::cout << '\n';
	}

	for (int i = 0; i < data.numParticles; i++) {
		glm::dvec2 vector;
		glm::vec4 color;
		int colorNum = rand() % data.colors.size();
		for (int j = 0; j < 2; j++) {
			vector[j] = rand() % 801;
		}

		color = data.colors[colorNum];

		data.particles.emplace_back(vector, color, data.colors);
		data.grid->addParticle(&data.particles.back());
	}

	gl2d::init();
	renderer.create();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

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