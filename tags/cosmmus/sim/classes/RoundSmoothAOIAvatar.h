#pragma once

#include "Avatar.h"

class RoundSmoothAOIAvatar : public Avatar {
	
	public:
	
		double OtherRelevance(Avatar* other) {
			
			float ox = other->GetX();
			float oy = other->GetY();			
			
			float dist = distance(posx, posy, ox, oy);
			
			if (dist == 0.0) dist = 0.0001;
			
			float relevance;
			
			if (dist < THRESHOLD_DISTANCE) relevance = 1.0;
			else {
												
				relevance = 1 - ( ((double)(dist) - (double)THRESHOLD_DISTANCE) / 
										((double)(VIEW_DISTANCE) - (double)(THRESHOLD_DISTANCE)) );
				
			}
			
			
			
			if (relevance > 1.0) relevance = 1.0;
			if (relevance < 0.0) relevance = 0.0;
			
			other->markAsSeen( relevance );
			return relevance;
			
		}
	
};
