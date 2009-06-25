#ifdef _WIN32
	#include "../../myutils.h"
#else
	#include "myutils.h"
#endif

#include "Avatar.h"
#include "kdtree.h"

SDL_sem* vsem = NULL;
SDL_sem* esem = NULL;
    
int Avatar::m_numofhotspots = 0;
vector<coord> Avatar::mv_hotspotlist;
SDL_Surface* Avatar::surface_vertex_weight;
SDL_Surface* Avatar::surface_edge_weight;
bool Avatar::showv = false;
bool Avatar::showe = false;
bool Avatar::isMobile = true;
long Avatar::total_weight = 0;
long Avatar::migration_walk = 0;
long Avatar::migration_still = 0;

Avatar::Avatar() {
  init();
  isDrawable = false;
  player_id = 555;
}    

void Avatar::setDrawable(string my_surface_file, string seen_surface_file, SDL_Surface* out_screen) {     
  my_surface = load_image(my_surface_file);
  seen_surface = load_image(seen_surface_file);
  isDrawable = true;
  screen = out_screen;
}

Avatar::~Avatar() {}

void Avatar::init() {          
  do {
    posx = (float)(rand() % WW);
    posy = (float)(rand() % WW);
    dirx = rand() % WW;
    diry = rand() % WW;
    destx = rand() % WW;
    desty = rand() % WW;
		parentNode = NULL;
  } while (distance(posx, posy, destx, desty) <= 20.0f);      
  R = rand() % 255;
  G = rand() % 255;
  B = rand() % 255;      
  if (m_numofhotspots) return;  
  m_numofhotspots = 3;// + rand() % 7;//TODO mudar para ser aleatorio novamente
//  cout << "Number of hot spots: " << m_numofhotspots << endl;
/*  for (int index = 0 ; index < m_numofhotspots ; index++) {
    coord newspot;
    newspot.X = rand() % WW;
    newspot.Y = rand() % WW;
    mv_hotspotlist.push_back(newspot);        
  }     */ 
// X[0] = 749 ; Y[0] = 266
// X[1] = 308 ; Y[1] = 19
// X[2] = 440 ; Y[2] = 401
  coord newspot;
  newspot.X = 740; newspot.Y = 270; mv_hotspotlist.push_back(newspot);
  newspot.X = 310; newspot.Y = 20; mv_hotspotlist.push_back(newspot);
  newspot.X = 440; newspot.Y = 400; mv_hotspotlist.push_back(newspot);

  for (int index = 0 ; index < m_numofhotspots ; index++) {
    cout << "X[" << index << "] = " << mv_hotspotlist[index].X << " ; Y[" << index << "] = " << mv_hotspotlist[index].Y << endl;
  }  
  last_move = 0;
  isSeen = false;      
  //surface_vertex_weight = load_image("vweight.bmp");
  //surface_edge_weight = load_image("eweight.bmp");
  vsem = SDL_CreateSemaphore(1);
  vsem = SDL_CreateSemaphore(1);
}



void Avatar::step(unsigned long delay) { // delay in microseconds  
  Uint32 elapsed_time = SDL_GetTicks() - last_move;
  if (!isMobile) return;

  float distance_ = distance(posx, posy, destx, desty);  
  if (distance_ > 20) {    
    int speed = rand() % 5 + 1;   
    incr_y =  speed * (desty - posy) / distance(posx, posy, destx, desty); // sin of the direction angle
    incr_x =  speed * (destx - posx) / distance(posx, posy, destx, desty); // cossin of the direction angle               
    incr_y *= (float)delay/250.0f;
    incr_x *= (float)delay/250.0f;
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
    if (rand() % 100 < 30) { //selects a random spot
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
  checkMigration();
}

void Avatar::checkMigration() {
	int xmin, xmax, ymin, ymax;
	parentNode->getLimits(xmin, xmax, ymin, ymax);
	if (posx < xmin ||posx >= xmax || posy < ymin || posy >= ymax) {
		parentNode->removeAvatar(this);
		KDTree::getRoot()->insertAvatar(this);
	}
	
/**  coord cell_coord = getCell();
  Cell* new_cell = Cell::getCell(cell_coord.X, cell_coord.Y);  
  Region* new_region = new_cell->getParentRegion();
  if (new_region != old_region) {
    if (new_cell   !=   old_cell)
      migration_walk++; 
    else
      migration_still++;    
  }
  old_cell = new_cell;
  old_region = new_region;
*/
}

long Avatar::getMigrationStill(bool clear_migs) {
  long migs = migration_still;
  if (clear_migs) migration_still = 0;
  return migs;
}

long Avatar::getMigrationWalk(bool clear_migw) {
  long migw = migration_walk;
  if (clear_migw) migration_walk = 0;
  return migw;
}

void Avatar::setPlayerId (int i) {
  player_id = i;
}

void Avatar::setParentNode (KDTree *_parent) {
	parentNode = _parent;
}

void Avatar::markAsSeen(int relevance_) {
  relevance = relevance_;
  isSeen = true;      
}

long Avatar::getWeight() {
  /***/
  return 1;
  /***/
  
  long wgt = 0;
	list<Avatar*> companions = parentNode->getAvList();
	for (list<Avatar*>::iterator it = companions.begin() ; it != companions.end() ; it++) {
		wgt += OtherRelevance(*it);
	}
	return wgt;
}

int Avatar::OtherRelevance(Avatar* other) {//valor entre 0 e 100, ao invés de 0.0f e 1.0f
  float ox = other->GetX();
  float oy = other->GetY();                        
  float dist = distance (posx, posy, ox, oy);                        
  if (dist < THRESHOLD_DISTANCE) {
    return 100;
  }
  if (dist < VIEW_DISTANCE && belongsToVisibility(posx, posy, incr_x, incr_y, ox, oy)) {
    return approx(convertToScale(dist, THRESHOLD_DISTANCE, VIEW_DISTANCE, 0, 100));
  }                        
  return 0;
}

float Avatar::GetX() { 
  return posx; 
}

float Avatar::GetY() { 
  return posy; 
}

void* Avatar::setImage(string filename) {
  my_surface = load_image(filename);
  return (void*) my_surface;
}

void Avatar::draw() {
  if (isSeen) {
    //SDL_SetAlpha( seen_surface, SDL_SRCALPHA, approx(2.55f*relevance) );
    apply_surface( approx(posx), approx(posy), my_surface, screen );
    apply_surface( approx(posx), approx(posy), seen_surface, screen );
  }
  else {
    //SDL_SetAlpha( my_surface, SDL_SRCALPHA, 255 );
    apply_surface( approx(posx), approx(posy), my_surface, screen );
  }
  isSeen = false;
}

coord Avatar::getCell () {
/**
  coord cell;      
  cell.X = int (simpleScale(posx, WW, CELLS_ON_A_ROW));
  cell.Y = int (simpleScale(posy, WW, CELLS_ON_A_ROW));    
  return cell;
*/
}

/**long Avatar::getInteraction(Cell* _cell) { //mudar pra usar cada celula diferente: getWeightE(UP_LEFT), por exemplo.
  list<Avatar*>::iterator it;
  long _interaction = 0;
  for (it = _cell->getAvatars().begin() ; it != _cell->getAvatars().end() ; it++)
    _interaction += this->OtherRelevance(*it);
  return _interaction;
}*/

/**void Avatar::drawCells (SDL_Surface* output) {  
  coord cell;
  float alpha;  
  for (int i = 0 ; i < Cell::getRowLength() ; i++) {
    for (int j = 0 ; j < Cell::getRowLength() ; j++) {       
      if (showv) {        
        alpha = alpha>255?255:alpha;
        SDL_SetAlpha( surface_vertex_weight , SDL_SRCALPHA, approx(alpha) );
        apply_surface( i*CELL_LENGTH, j*CELL_LENGTH , surface_vertex_weight, output );
      }      
      if (showe) {        
        alpha = alpha>255?255:alpha;
        SDL_SetAlpha( surface_edge_weight , SDL_SRCALPHA, approx(alpha) );
        apply_surface( i*CELL_LENGTH, j*CELL_LENGTH , surface_edge_weight, output );
      }
    }
  }  
}*/

bool Avatar::toggleVertex() {
  showv = !showv;
  return showv;
}

bool Avatar::toggleEdge() {
  showe = !showe;
  return showe;
}

bool Avatar::toggleMobility() {
  isMobile = !isMobile;
  return isMobile;
}

bool Avatar::setMobility(bool value) {
  bool oldstate = isMobile;
  isMobile = value;
  return oldstate;
}

bool Avatar::compareX(Avatar* a, Avatar* b) {
	return a->GetX() < b->GetX();
}

bool Avatar::compareY(Avatar* a, Avatar* b) {
	return a->GetY() < b->GetY();
}
