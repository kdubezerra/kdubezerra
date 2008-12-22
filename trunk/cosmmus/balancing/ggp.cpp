#ifdef _WIN32
#include "../../myutils.h"
#else
#include "myutils.h"
#endif

#define CORE_COUNT 4
#define NUM_SERVERS 2

#define BG_IMAGE "bg.bmp"
#define VERTEX_IMAGE "vweight.bmp"
#define EDGE_IMAGE "eweight.bmp"
#define PLAYER_IMAGE "player.bmp"
#define PLAYER_ZERO_IMAGE "player0.bmp"
#define PLAYER_SEEN_IMAGE "seen.bmp"

#include "Avatar.h"
#include "Cell.h"
#include "Region.h"
#include "Server.h"



// typedef struct {
//   Avatar** avlist;
//   int thid;
// } thread_data;

Avatar* player[nplayers];
Server* server[NUM_SERVERS];
SDL_Surface* screen = NULL;
SDL_Event event;
SDL_sem* msem = NULL;
SDL_sem* tsem = NULL;
SDL_Thread* thread[CORE_COUNT];
 //thread_data dt[CORE_COUNT];

SDL_Surface* message = NULL;
TTF_Font *font = NULL;
SDL_Color textColor = { 255, 255, 255 };

void checkInput();
int weighter (void* data);

//AQUI COMECA O PROGRAMA

int main (int argc, char* argv[]) {
  srand(time(NULL));
  
  setSdl(&screen);
  SDL_EnableKeyRepeat(400, 10);
  
  for (int i = 0 ; i < NUM_SERVERS ; i++) {
    server[i] = new Server (100*(rand() % 2500));
    cout << "Server " << i << " has power of " << server[i]->getServerPower() << endl;
  }
  

  SDL_Surface* bg = NULL;
  bg = load_image (BG_IMAGE);
  if (!bg) cerr << "\nErro setando a imagem de plano de fundo: " << BG_IMAGE << endl;
  tsem = SDL_CreateSemaphore(0);
  msem = SDL_CreateSemaphore(0);
  font = TTF_OpenFont( "FreeSansBold.ttf", 10 );
  if (!font)
	  cerr << "Erro carregando a fonte" << endl;

  //TODO instanciar as células
  Cell::allocCellMatrix(15);
  Cell::setCellSurfaces(VERTEX_IMAGE, EDGE_IMAGE);

  for (int i = 0 ; i < nplayers ; i++) {
    player[i] = new Avatar();
    player[i]->setDrawable(PLAYER_IMAGE, PLAYER_SEEN_IMAGE, screen);
  }
        
  if (!player[0]->setImage(PLAYER_ZERO_IMAGE)) cerr << "\nErro setando a imagem do player 0: " << PLAYER_ZERO_IMAGE << endl;

  Uint32 time = SDL_GetTicks();
  Uint32 step_delay = 250;
  Uint32 dtime = time;
  long unsigned count = 0;
  Color bli;
  bli.R = 255;
  bli.G = 155;
  bli.B = 100;

  while (1) {
    apply_surface(0,0,bg,screen);

    step_delay = SDL_GetTicks() - time;
    step_delay = step_delay > 40 ? 40 : step_delay;
    for (int i = 0 ; i < nplayers ; i ++) player[i]->step(10*step_delay);
    time = SDL_GetTicks();
    
    if (SDL_GetTicks() - dtime >= 25) {
      Cell::updateAllEdgesAndVertexWeights();
      dtime = SDL_GetTicks();
    }    
    Cell::drawCells(screen);
    Region::drawAllRegions(screen);
    Region::drawAllRegionsWeights(screen, font);
    
    for (int i = 0 ; i < nplayers ; i ++) player[i]->draw();

    time = SDL_GetTicks();    
    apply_surface( 200, 200, message, screen ); 
    SDL_Flip( screen );
    count++;
    checkInput();
  }
}

void checkInput() {
  int i = 0;
  if( SDL_PollEvent( &event ) ) { //If a key was pressed 
    if( event.type == SDL_KEYDOWN ) { 
      switch( event.key.keysym.sym ) {
        case SDLK_o:
          //Region::improveBalancing_kwise(Region::getRegionList());
          Region::improveBalancing_v2(Region::getRegionList());
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
  int my_id = (int) data;
  cout << "Thread: Minha id é: " << my_id << endl;
  
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
