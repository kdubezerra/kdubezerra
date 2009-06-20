#include <iostream>
#include <list>
#include 'kdtree.h'
#include 'Avatar.h'

#define NUM_PLAYERS 750

int main () {
  //srand(time(NULL));
  list<Avatar*> avatar_list;
  for (int i = 0 ; i < 100 ; i++) {
    Avatar* av = new Avatar();
    avatar_list.push_back(av);
  }
  
  KDTree* kdt = new KDTree(avatar_list);
  
  Uint32 time = 0;
  Uint32 step_delay = 250;
  
  SDL_Surface* screen = NULL;
  setSdl(&screen);
  SDL_EnableKeyRepeat(400, 10);
  SDL_Surface* bg = NULL;
  bg = load_image (BG_IMAGE);
  if (!bg) cerr << "\nErro setando a imagem de plano de fundo: " << BG_IMAGE << endl;
  font = TTF_OpenFont( "FreeSansBold.ttf", 10 );
  if (!font) cerr << "Erro carregando a fonte" << endl;
  
  while (1) {
    apply_surface(0,0,bg,screen);
    step_delay = SDL_GetTicks() - time;
    step_delay = step_delay > 40 ? 40 : step_delay;
    
    for (list<Avatar>::iterator it = avatar_list.begin() ; it != avatar_list.end() ; it++){
       (*it)->step(100/* * step_delay*/);
    }
    //time = SDL_GetTicks();
    time += 100;
    
    for (int i = 0 ; i < nplayers ; i ++) player[i]->draw();
  }
  
  return 0;
}
