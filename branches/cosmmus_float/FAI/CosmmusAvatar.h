#pragma once

#include "Avatar.h"

class CosmmusAvatar : public Avatar {
	
	public:
	
		double OtherRelevance(Avatar* other) {
			
			float ox = other->GetX();
			float oy = other->GetY();
			
			float dist = distance (posx, posy, ox, oy);
			
			if (dist < THRESHOLD_DISTANCE) {
				other->markAsSeen(1.0);
				return 1;				
			}
			
			if (dist < VIEW_DISTANCE && belongsToVisibility(posx, posy, incr_x, incr_y, ox, oy)) {
				other->markAsSeen(1.0);
				return 1;
			}
			
			return 0;
		}
	
};
