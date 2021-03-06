#include "../headers/myutils.h"

#include <SDL/SDL.h>

#include <string>
#include <string.h>
#include <fstream>

#define CORE_COUNT 4
#define NUM_SERVERS 8
#define MULTIPLIER 20000

#define REBAL_INTERVAL 1000 // INTERVALO ENTRE REBALANCEAMENTOS, EM MILISSEGUNDOS

#define BG_IMAGE "bg.bmp"
#define VERTEX_IMAGE "vweight.bmp"
#define EDGE_IMAGE "eweight.bmp"
#define PLAYER_IMAGE "player.bmp"
#define PLAYER_ZERO_IMAGE "player0.bmp"
#define PLAYER_SEEN_IMAGE "seen.bmp"
#define EXECUTION_TIME 1200000u

#include "../headers/Avatar.h"
#include "../headers/Cell.h"
#include "../headers/Region.h"
#include "../headers/Server.h"
#include "../headers/KDTree.h"
#include "../headers/BSPTree.h"
#include "../headers/Simulation.h"


bool showing_help = false;

KDTree* kdt;
BSPTree* bspt;
Avatar** player;
Server* server[NUM_SERVERS];
SDL_Surface* screen = NULL;
SDL_Event event;
SDL_sem* msem = NULL;
SDL_sem* tsem = NULL;
SDL_Thread* thread[CORE_COUNT];

SDL_Surface* message = NULL;
TTF_Font *font = NULL;
SDL_Color textColor = { 255, 255, 255 };
Uint32 lastbal = 0;

inflong sum_weight[NUM_SERVERS];
inflong sum_oh[NUM_SERVERS];
long total_mig_walk = 0;
long total_mig_still = 0;

int nplayers;

void checkInput();
void showHelp();
void toggleShowHelp();
int weighter (void* data);
void plotAllAvatars(string filename);

int main (int argc, char* argv[]) {
  ///srand(time(NULL));

  setSdl(&screen);
  SDL_EnableKeyRepeat(400, 10);

  SDL_Surface* bg = NULL;
  bg = load_image (BG_IMAGE);
  if (!bg) cerr << "\nErro setando a imagem de plano de fundo: " << BG_IMAGE << endl;
  tsem = SDL_CreateSemaphore(0);
  msem = SDL_CreateSemaphore(0);
  font = TTF_OpenFont( "FreeSansBold.ttf", 10 );
  if (!font)
    cerr << "Erro carregando a fonte" << endl;

  //Uint32 time = SDL_GetTicks();
  Uint32 time = 0;
  Uint32 step_delay = 250;
  Uint32 dtime = time;
  long unsigned count = 0;
  Color bli;
  bli.R = 255;
  bli.G = 155;
  bli.B = 100;

  nplayers = atoi(argv[1]);
  player = new Avatar*[nplayers];
  int probtohotspot = atoi(argv[2]);
  char rebal_method[255];
  strcpy(rebal_method, argv[3]);

  Avatar::setHotspotsProbability(probtohotspot);

  if (!strcmp(rebal_method,"PROGREGA")) {
    cout << "PROGREGA" << endl;
    Simulation::setSpacePartMethod(CELLS);
    Region::setMethod(PROGREGA);
  } else if (!strcmp(rebal_method,"PROGREGA_KH")) {
    cout << "PROGREGA_KH" << endl;
    Simulation::setSpacePartMethod(CELLS);
    Region::setMethod(PROGREGA_KH);
  } else if (!strcmp(rebal_method,"PROGREGA_KF")) {
    cout << "PROGREGA_KF" << endl;
    Simulation::setSpacePartMethod(CELLS);
    Region::setMethod(PROGREGA_KF);
  } else if (!strcmp(rebal_method,"BFBCT")) {
    cout << "BFBCT" << endl;
    Simulation::setSpacePartMethod(CELLS);
    Region::setMethod(BFBCT);
  } else if (!strcmp(rebal_method,"AHMED")) {
    cout << "AHMED" << endl;
    Simulation::setSpacePartMethod(CELLS);
    Region::setMethod(AHMED);
  } else if (!strcmp(rebal_method,"KDTREE")) {
    cout << "KDTREE" << endl;
    Simulation::setSpacePartMethod(KDTREE);
  } else if (!strcmp(rebal_method, "BSPTREE")) {
    cout << "BSPTREE" << endl;
    Simulation::setSpacePartMethod(BSPTREE);
  }

  for (int i = 0 ; i < NUM_SERVERS ; i++) {
    server[i] = new Server (100000);//((i+1)*MULTIPLIER);
  }

  
  if (Simulation::getSpacePartMethod() == BSPTREE) {
     for (int i = 0 ; i < nplayers ; i++) {
       player[i] = new Avatar();
       player[i]->setDrawable(PLAYER_IMAGE, PLAYER_SEEN_IMAGE, screen);
     }
     bspt = new BSPTree(Server::getServerList(), Avatar::getAvatarList());
     bspt->setScreen(screen);
   }
  if (Simulation::getSpacePartMethod() == KDTREE) {
    for (int i = 0 ; i < nplayers ; i++) {
      player[i] = new Avatar();
      player[i]->setDrawable(PLAYER_IMAGE, PLAYER_SEEN_IMAGE, screen);
    }
    kdt = new KDTree(Server::getServerList(), Avatar::getAvatarList());
    kdt->setScreen(screen);
  }
  else if (Simulation::getSpacePartMethod() == CELLS) {
    Server::releaseAllRegions();
    Region::initRegions(NUM_SERVERS);
    list<Region*>::iterator it = Region::getRegionList().begin();
    for (int i = 0 ; i < NUM_SERVERS ; i++) {
      server[i]->assignRegion(*(it++));
    }
    Cell::allocCellMatrix(15);
    for (int i = 0 ; i < nplayers ; i++) {
      player[i] = new Avatar();
      player[i]->setDrawable(PLAYER_IMAGE, PLAYER_SEEN_IMAGE, screen);
    }
    Cell::setCellSurfaces(VERTEX_IMAGE, EDGE_IMAGE);
    Cell::updateAllEdgesAndVertexWeights();
    Region::partitionWorld();
    Region::checkAllRegionsNeighbors();
  }

  if (!player[0]->setImage(PLAYER_ZERO_IMAGE)) cerr << "\nErro setando a imagem do player 0: " << PLAYER_ZERO_IMAGE << endl;

  //Avatar::toggleMobility(); ///:TODO: fazer com que a simula��o realmente PARE quando chamar essa fun��o

  while (time < EXECUTION_TIME) { // CICLO PRINCIPAL
    //apply_surface(0,0,bg,screen);///*** COMENTE PARA N�O RENDERIZAR
    SDL_FillRect(screen, 0, 0);///*** COMENTE PARA N�O RENDERIZAR

    step_delay = SDL_GetTicks() - time;
    step_delay = step_delay > 40 ? 40 : step_delay;
    for (int i = 0 ; i < nplayers ; i ++) player[i]->step(100/* * step_delay*/);
    //time = SDL_GetTicks();
    time += 100;///:TODO:fazer com que a simula��o inteira PARE mesmo

    
    if (Simulation::getSpacePartMethod() == BSPTREE) {
          bspt->drawTree();
          Server::clearOverhead();
          Avatar::calcWeight();
          bspt->checkBalanceFromRoot();
    }
    if (Simulation::getSpacePartMethod() == KDTREE) {
      kdt->drawTree();
      Server::clearOverhead();
      Avatar::calcWeight();
      kdt->checkBalanceFromRoot();
    }
    else if (Simulation::getSpacePartMethod() == CELLS) {
      //    if (/*SDL_GetTicks()*/ time - dtime >= 25) {
      Cell::updateAllEdgesAndVertexWeights();
      //      dtime = time;
      //    }    
      Cell::drawCells(screen);///*** COMENTE PARA N�O RENDERIZAR
      Region::drawAllRegions(screen);///*** COMENTE PARA N�O RENDERIZAR
      Region::drawAllRegionsWeights(screen, font);///*** COMENTE PARA N�O RENDERIZAR
    }

    for (int i = 0 ; i < nplayers ; i ++) player[i]->draw();///*** COMENTE PARA N�O RENDERIZAR

    //time = SDL_GetTicks();
    showHelp();///*** COMENTE PARA N�O RENDERIZAR
    SDL_Flip( screen );///*** COMENTE PARA N�O RENDERIZAR
    count++;
    checkInput();

    if (time > lastbal + REBAL_INTERVAL) {
      if (Simulation::getSpacePartMethod() == CELLS) {
        Region::checkAllRegionsNeighbors();
        Region::updateAllEdgesAllRegions();
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

      if (Simulation::getSpacePartMethod() == CELLS) {
        for (list<Region*>::iterator it = Region::getRegionList().begin() ; it != Region::getRegionList().end() ; it++) {
          (*it)->checkBalancing();
        }
      }
      lastbal = time;//SDL_GetTicks();
    }
  }
  cout << total_mig_walk << " " << total_mig_still << " " << NUM_SERVERS << " ";
  for (int i = 0 ; i < NUM_SERVERS ; i++) {
    cout << server[i]->getServerPower() << " ";
  }
  for (int i = 0 ; i < NUM_SERVERS ; i++) {
    cout << sum_weight[i].getAverage() << " ";
  }
  for (int i = 0 ; i < NUM_SERVERS ; i++) {
    cout << sum_oh[i].getAverage() << " ";
  }
  cout << endl;

  long pesototaldosservidores = 0;
  for (int i = 0 ; i < NUM_SERVERS ; i++) {  
    pesototaldosservidores += server[i]->getWeight();
  }

  cout << "Peso total (servidores) no �ltimo instante = " << pesototaldosservidores << endl;

  long pesototaldosavatares = 0l;
  for (int i = 0 ; i < nplayers ; i ++) {
    pesototaldosavatares += player[i]->getWeight();
  }
  cout << "Peso total (avatares) no �ltimo instante = " << pesototaldosavatares << endl;

  return 0;
}

void checkInput() {
  int i = 0;
  if( SDL_PollEvent( &event ) ) { //If a key was pressed 
    if( event.type == SDL_KEYDOWN ) { 
      switch( event.key.keysym.sym ) {
    case SDLK_h:
      toggleShowHelp();
      break;
    case SDLK_y:
      Region::rebalance_progrega(Region::getRegionList());
      break;
    case SDLK_u:
      Region::improveBalancing_kwise(Region::getRegionList()); //aquele que eu achei que podia ser o melhor... ver coment�rio
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
    case SDLK_p:
      Avatar::toggleMobility();
      break;
    case SDLK_v:
      Cell::toggleShowVertexWeight();
      break;
    case SDLK_e:
      Cell::toggleShowEdgeWeight();
      break;
    case SDLK_r:
      Region::toggleDisplayRegions();
      KDTree::toggleDisplayRegions();
      BSPTree::toggleDisplayRegions();
      break;
    case SDLK_w:
      Region::toggleDisplayRegionsWeights();
      KDTree::toggleDisplayRegionsWeights();
      BSPTree::toggleDisplayRegionsWeights();
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
      break;
    case SDLK_q:
      exit(0);
      }
    }
    if( event.type == SDL_QUIT ) {
      exit(0);
    }
  }    
}

int weighter (void* data) {
  long my_id = (long) data;
  cout << "Thread: Minha id �: " << my_id << endl;

  while (1) {
    SDL_SemWait(tsem);

    for (int i = my_id * approx(WW / CORE_COUNT) ; i < (my_id + 1)*approx(WW / CORE_COUNT) ; i++){
      for( int j = 0 ; j < WW ; j++ ){
        player[i]->checkCellWeight(player[j]);
      }
    }
    SDL_SemPost(msem);
  }
}

void toggleShowHelp() {
  showing_help = !showing_help;
}

void showHelp() {
  if (!showing_help) return;

  static SDL_Surface* helpSurf[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  SDL_Color txtColor;
  static coord helpPos;
  helpPos.X = helpPos.Y = 10;
  //string helpTxt = "p : pausa\nv : mostrar pesos das c�lulas\ne : mostrar peso das arestas\nd : fazer todo particionamento do 0\nr : mostrar regi�es\nw : mostrar pesos das regi�es\n";
  static string helpTxt[] = {
    "d : particionamento global, do zero",
    "y : particionamento local, do zero",
    "u : distribuir celulas mais leves a regioes que se aproximem da carga ideal",
    "t : v2 (NAO!), getCellWithWeightLowerThanButClosestTo",
    "o : v3 (parece funcionar, mas *NAO* da forma ideal), mantem a celula mais pesada",
    "i : v4 (NAO! bagunca muito as regioes), libera as celulas mais leves e algum outro server pega",
    "PROPOSTA: P-Y-L! melhor ate entao. (P-I-L tambem eh bom)"
  };
  for (int i = 0 ; i < 7 ; i++) {
    if (helpSurf[i]) SDL_FreeSurface(helpSurf[i]);
  }
  txtColor.r = (Uint8) 0xFF;
  txtColor.g = (Uint8) 0xFF;
  txtColor.b = (Uint8) 0xFF;
  for (int i = 0 ; i < 7 ; i++) {
    helpSurf[i] = TTF_RenderText_Blended(font, helpTxt[i].c_str(), txtColor);
    apply_surface(helpPos.X, helpPos.Y + 15*i, helpSurf[i], screen);
  }  


}

void plotAllAvatars(string filename) {
  ofstream datafile;
  datafile.open(filename.c_str());
  for (int i = 0 ; i < nplayers ; i++) {
    datafile << player[i]->GetX() << " " << player[i]->GetY() << endl;
  }
}
