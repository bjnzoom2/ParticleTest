#pragma once
#include "particle.h"

struct Cell {
	std::vector<Particle*> particles = {};
};

class Grid {
public:
	Grid(int worldWidth, int worldHeight, int gridSize);

	Cell* getCell(int x, int y);

	Cell* getCell(const glm::dvec2 pos);

	void addParticle(Particle* particle);

	void addParticle(Particle* particle, Cell* cell);

	void removeParticle(Particle* particle);

	std::vector<Cell>& getGrid() { return cells; }
	int getGridX() { return gridX; }
	int getGridY() { return gridY; }

private:
	std::vector<Cell> cells = {};
	int cellSize;
	int width;
	int height;
	int gridX;
	int gridY;
};