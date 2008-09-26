#include "Region.h"
#include "Server.h"
#include "Cell.h"

//===========================================static members

list<Region*> Region::regionList;
bool Region::showw = false;
bool Region::showe = false;
bool Region::showr = false;
int Region::numRegions;
float worldCapacity = 0.0f;

//================================cons/des-truction methods

Region::Region(Uint32 _borderColor) : parentServer(NULL) {
  setBorderColor(_borderColor);
}

Region::~Region() {
}    
    
//============================================other methods

void Region::subscribe(Cell* c) {
  if (hasCell(c)) return;
  cells.push_back(c);
  c->setParentRegion(this);
}

void Region::unsubscribe(Cell* c) {
  if (!hasCell(c)) return;
  cells.remove(c);
  c->releaseCellFromRegion();
}

void Region::unsubscribeAllCells() {
  list<Cell*>::iterator it;
  for (it = cells.begin() ; it != cells.end() ; it++)
    (*it)->releaseCellFromRegion();
  cells.clear();
}

bool Region::setServer(Server* s) {
  if (parentServer || !s) return false;  
  parentServer = s;
  parentServer->assignRegion(this);  
  return true;
}

void Region::unsetServer() {
  if (!parentServer) return;
  Server* s = parentServer;
  parentServer = NULL;
  s->releaseRegion();
}

Server* Region::getServer() {
  return parentServer;
}

list<Cell*> &Region::getCells() {
  return cells;
}

list<Region*> &Region::getNeighbors() {
  return neighbors;
}

int Region::getNumberOfNeighbors() {
  return neighbors.size();
}

Region* Region::getNeighbor(int neighbor) {
  list<Region*>::iterator it = neighbors.begin();
  for (int i = 0 ; i < neighbor ; i++) it++;
  return *it;
}

bool Region::hasCell(Cell* c) {
  for (list<Cell*>::iterator it = cells.begin() ; it != cells.end() ; it++)
    if (*it == c)
      return true;
  return false;
}

void Region::setRegionCapacity(float cap) {
  regionCapacity = cap;
}

float Region::getRegionCapacity(void) {
  return regionCapacity;
}

float Region::getRWeight() {
  list<Cell*>::iterator it;
  float weight = 0.0f;
  for (it = cells.begin() ; it != cells.end() ; it++)
    weight += (*it)->getVWeight() + (*it)->getEWeightToSameRegion();
  return weight;
}

float Region::getEWeight(int neighbor) {//TODO: FIXME
  if (neighbor >= getNumberOfNeighbors())
    return 0.0f;
  list<float>::iterator it = edgeWeight.begin();
  for (int i = 0 ; i < neighbor ; i++) it++;
  return *it;
}

float Region::getAllEdgesWeight() {
  float aew = 0.0f;
  for (list<Cell*>::iterator it = cells.begin() ; it != cells.end() ; it++) {
    aew += (*it)->getEWeightToAnotherRegion();
  }
  return aew;
}

float Region::getAbsoluteLoad() {
  updateAllEdges();
  return getAllEdgesWeight() + getRWeight();
}

float Region::getWorldLoad() {
  float wl = 0.0f;
  for (list<Region*>::iterator it = regionList.begin() ; it != regionList.end() ; it++) {
    wl += (*it)->getAbsoluteLoad();
  }
  return wl;
}

float Region::getEdgeCut() {
  float ec = 0.0f;
  for (list<Region*>::iterator it = regionList.begin() ; it != regionList.end() ; it++) {
    ec += (*it)->getAllEdgesWeight();
  }
  return ec;
}

void Region::updateEWeight(int neighbor) {

  float edgew = 0.0f;
  Region* neighRegion = getNeighbor(neighbor);
  for (list<Cell*>::iterator itrc = cells.begin() ; itrc != cells.end() ; itrc++) {
    list<Cell*> neighCells = (*itrc)->getAllNeighbors();
    for (list<Cell*>::iterator itnc = neighCells.begin() ; itnc != neighCells.end() ; itnc++)
      if (!this->hasCell(*itnc) && neighRegion->hasCell(*itnc))
        edgew += (*itrc)->getEWeight(*itnc);    
  }
  list<float>::iterator it = edgeWeight.begin();
  for (int i = 0 ; i < neighbor ; i++) it++;
  *it = edgew;
}

void Region::updateAllEdges() {
  for (int edge = 0 ; edge < getNumberOfNeighbors() ; edge++)
    updateEWeight(edge);
}

void Region::setBorderColor(Uint32 bc) {
  borderColor = bc;
}

void Region::drawRegion(SDL_Surface* output) {
  for (list<Cell*>::iterator it = cells.begin() ; it != cells.end() ; it++) {        
    (*it)->drawAllBorders(output, borderColor);
  }
}

void Region::drawAllRegions(SDL_Surface* output) {  
  if (showr)
    for (list<Region*>::iterator itr = regionList.begin() ; itr != regionList.end() ; itr++)
      (*itr)->drawRegion(output);
}

void Region::drawEdge(SDL_Surface* output, int neighbor) {
  //TODO
}

void Region::drawAllEdges(SDL_Surface* output) {
  //TODO
}

void Region::drawAllRegionsEdges(SDL_Surface* output) {
  //TODO
}

void Region::drawLoad(SDL_Surface* output, TTF_Font* font) {
  if (cells.empty()) return;
  static SDL_Surface* ldSurf = NULL;
  static SDL_Surface* capSurf = NULL;
  static SDL_Surface* percSurf = NULL;
  SDL_Color txtColor;
  static coord ldPos, capPos, percPos;
  string ldTxt, capTxt, percTxt;
  ldTxt = floatToString(getAbsoluteLoad());  
  ldPos = cells.front()->getAbsolutePosition();
  ldPos.X = ldPos.X + 3;
  ldPos.Y += 3;
  if (ldSurf) SDL_FreeSurface(ldSurf);
  txtColor.r = (Uint8) ((borderColor & 0xFF0000) >> 16);
  txtColor.g = (Uint8) ((borderColor & 0x00FF00) >> 8);
  txtColor.b = (Uint8) (borderColor & 0x0000FF);
  ldSurf = TTF_RenderText_Blended(font, ldTxt.c_str(), txtColor);
  apply_surface(ldPos.X, ldPos.Y, ldSurf, output);
  if (getServer()) {
    capTxt = floatToString(getRegionCapacity());
    percTxt = floatToString(getAbsoluteLoad() / getRegionCapacity());
    if (capSurf) SDL_FreeSurface(capSurf);
    if (percSurf) SDL_FreeSurface(percSurf);
    capSurf = TTF_RenderText_Blended(font, capTxt.c_str(), txtColor);
    percSurf = TTF_RenderText_Blended(font, percTxt.c_str(), txtColor);
    capPos.X = percPos.X = ldPos.X;
    capPos.Y = ldPos.Y + 15;
    percPos.Y = capPos.Y + 15;
    apply_surface(capPos.X, capPos.Y, capSurf, output);
    apply_surface(percPos.X, percPos.Y, percSurf, output);
  }
}

void Region::drawAllRegionsWeights(SDL_Surface* output, TTF_Font* font) {
  if (!showw) return;
  for (list<Region*>::iterator it = regionList.begin() ; it != regionList.end() ; it++) {
    (*it)->drawLoad(output, font);
  }
}

void Region::toggleShowRegions() {
  showr = !showr;
}

void Region::toggleShowEdges() {
  showe = !showe;
}

void Region::toggleShowRegionWeight() {
  showw = !showw;
}

list<Region*> &Region::getRegionList() {
  return regionList;
}

int Region::getNumRegions() {
  return numRegions;
}

void Region::balanceRegions() {
  //TODO - não sempre fazer do zero, mas aproveitar as partições que já existem
  for (list<Region*>::iterator it = regionList.begin() ; it != regionList.end() ; it++) {
    (*it)->getWorldPartition();
  }
}

void Region::getWorldPartitionEXAMPLE() {
  list<Region*>::iterator it;
  int regCode = 0;
  int sX = 0;
  int sY = 0;
  for (it = regionList.begin() ; it != regionList.end() && *it != this ; it++) regCode++;
  cout << "My regCode (Region Code) is " << regCode << endl;
  switch(regCode) {
    case 1 :
      sX += Cell::getRowLength()/2;
      break;
    case 2 :
      sY += Cell::getRowLength()/2;
      break;
    case 3 :
      sX += Cell::getRowLength()/2;
      sY += Cell::getRowLength()/2;
      break;
  }
  Cell* c;
  for (int i = sX ; i < sX + Cell::getRowLength()/2 ; i++)
    for (int j = sY ; j < sY + Cell::getRowLength()/2 ; j++) {
      c = Cell::getCell(i,j);
      subscribe(c);      
    }
}

void Region::getWorldPartitionRandomStart() {
  int cX = rand() % Cell::getRowLength();
  int cY = rand() % Cell::getRowLength();
  Cell* c = Cell::getCell(cX, cY);  
  //TODO fazer com que a verificação do peso total permita que TODAS as células sejam selecionadas por alguma região
  while (c && getRWeight() < Cell::getWorldWeight() / getNumRegions()) { //TODO fazer de forma que não precise fazer subscribe o tempo todo (mas não sei se é realmente um problema)
    subscribe(c);
    c = Cell::getHighestEdgeFreeNeighbor(getCells());
  }
}

void Region::getWorldPartition() {
  Cell* c = Cell::getHeaviestFreeCell();
  //TODO fazer com que a verificação do peso total permita que TODAS as células sejam selecionadas por alguma região
  //while (c && getRWeight() < Cell::getWorldWeight() / getNumRegions()) { //TODO fazer de forma que não precise fazer subscribe o tempo todo (mas não sei se é realmente um problema)
  while (c) { //TODO fazer de forma que não precise fazer subscribe o tempo todo (mas não sei se é realmente um problema)
    if (getServer() && getAbsoluteLoad() > getRegionCapacity()) break;
    subscribe(c);
    c = Cell::getHighestEdgeFreeNeighbor(getCells());
  }
}

void Region::initRegions(int num_reg) {
  if (!regionList.empty()) Region::diposeRegions();
  numRegions = num_reg;
  Uint32 color;
  for (int r = 0 ; r < num_reg ; r++) {
    if (r < NUM_COLORS)
      color = colorTable(r);
    else
      color = rand() % (255*255*255);      
    regionList.push_back(new Region(color));
  }
}

void Region::diposeRegions() {
  list<Region*>::iterator it;
  for (it = regionList.begin() ; it != regionList.end() ; it++) {
    (*it)->unsubscribeAllCells();
    delete *it;
  }
  regionList.clear();
}

void Region::partitionWorld() {
  for (list<Region*>::iterator it = regionList.begin() ; it != regionList.end() ; it++) {
    (*it)->getProportionalPartition();
  }
  if (Cell::getOrphanCell()) {
    distributeOrphanCells();
    //TODO escolher como as celulas órfãs serão dividas: atribuir a célula órfã mais pesada ao servidor com mais recursos livres
  }
}

void Region::distributeOrphanCells() {
  list<Cell*> orphanCells(Cell::getOrphansSortedByTotalWeight());
  list<Cell*>::iterator it_cell = orphanCells.begin();
  while (it_cell != orphanCells.end()) {
    sortRegionsByFreeCapacity();
    (*regionList.begin())->subscribe(*it_cell);
    it_cell++;
  }
}

void Region::sortRegionsByFreeCapacity() {
  regionList.sort(compareRegionsFreeCapacity);
}

bool Region::compareRegionsFreeCapacity(Region* rA, Region* rB) {
  return rA->getRegionCapacity() - rA->getAbsoluteLoad() > rB->getRegionCapacity() - rB->getAbsoluteLoad();
}

void Region::swapCellsRegions(Cell* c1, Cell* c2) {
  Region* r1 = c1->getParentRegion();
  Region* r2 = c2->getParentRegion();
  r1->unsubscribe(c1);
  r2->unsubscribe(c2);  
  r1->subscribe(c2);
  r2->subscribe(c1);    
  r1->updateAllEdges();
  r2->updateAllEdges();
}


bool Region::testCellSwap(Cell* loc, Cell* ext, float& gain) {
  if(!loc->isBorderCell() || !ext->isBorderCell())
    return false;
  Region* other = ext->getParentRegion();
  bool disbalanced = false;
  gain = getEdgeCut();
  swapCellsRegions(loc, ext);
  gain -= getEdgeCut();
  disbalanced = other->getServer()->isDisbalanced() || this->getServer()->isDisbalanced();
  swapCellsRegions(loc, ext);
  return disbalanced;
}

void Region::refinePartitioningGlobal(int passes) {
  int debug = 0;
  int passcount = 0;
  Cell* c1 = NULL;
  Cell* c2 = NULL;
  Cell* _c1;
  Cell* _c2;
  float gain = 0.0f;
  float new_gain = 0.0f;
  cout << endl << "Global partitioning refinement started..." << endl;
  while (!passes || passcount < passes) {
    cout << "Executing pass number " << debug++ << "... ";
    c1 = c2 = NULL;
    passcount++;    
    for (list<Region*>::iterator it_ri = regionList.begin() ; it_ri != regionList.end() ; it_ri++) {
      for (list<Region*>::iterator it_rj = it_ri ; it_rj != regionList.end() ; it_rj++) {                
        if (it_rj == it_ri) it_rj++;
        if (it_rj == regionList.end()) break;
        getBestCellPair(*it_ri, *it_rj, _c1, _c2, &new_gain);
        if (_c1 && _c2 && new_gain > gain) {
          gain = new_gain;
          c1 = _c1;
          c2 = _c2;
        }        
      }
    }
    cout << " executed [OK]" << endl;
    if (c1 && c2) swapCellsRegions(c1, c2);
    else break;
  }
  cout << "Global partitioning refinement concluded." << endl;
}

void Region::refinePartitioningLocal(Region* other, int passes) {
  int passcount = 0;
  Cell* c1;
  Cell* c2;
  while (!passes || passcount < passes) {
    passcount++;
    c1 = c2 = NULL;
    getBestCellPair(this, other, c1, c2);
    if (!c1 || !c2) break;
    swapCellsRegions(c1, c2);
  }
}

void Region::getBestCellPair(Region* r1, Region* r2, Cell*& c1, Cell*& c2, float* gain) {
  list<Cell*> r1_cells(r1->getCells());
  list<Cell*> r2_cells(r2->getCells());
  
  float _new_gain = 0.0f;
  float _gain = 0.0f;
  c1 = c2 = NULL;
  for (list<Cell*>::iterator it1 = r1_cells.begin() ; it1 != r1_cells.end() ; it1++)
    for (list<Cell*>::iterator it2 = r2_cells.begin() ; it2 != r2_cells.end() ; it2++) {
      if (r1->testCellSwap(*it1, *it2, _new_gain) && _new_gain > _gain) {
        c1 = (*it1);
        c2 = (*it2);
        _gain = _new_gain;
      }
    }
  if (gain)
    *gain = _gain;
}
