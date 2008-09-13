#include "Avatar.h"
#include "Cell.h"
#include "myutils.h"
#define CORE_COUNT 4

// typedef struct {
//   Avatar** avlist;
//   int thid;
// } thread_data;

Avatar* player[nplayers];
SDL_Surface* screen;
SDL_Event event;
SDL_sem* msem = NULL;
SDL_sem* tsem = NULL;
SDL_Thread* thread[CORE_COUNT];
// thread_data dt[CORE_COUNT];

void checkInput();
int weighter (void* data);

//AQUI COMECA O PROGRAMA

int main () {
  srand(time(NULL));
  
  setSdl(&screen);
  SDL_Surface* bg = load_image ("bg.bmp");
  tsem = SDL_CreateSemaphore(0);
  msem = SDL_CreateSemaphore(0);

  //TODO instanciar as células
  Cell::allocCellMatrix(15);
  
  for (int i = 0 ; i < nplayers ; i++) {
    player[i] = new Avatar();
    player[i]->setDrawable("player.bmp", "seen.bmp", screen);
  }
        
  if (!player[0]->setImage("player0.bmp")) cerr << "\nErro setando a imagem do player 0\n" << endl;

//   for (int i = 0 ; i < CORE_COUNT ; i++) {
//     thread[i] = SDL_CreateThread( weighter , (void*)(i) );
//   }

  Uint32 time = SDL_GetTicks();

  while (1) {    
    apply_surface(0,0,bg,screen);
    
    for (int i = 0 ; i < nplayers ; i ++) player[i]->step((SDL_GetTicks() - time));
    time = SDL_GetTicks();
    
//     for (int i = 0 ; i < CORE_COUNT ; i++)
//       SDL_SemPost(tsem);    
//     for (int i = 0 ; i < CORE_COUNT ; i++)
//       SDL_SemWait(msem);
    
    
//     for (int i = 0 ; i < nplayers ; i ++) player[i]->checkEdgeWeight();
//     Avatar::drawCells(screen);
//     for (int i = 0 ; i < nplayers ; i ++) player[i]->draw();
    
//              while (1);
                
    checkInput();
    
    SDL_Flip( screen );
                
//     SDL_Delay(1500);
    
  }

}

void checkInput() {
  if( SDL_PollEvent( &event ) ) { //If a key was pressed 
    if( event.type == SDL_KEYDOWN ) { 
      switch( event.key.keysym.sym ) {
        case SDLK_v:
          Avatar::toggleVertex();
          break;
        case SDLK_e:
          Avatar::toggleEdge();
          break;
        case SDLK_q:
          exit(0);
      }
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
