#pragma once

enum SpacePartMethod { KDTREE, CELLS, BSPTREE };

class Simulation {
	public :
		static void setSpacePartMethod (SpacePartMethod method);
		static SpacePartMethod getSpacePartMethod();

	protected :
		static usedPartMethod;
};