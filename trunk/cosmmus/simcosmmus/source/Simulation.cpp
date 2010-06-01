#include "../headers/Simulation.h"

SpacePartMethod Simulation::usedPartMethod = CELLS;

SpacePartMethod Simulation::getSpacePartMethod() {
	return usedPartMethod;
}

void Simulation::setSpacePartMethod (SpacePartMethod method) {
	usedPartMethod = method;
}