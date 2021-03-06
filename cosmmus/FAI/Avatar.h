#pragma once

#include "myutils.h"
#include <vector>

#define VIEW_DISTANCE 120
#define THRESHOLD_DISTANCE 40

using namespace std;

class Avatar {

	public:

		
		Avatar() {
			init();
			isDrawable = false;
			player_id = 555;
		}
		
		
		void setDrawable(string my_surface_file, string seen_surface_file, SDL_Surface* out_screen) {			
			my_surface = load_image(my_surface_file);
			seen_surface = load_image(seen_surface_file);
			isDrawable = true;
			screen = out_screen;
		}

		~Avatar() {}
		
		
		void init() {
			
			do {
				posx = rand() % WW;
				posy = rand() % WW;
				dirx = rand() % WW;
				diry = rand() % WW;
				destx = rand() % WW;
				desty = rand() % WW;				
			} while (distance(posx, posy, destx, desty) <= 20);
			 
			R = rand() % 255;
			G = rand() % 255;
			B = rand() % 255;
	
                        m_numofhotspots = rand() % 20;
                        for (int index = 0 ; index < m_numofhotspots ; index++) {
                          coord newspot;
                          newspot.X = (float) (rand() % WW);
                          newspot.Y = (float) (rand() % WW);
                          mv_hotspotlist.push_back(newspot);
                        }
                        
// 			my_surface = load_image("player.bmp");
// 			seen_surface = load_image("seen.bmp");
	
			last_move = 0;
			isSeen = false;
		}
		
		

		void step(unsigned long delay) { // delay in microseconds
	
			Uint32 elapsed_time = SDL_GetTicks() - last_move;
			
 			if (isDrawable) draw();
			
			float distance_ = distance(posx, posy, destx, desty);
	
			if (distance_ > 20) {
	
				int speed = rand() % 5 + 1;		
				incr_y =  speed * (desty - posy) / distance(posx, posy, destx, desty); // sin of the direction angle
				incr_x =  speed * (destx - posx) / distance(posx, posy, destx, desty); // cossin of the direction angle				
				
				incr_y *= (double)delay/250.0f;
				incr_x *= (double)delay/250.0f;

				posy += incr_y;
				posx += incr_x;							
				
				if (posx < 3) posx = 3;
				if (posx >= WW - 3) posx = WW - 3;
				if (posy < 3) posy = 3;
				if (posy >= WW - 3) posy = WW - 3;
		
				stopped_time = 0;
				
				last_move = SDL_GetTicks();
				resting_time = rand () % MAX_RESTING_TIME; //just for the case in which this is its last move towards its destination...
		
			} else {
				
				stopped_time += delay;
	
				if (stopped_time < resting_time) return; //only chooses a new destination with a 0.05 probability
                                if (rand() % 100 < 50) { //selects a random spot
				  destx = rand() % WW;
				  desty = rand() % WW;
				  last_move = SDL_GetTicks();
                                } 
                                else { //selects a hotspot to go to 
                                  int spot = rand() % m_numofhotspots;
                                  destx = mv_hotspotlist[spot].X;
                                  desty = mv_hotspotlist[spot].Y;
                                }

			}

		}
		
		
		
      void setPlayerId (int i) {
        player_id = i;
      }
		
		
		
		void markAsSeen(double relevance_) {
			relevance = relevance_;
			isSeen = true;			
		}

		
		
		
		
		
		virtual double OtherRelevance(Avatar* other);
	
		float GetX() { return posx; }
		float GetY() { return posy; }
		
		
		
		
		
		
		void* setImage(string filename) {
			my_surface = load_image(filename);
			return (void*) my_surface;
		}
		
		
		
		
		
		
		void draw() {
			if (isSeen) {
				SDL_SetAlpha( seen_surface, SDL_SRCALPHA, approx(255*relevance) );
				apply_surface( approx(posx), approx(posy), my_surface, screen );
				apply_surface( approx(posx), approx(posy), seen_surface, screen );
			}
			else {
				SDL_SetAlpha( my_surface, SDL_SRCALPHA, 255 );
				apply_surface( approx(posx), approx(posy), my_surface, screen );
			}
			isSeen = false;
		}
		




	protected:

		SDL_Surface* my_surface;
		SDL_Surface* seen_surface;
		float posx, posy;
		int dirx, diry;
		int destx, desty;
		unsigned char R,G,B;
		Uint32 last_move, resting_time;
		bool isSeen;
		double incr_y;
		double incr_x;
		double relevance;
		unsigned long stopped_time;
		bool isDrawable;		
		unsigned long player_id;
                int m_numofhotspots;
                vector<coord> mv_hotspotlist;
		
#ifdef _SDL_H
		SDL_Surface* screen;		
#endif

};
