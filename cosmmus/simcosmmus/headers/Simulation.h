#pragma once

enum SpacePartMethod { CELLS, KDTREE, BSPTREE };

class Simulation {
	public :
		static void setSpacePartMethod (SpacePartMethod method);
		static SpacePartMethod getSpacePartMethod();

	protected :
		static SpacePartMethod usedPartMethod;
};
