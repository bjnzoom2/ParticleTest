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
	int range = 200;
	int gridSize = 133;
	int numParticles = 5000;
	float forcefactor = 5.0f;
	std::vector<Particle> particles = {};
	std::unique_ptr<Grid> grid;

	std::vector<float> attFactorMat = {};

	std::vector<glm::vec4> colors = {
		Colors_Red, Colors_Orange, Colors_Yellow, Colors_Green, Colors_Blue, Colors_Magenta
	};

	bool running = false;
};

gl2d::Renderer2D renderer;
simData data;

bool gameLogic(GLFWwindow* window, float deltatime) {
	glClear(GL_COLOR_BUFFER_BIT);
	renderer.clearScreen({ 0.0f, 0.0f, 0.0f, 1.0f });

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		renderer.currentCamera.position.y -= 1200.0f * deltatime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		renderer.currentCamera.position.y += 1200.0f * deltatime;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		renderer.currentCamera.position.x -= 1200.0f * deltatime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		renderer.currentCamera.position.x += 1200.0f * deltatime;
	}

	int gridX = data.grid->getGridX();
	int gridY = data.grid->getGridY();
	std::vector<Cell> cellGrid = data.grid->getGrid();
	int nColors = data.colors.size();

	for (int i = 0; i < data.particles.size(); i++) {
		data.particles[i].render(renderer);
	}

	if (data.running) {
		for (int i = 0; i < cellGrid.size(); i++) {
			int x = i % gridX;
			int y = i / gridY;

			Cell& cell = cellGrid[i];

			for (int j = 0; j < cell.particles.size(); j++) {
				Particle* particle = cell.particles[j];
				for (int k = j + 1; k < cell.particles.size(); k++) {
					particle->getForce(data.range, data.attFactorMat[particle->colorID * nColors + cell.particles[k]->colorID] * data.forcefactor, *cell.particles[k]);
				}
				Cell* neighCell;
				if (x > 0) {
					neighCell = data.grid->getCell(x - 1, y);
					for (int k = 0; k < neighCell->particles.size(); k++) {
						particle->getForce(data.range, data.attFactorMat[particle->colorID * nColors + neighCell->particles[k]->colorID] * data.forcefactor, *neighCell->particles[k]);
					}
					if (y > 0) {
						neighCell = data.grid->getCell(x - 1, y - 1);
						for (int k = 0; k < neighCell->particles.size(); k++) {
							particle->getForce(data.range, data.attFactorMat[particle->colorID * nColors + neighCell->particles[k]->colorID] * data.forcefactor, *neighCell->particles[k]);
						}
					}
					if (y < data.grid->getGridY() - 1) {
						neighCell = data.grid->getCell(x - 1, y + 1);
						for (int k = 0; k < neighCell->particles.size(); k++) {
							particle->getForce(data.range, data.attFactorMat[particle->colorID * nColors + neighCell->particles[k]->colorID] * data.forcefactor, *neighCell->particles[k]);
						}
					}
				}
				if (y > 0) {
					neighCell = data.grid->getCell(x, y - 1);
					for (int k = 0; k < neighCell->particles.size(); k++) {
						particle->getForce(data.range, data.attFactorMat[particle->colorID * nColors + neighCell->particles[k]->colorID] * data.forcefactor, *neighCell->particles[k]);
					}
				}
			}
		}

		for (int i = 0; i < data.particles.size(); i++) {
			data.particles[i].step(1.0f, deltatime, data.grid.get());
		}
	}

	renderer.flush();

	float fps = 1.0f / deltatime;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");

	ImGui::Text("Particle Count: %d", data.numParticles);
	ImGui::Text("FPS: %f", fps);
	ImGui::SliderFloat("Zoom", &renderer.currentCamera.zoom, 0.1f, 1.0f);

	if (data.running) {
		if (ImGui::Button("Pause")) {
			data.running = !data.running;
		}
	}
	else {
		if (ImGui::Button("Unpause")) {
			data.running = !data.running;
		}
	}

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
	glViewport(0, 0, windowWidth, windowHeight);

	data.grid = std::make_unique<Grid>(worldWidth, worldHeight, data.gridSize);
	data.particles.reserve(data.numParticles);
	data.attFactorMat.resize(data.colors.size() * data.colors.size());

	for (int i = 0; i < data.colors.size(); i++) {
		for (int j = 0; j < data.colors.size(); j++) {
			data.attFactorMat[i * data.colors.size() + j] = static_cast<float>(rand() % 201) / 100.0f - 1.0f;
			std::cout << data.attFactorMat[i * data.colors.size() + j] << ' ';
		}
		std::cout << '\n';
	}

	for (int i = 0; i < data.numParticles; i++) {
		glm::vec2 vector = {};
		glm::vec4 color = {};
		int colorNum = rand() % data.colors.size();
		for (int j = 0; j < 2; j++) {
			vector[j] = static_cast<float>(rand() % 1401) - 400.0f;
		}

		color = data.colors[colorNum];

		data.particles.emplace_back(vector, color, data.colors);
		data.grid->addParticle(&data.particles.back());
	}

	gl2d::init();
	renderer.create();
	renderer.updateWindowMetrics(windowWidth, windowHeight);

	renderer.currentCamera.zoom = 0.25f;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	float lastframe = 0.0f;
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