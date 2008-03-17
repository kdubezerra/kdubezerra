#pragma once

#include "Avatar.h"

class EveryoneAvatar : public Avatar {
	
	public:
	
		double OtherRelevance(Avatar* other) {
			return 1;
		}
	
};
