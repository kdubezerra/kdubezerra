#include "../headers/Simulation.h"

SpacePartMethod Simulation::setSpacePartMethod = CELLS;

bool Simulation::getSpacePartMethod() {
	return usedPartMethod;
}

void Simulation::setSpacePartMethod (SpacePartMethod method) {
	usedPartMethod = method;
}