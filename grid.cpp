#include "grid.h"

Grid::Grid(int worldWidth, int worldHeight, int gridSize) : width(worldWidth), height(worldHeight), cellSize(gridSize) {
	gridX = ceil(float(width / cellSize));
	gridY = ceil(float(height / cellSize));

	cells.resize(gridX * gridY);
	for (int i = 0; i < cells.size(); i++) {
		cells[i].particles.reserve(25);
	}
}

Cell* Grid::getCell(int x, int y) {
	if (x < 0) x = 0;
	if (x > gridX) x = gridX;
	if (y < 0) y = 0;
	if (y > gridY) y = gridY;

	return &cells[y * gridX + x];
}

Cell* Grid::getCell(const glm::dvec2 pos) {
	int cellX = (int)(pos.x / cellSize);
	int cellY = (int)(pos.y / cellSize);

	return getCell(cellX, cellY);
}

void Grid::addParticle(Particle* particle) {
	Cell* cell = getCell(particle->position);
	cell->particles.push_back(particle);
	particle->ownerCell = cell;
	particle->cellIndex = cell->particles.size() - 1;
}

void Grid::addParticle(Particle* particle, Cell* cell) {
	cell->particles.push_back(particle);
	particle->ownerCell = cell;
	particle->cellIndex = cell->particles.size() - 1;
}

void Grid::removeParticle(Particle* particle) {
	std::vector<Particle*>& particles = particle->ownerCell->particles;
	particles[particle->cellIndex] = particles.back();
	particles.pop_back();
	if (particle->cellIndex < particles.size()) { particles[particle->cellIndex]->cellIndex = particle->cellIndex; }
	particle->cellIndex = -1;
	particle->ownerCell = nullptr;
}