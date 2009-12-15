#include <iostream>
#include <vector>
#include <list>
#include "kdtree.h"
#include "Avatar.h"
#include "Server.h"
#include "myutils.h"


//#define NUM_PLAYERS 750
#define NUM_SERVERS 8
#define MULTIPLIER 20000

#define BG_IMAGE "bg.bmp"
#define PLAYER_IMAGE "player.bmp"
#define PLAYER_ZERO_IMAGE "player0.bmp"
#define PLAYER_SEEN_IMAGE "seen.bmp"

#define REBAL_INTERVAL 1000 // INTERVALO ENTRE REBALANCEAMENTOS, EM MILISEGUNDOS
#define EXECUTION_TIME 1200000u


void checkInput();

int main (int arc, char** argv) {
	//srand(time(NULL));

  int NUM_PLAYERS = atoi(argv[1]);
  int HOTSPOTS_CONCENTRATION = atoi(argv[2]);

	SDL_Surface* screen = NULL;
	
	setSdl(&screen);
  
	list<Avatar*> avatar_list;
	list<Server*> server_list;
	Server* server[NUM_SERVERS];
	for (int i = 0 ; i < NUM_PLAYERS ; i++) {
		Avatar* av = new Avatar();
		//av->setDrawable(PLAYER_IMAGE, PLAYER_SEEN_IMAGE, screen);
		avatar_list.push_back(av);
	}
  Avatar::setHotspotsProbability(HOTSPOTS_CONCENTRATION);
	
  
  
	for (int i = 0 ; i < NUM_SERVERS ; i++) {
		//_s = new Server ((i+1)*20000);
		server[i] = new Server ((i+1)*MULTIPLIER);
		server_list.push_back(server[i]);
		//server[i]->assignRegion(*(it++));
		cout << "Server " << i << " has power of " << server[i]->getServerPower() << endl;
	}
  
	Server* sampleserver = server[1]; ///servidor 2
	//Avatar::toggleMobility();
	KDTree* kdt = new KDTree(server_list, avatar_list);
	kdt->setScreen(screen);
	//server_list.reverse();//* só pra ver se a ordem do checkBalance muda alguma coisa significativamente*//
	
	Uint32 time = 0;
	Uint32 step_delay = 250;
	Uint32 lastbal = 0;
	SDL_EnableKeyRepeat(400, 10);
	SDL_Surface* bg = NULL;
	TTF_Font *font = NULL;	
	bg = load_image (BG_IMAGE);
	if (!bg) cerr << "\nErro setando a imagem de plano de fundo: " << BG_IMAGE << endl;
	font = TTF_OpenFont( "FreeSansBold.ttf", 10 );
	if (!font) cerr << "Erro carregando a fonte" << endl;
	long total_mig_walk = 0;
  long total_mig_still = 0;
  inflong sum_weight[NUM_SERVERS];
  inflong sum_oh[NUM_SERVERS];
  
	while (time < EXECUTION_TIME) {
		//apply_surface(0,0,bg,screen);
		//step_delay = SDL_GetTicks() - time;
		//step_delay = step_delay > 40 ? 40 : step_delay;
		//time = SDL_GetTicks();
		time += 100;

		for (list<Avatar*>::iterator it = avatar_list.begin() ; it != avatar_list.end() ; it++){
			(*it)->step(100/* * step_delay*/);
		}
		
		
		//kdt->drawTree();
    //kdt->checkBalanceFromRoot();
    //(*sampleserver)->getNode()->checkBalance();
    //boxRGBA(screen, 20, 20, 200, 200, 255, 127, 63, 255);
    Server::clearOverhead();
		Avatar::calcWeight();
    
    if (time > lastbal + REBAL_INTERVAL) {

      for (int i = 0 ; i < NUM_SERVERS ; i++) {
        server[i]->getNode()->checkBalance();
      }

      cout << time << " " << Avatar::getMigrationWalk(false) << " " << Avatar::getMigrationStill(false) << " " << Server::getUsageDeviation() << " " << NUM_SERVERS << " ";
      total_mig_walk += Avatar::getMigrationWalk();
      total_mig_still += Avatar::getMigrationStill();
      
      for (int i = 0 ; i < NUM_SERVERS ; i++) {
        cout << server[i]->getWeight() << " ";
        sum_weight[i].add(server[i]->getWeight());
      }
      
      for (int i = 0 ; i < NUM_SERVERS ; i++) {
        cout << server[i]->getOverhead() << " ";
        sum_oh[i].add(server[i]->getOverhead());
      }
      cout << endl;

      lastbal = time;
    }

		for (list<Avatar*>::iterator it = avatar_list.begin() ; it != avatar_list.end() ; it++){
			//(*it)->draw();
		}
    
    //stringColor(screen, WW/2, WW/2, longToString(approxLong(1000.0f / (float) step_delay)).c_str(), 0xFFFFFFFF);
		//SDL_Flip( screen );
		//checkInput();
	}

  cout << total_mig_walk << " " << total_mig_still << " " << NUM_SERVERS << " ";
  for (int i = 0 ; i < NUM_SERVERS ; i++) {
    cout << server[i]->getServerPower() << " ";
  }
  for (int i = 0 ; i < NUM_SERVERS ; i++) {
    cout << sum_weight[i].getAverage() << " ";
  }
  for (int i = 0 ; i < NUM_SERVERS ; i++) {
    cout << sum_oh[i].getAverage() << "  ";
  }
  cout << endl;
  
  long pesototaldosservidores = 0;
  for (int i = 0 ; i < NUM_SERVERS ; i++) {  
    pesototaldosservidores += server[i]->getWeight();
  }
  
  cout << "Peso total (servidores) no último instante = " << pesototaldosservidores << endl;
  
  long pesototaldosavatares = 0l;
  for (list<Avatar*>::iterator it = avatar_list.begin() ; it != avatar_list.end() ; it++){
    pesototaldosavatares += (*it)->getWeight();
	}
  cout << "Peso total (avatares) no último instante = " << pesototaldosavatares << endl;
  
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
