

#include "myutils.h"



#ifdef _SDL_H
	SDL_Surface* screen;
	SDL_Surface* bg;
	SDL_Surface* player[nplayers];
#endif

class Avatar {

	public:

		Avatar() {			
			posx = rand() % WW;
			posy = rand() % WW;
			dirx = rand() % WW;
			diry = rand() % WW;
			destx = rand() % WW;
			desty = rand() % WW;
	
			R = rand() % 255;
			G = rand() % 255;
			B = rand() % 255;
	
			my_surface = load_image("player.bmp");
			seen_surface = load_image("seen.bmp");
	
			last_move = 0;
			isSeen = false;
		}

		~Avatar() {}

		void step() {			
	
			Uint32 elapsed_time = SDL_GetTicks() - last_move;
			
			if (isSeen) apply_surface( approx(posx), approx(posy), seen_surface, screen );	
			else apply_surface( approx(posx), approx(posy), my_surface, screen );
			isSeen = false;
			
			float distance_ = distance(posx, posy, destx, desty);
	
			if (distance_ > 20) {
	
				int speed = rand() % 5 + 1;				
				double incr_y =  speed * (desty - posy) / distance(posx, posy, destx, desty); // sin of the direction angle
				double incr_x =  speed * (destx - posx) / distance(posx, posy, destx, desty); // cossin of the direction angle				
				incr_y *= (double)elapsed_time/250;
				incr_x *= (double)elapsed_time/250;
				posy += incr_y;
				posx += incr_x;			
				if (posx < 1) posx = 1;
				if (posx >= WW) posx = WW - 1;
				if (posy < 1) posy = 1;
				if (posy >= WW) posy = WW - 1;
		
				last_move = SDL_GetTicks();
				resting_time = rand () % MAX_RESTING_TIME; //just for the case in which this is its last move towards its destination...
		
			} else {
	
				if (elapsed_time < resting_time) return; //only chooses a new destination with a 0.05 probability
				destx = rand() % WW;
				desty = rand() % WW;
				last_move = SDL_GetTicks();

			}

		}
		
		void markAsSeen(double relevance) {
			isSeen = true;			
		}

		virtual double OtherRelevance(Avatar* other) = 0;
	
		float GetX() { return posx; }
		float GetY() { return posy; }



	protected:

		SDL_Surface* my_surface;
		SDL_Surface* seen_surface;
		float posx, posy;
		int dirx, diry;
		int destx, desty;
		unsigned char R,G,B;
		Uint32 last_move, resting_time;
		bool isSeen;

};
