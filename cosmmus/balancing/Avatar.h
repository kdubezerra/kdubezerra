#pragma once

#include "myutils.h"
#include <vector>

#define VIEW_DISTANCE 50
#define THRESHOLD_DISTANCE 10
#define CELLS_ON_A_ROW 15
#define CELL_LENGTH int(WW/CELLS_ON_A_ROW)

SDL_sem* vsem = NULL;
SDL_sem* esem = NULL;

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
      
      if (m_numofhotspots) return;
	
      m_numofhotspots = 3 + rand() % 7;
      cout << "Number of hot spots: " << m_numofhotspots << endl;
      for (int index = 0 ; index < m_numofhotspots ; index++) {
        coord newspot;
        newspot.X = rand() % WW;
        newspot.Y = rand() % WW;
        mv_hotspotlist.push_back(newspot);
        
      }
      
      for (int index = 0 ; index < m_numofhotspots ; index++) {
        cout << "X[" << index << "] = " << mv_hotspotlist[index].X << " ; Y[" << index << "] = " << mv_hotspotlist[index].Y << endl;
      }
// 			my_surface = load_image("player.bmp");
// 			seen_surface = load_image("seen.bmp");
	
      last_move = 0;
      isSeen = false;
      
      surface_vertex_weight = load_image("vweight.bmp");
      surface_edge_weight = load_image("eweight.bmp");
      
      vsem = SDL_CreateSemaphore(1);
      vsem = SDL_CreateSemaphore(1);
      
    }
		
		

    void step(unsigned long delay) { // delay in microseconds
	
      Uint32 elapsed_time = SDL_GetTicks() - last_move;
			
//       if (isDrawable) draw();
			
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
        if (rand() % 100 < 15) { //selects a random spot
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

		
		
		
		
		
    float OtherRelevance(Avatar* other) {
                        
      float ox = other->GetX();
      float oy = other->GetY();                        
      float dist = distance (posx, posy, ox, oy);
                        
      if (dist < THRESHOLD_DISTANCE) {
//         other->markAsSeen(1.0);
        return 1;                               
      }                       
                        
      if (dist < VIEW_DISTANCE && belongsToVisibility(posx, posy, incr_x, incr_y, ox, oy)) {
        float relevance = 1 - ( 
                                                                                                
                                ( (float)(dist)-(float)(THRESHOLD_DISTANCE) )
                            
                                                                             
              / 
                                                        
              ( (float)(VIEW_DISTANCE)-(float)(THRESHOLD_DISTANCE) ) 
                                                        
                               );
                                
        if (relevance < 0) relevance = 0;
//         other->markAsSeen(relevance);
        return relevance;
      }
                        
      return 0;
    }
	
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
    
    static void resetCells ();
    
    static void drawCells (SDL_Surface* output);
    
    static void toggleVertex();    
    static void toggleEdge();
    
    coord getCell () {
      coord cell;
      
      cell.X = int (simpleScale(posx, WW, CELLS_ON_A_ROW));
      cell.Y = int (simpleScale(posy, WW, CELLS_ON_A_ROW));
    
      return cell;
    }
    
    void checkCellWeight (Avatar* other) { //por hora é mto simples, apenas contando q o peso da celula = numero de avatares nela
      if (!showv && !showe) return;
      
      coord myCell = getCell();
      coord otherCell = other->getCell();
//       float rel = OtherRelevance(other);
      
      if (otherCell = myCell) {
        SDL_SemWait(vsem);
        vweight_matrix[myCell.X][myCell.Y] += 1;
        SDL_SemPost(vsem);
       } //else {
//         SDL_SemWait(esem);
//         eweight_matrix[myCell.X][myCell.Y] += rel;
//         SDL_SemPost(esem);
//       }
    }
    
    float getWeightE(short praqualcelular) { //mudar pra usar cada celula diferente: getWeightE(UP_LEFT), por exemplo.
      list       
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
    static int m_numofhotspots;
    static vector<coord> mv_hotspotlist;
    static float vweight_matrix[CELLS_ON_A_ROW][CELLS_ON_A_ROW];
    static float eweight_matrix[CELLS_ON_A_ROW][CELLS_ON_A_ROW];
    static SDL_Surface* surface_vertex_weight;
    static SDL_Surface* surface_edge_weight;
    static float total_weight;
    static bool showv, showe;
    float my_vweight, my_eweight;
    
		
#ifdef _SDL_H
    SDL_Surface* screen;		
#endif

};

int Avatar::m_numofhotspots = 0;
vector<coord> Avatar::mv_hotspotlist;
float Avatar::vweight_matrix[CELLS_ON_A_ROW][CELLS_ON_A_ROW];
float Avatar::eweight_matrix[CELLS_ON_A_ROW][CELLS_ON_A_ROW];
SDL_Surface* Avatar::surface_vertex_weight;
SDL_Surface* Avatar::surface_edge_weight;
bool Avatar::showv = true;
bool Avatar::showe = false;

float Avatar::total_weight;

void Avatar::resetCells () {
  for (int i = 0 ; i < CELLS_ON_A_ROW ; i++) {
    for (int j = 0 ; j < CELLS_ON_A_ROW ; j++) {       
      vweight_matrix[i][j] = 0.0f;
      eweight_matrix[i][j] = 0.0f;
    }
  }
  total_weight = 0;
}
    
void Avatar::drawCells (SDL_Surface* output) {
  
  coord cell;
  float alpha;
  
  for (int i = 0 ; i < CELLS_ON_A_ROW ; i++) {
    for (int j = 0 ; j < CELLS_ON_A_ROW ; j++) {       
//       alpha = 255 * vweight_matrix[i][j] / total_weight;
      if (showv) {
        alpha = convertToScale(vweight_matrix[i][j], 0, WW/10, 0, 255);
        alpha = alpha>255?255:alpha;
        SDL_SetAlpha( surface_vertex_weight , SDL_SRCALPHA, approx(alpha) );
        apply_surface( i*CELL_LENGTH, j*CELL_LENGTH , surface_vertex_weight, output );
      }
      
      if (showe) {
        alpha = convertToScale(eweight_matrix[i][j], 0, WW/10, 0, 255);
        alpha = alpha>255?255:alpha;
        SDL_SetAlpha( surface_edge_weight , SDL_SRCALPHA, approx(alpha) );
        apply_surface( i*CELL_LENGTH, j*CELL_LENGTH , surface_edge_weight, output );
      }
    }
  }
  
}

void Avatar::toggleVertex() {
  showv = !showv;
}

void Avatar::toggleEdge() {
  showe = !showe;
}
