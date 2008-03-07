#include "SquareAOIAvatar.h"

int main () {

#ifdef _SDL_H	
	setSdl(&screen);	
#endif	
	srand(0);
	
	bg = load_image ("bg.bmp");
	
	SquareAOIAvatar player[nplayers];
	
// 	for (int i = 0 ; i < WW ; i ++) player[i] = new Avatar();
	
	while (1) {
		apply_surface(0,0,bg,screen);

		for (int i = 1 ; i < nplayers ; i ++) player[0].OtherRelevance(&player[i]);
		for (int i = 0 ; i < nplayers ; i ++) player[i].step();
// 		while (1);
		
		SDL_Flip( screen );
		
// 		SDL_Delay(100);
	}

}


