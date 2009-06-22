#include <iostream>
#include <list>
#include "kdtree.h"
#include "Avatar.h"
#include "myutils.h"

#define NUM_PLAYERS 750
#define NUM_SERVERS 8

#define BG_IMAGE "bg.bmp"
#define PLAYER_IMAGE "player.bmp"
#define PLAYER_ZERO_IMAGE "player0.bmp"
#define PLAYER_SEEN_IMAGE "seen.bmp"

void checkInput();

int main () {
	//srand(time(NULL));

	SDL_Surface* screen = NULL;
	
	setSdl(&screen);
	
	list<Avatar*> avatar_list;
	
	for (int i = 0 ; i < NUM_PLAYERS ; i++) {
		Avatar* av = new Avatar();
		av->setDrawable(PLAYER_IMAGE, PLAYER_SEEN_IMAGE, screen);
		avatar_list.push_back(av);
	}
	
	KDTree* kdt = new KDTree(NUM_SERVERS, avatar_list);
	
	Uint32 time = 0;
	Uint32 step_delay = 250;		
	SDL_EnableKeyRepeat(400, 10);
	SDL_Surface* bg = NULL;
	TTF_Font *font = NULL;	
	bg = load_image (BG_IMAGE);
	if (!bg) cerr << "\nErro setando a imagem de plano de fundo: " << BG_IMAGE << endl;
	font = TTF_OpenFont( "FreeSansBold.ttf", 10 );
	if (!font) cerr << "Erro carregando a fonte" << endl;
	
	while (1) {
		apply_surface(0,0,bg,screen);
		step_delay = SDL_GetTicks() - time;
		step_delay = step_delay > 40 ? 40 : step_delay;

		for (list<Avatar*>::iterator it = avatar_list.begin() ; it != avatar_list.end() ; it++){
			(*it)->step(100/* * step_delay*/);
			
		}
		//time = SDL_GetTicks();
		time += 100;

		for (list<Avatar*>::iterator it = avatar_list.begin() ; it != avatar_list.end() ; it++){
			(*it)->draw();
		}
		if (!(time % 10000)) cout << "Step no tempo " << time << endl;		
		SDL_Flip( screen );
		checkInput();
 }
	
	return 0;
}

void checkInput() {
  int i = 0;
	SDL_Event event;
  if( SDL_PollEvent( &event ) ) { //If a key was pressed 
    if( event.type == SDL_KEYDOWN ) { 
      switch( event.key.keysym.sym ) {
/*
        case SDLK_h:
          toggleShowHelp();
          break;
        case SDLK_y:
          Region::rebalance_progrega(Region::getRegionList());
          break;
        case SDLK_u:
          Region::improveBalancing_kwise(Region::getRegionList()); //aquele que eu achei que podia ser o melhor... ver comentário
          break;
        case SDLK_i:
          Region::rebalance_progrega_kf(Region::getRegionList());
          break;
        case SDLK_o:
          Region::rebalance_progrega_kh(Region::getRegionList());
          break;
        case SDLK_t:
          Region::rebalance_bfbct(Region::getRegionList());
          break;
*/

        case SDLK_p:
          Avatar::toggleMobility();
          break;
/*
        case SDLK_v:
          Cell::toggleShowVertexWeight();
          break;
        case SDLK_e:
          Cell::toggleShowEdgeWeight();
          break;
        case SDLK_r:
          Region::toggleShowRegions();
          break;
        case SDLK_w:
          Region::toggleShowRegionWeight();
          break;
        case SDLK_k:
          Region::refinePartitioningGlobal();
          break;
        case SDLK_l:
          Region::refineKL_kwise(Region::getRegionList());
          break;
        case SDLK_g:
          plotAllAvatars("avatarspositions.data");
          break;
        case SDLK_d:
          Server::releaseAllRegions();
          Region::initRegions(NUM_SERVERS);
          for (list<Region*>::iterator it = Region::getRegionList().begin() ; it != Region::getRegionList().end() ; it++) {
            server[i]->assignRegion(*it);
            i++;
            cout << "Server " << i << " receives region " << *it << endl;
          }
          Region::partitionWorld();
          Region::checkAllRegionsNeighbors();
          i = 0;
*/
          break;
        case SDLK_q: {
					cout << "Finalizado pelo usuário" << endl;
          exit(0);
				}
      }
    }
    if( event.type == SDL_QUIT ) {
			cout << "Finalizado pelo usuário" << endl;
      exit(0);
    }
  }    
}
