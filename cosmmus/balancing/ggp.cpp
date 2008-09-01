#include "Avatar.h"

int main () {

#ifdef _SDL_H   
  setSdl(&screen);        
#endif  
  srand(time(NULL));
        
  bg = load_image ("bg.bmp");
        
  EveryoneAvatar* player[nplayers];
        
  for (int i = 0 ; i < nplayers ; i++) {
    player[i] = new EveryoneAvatar();
    player[i]->setDrawable("player.bmp", "seen.bmp");
  }       
        
  if (!player[0]->setImage("player0.bmp")) cerr << "\nErro setando a imagem do player 0\n" << endl;
        
//      for (int i = 0 ; i < WW ; i ++) player[i] = new Avatar();
        
  while (1) {
    apply_surface(0,0,bg,screen);

    for (int i = 1 ; i < nplayers ; i ++) player[0]->OtherRelevance(player[i]);
    for (int i = 0 ; i < nplayers ; i ++) player[i]->step(50);
    for (int i = 0 ; i < nplayers ; i ++) player[i]->draw();
//              while (1);
                
    SDL_Flip( screen );
                
    SDL_Delay(50);
  }

}


