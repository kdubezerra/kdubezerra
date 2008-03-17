#pragma once

#include "Avatar.h"

class CosmmusSmoothAvatar : public Avatar {
	
	public:
	
		double OtherRelevance(Avatar* other) {
			
			float ox = other->GetX();
			float oy = other->GetY();
			
			float dist = distance (posx, posy, ox, oy);
			
			if (dist < THRESHOLD_DISTANCE) {
				other->markAsSeen(1.0);
				return 1;				
			}
			
			if (/*dist < VIEW_DISTANCE && */belongsToVisibility(posx, posy, incr_x, incr_y, ox, oy)) {
				double relevance = 1 - ( 
												
												 ( (double)(dist)-(double)(THRESHOLD_DISTANCE) )
												 
												 / 
							
												 ( (double)(VIEW_DISTANCE)-(double)(THRESHOLD_DISTANCE) ) 
							
											  );
				
				if (relevance < 0) relevance = 0;
				other->markAsSeen(relevance);
				return relevance;
			}
			
			return 0;
		}
	
};
