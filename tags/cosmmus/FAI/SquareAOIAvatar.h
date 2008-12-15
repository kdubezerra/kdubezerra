#pragma once

#include "Avatar.h"

class SquareAOIAvatar : public Avatar {
	
	public:
	
		double OtherRelevance(Avatar* other) {
			if (fabs(posx - other->GetX()) < VIEW_DISTANCE && fabs(posy - other->GetY()) < VIEW_DISTANCE) {
				other->markAsSeen(1.0);
				return 1;
			}
			else return 0;
		}
	
};
