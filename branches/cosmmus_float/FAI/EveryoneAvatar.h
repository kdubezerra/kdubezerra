#pragma once

#include "Avatar.h"

class EveryoneAvatar : public Avatar {
	
	public:
	
		double OtherRelevance(Avatar* other) {
			other->markAsSeen( 1.0f );
			return 1.0f;
		}
	
};
