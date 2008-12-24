#include "Region.h"
#include "Server.h"
#include "Cell.h"
#include "Avatar.h"

#define MIN_BAL_IMPROVEMENT 0.0f

//===========================================static members

list<Region*> Region::regionList;
bool Region::showw = false;
bool Region::showe = false;
bool Region::showr = false;
int Region::numRegions;
long worldCapacity = 0;

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

void Region::checkNeighborsList() {
  neighbors.clear();
  list<Cell*>::iterator it_cell, it_neighbor_cell;
  list<Cell*> neigh_cells;
  for (it_cell = cells.begin() ; it_cell != cells.end() ; it_cell++) {    
    neigh_cells = (*it_cell)->getAllNeighbors();
    for (it_neighbor_cell = neigh_cells.begin() ; it_neighbor_cell != neigh_cells.end() ; it_neighbor_cell++) {
      if (!hasCell(*it_neighbor_cell) && (*it_neighbor_cell)->getParentRegion() && !hasNeighbor((*it_neighbor_cell)->getParentRegion())) {
        neighbors.push_back((*it_neighbor_cell)->getParentRegion());
      }
    }
  }
}

void Region::checkAllRegionsNeighbors() {
  for (list<Region*>::iterator it = regionList.begin() ; it != regionList.end() ; it++)
    (*it)->checkNeighborsList();
  updateAllEdgesAllRegions();
}

list<Region*> &Region::getNeighbors() {
  return neighbors;
}

int Region::getNumberOfNeighbors() {
  return neighbors.size();
}

bool Region::hasNeighbor(Region* r) {
  for (list<Region*>::iterator it = neighbors.begin() ; it != neighbors.end() ; it++) {
    if ((*it) == r) return true;
  }
  return false;
}

bool Region::hasCell(Cell* c) {
  for (list<Cell*>::iterator it = cells.begin() ; it != cells.end() ; it++)
    if (*it == c)
      return true;
  return false;
}

Cell* Region::getHeaviestCell() {
  if (cells.empty()) return NULL;
  list<Cell*> celllist = cells;
  Cell::sortByWeight(celllist);
  return celllist.front();
}

void Region::setRegionCapacity(long cap) {
  regionCapacity = cap;
}

long Region::getRegionCapacity(void) {
  //return regionCapacity;
  return getServer()->getServerPower();
}

long Region::getRWeight() {
  list<Cell*>::iterator it;
  long weight = 0;
  for (it = cells.begin() ; it != cells.end() ; it++)
    weight += (*it)->getVWeight() + (*it)->getEWeightToSameRegion();
  return weight;
}

long Region::getRegionWeight() {
  list<Cell*>::iterator it;
  long weight = 0;
  for (it = cells.begin() ; it != cells.end() ; it++)
    weight += (*it)->getCellWeight();
  return weight;
}

long Region::getEWeight(Region* neighbor) {//TODO: FIXME
  return edgeByRegion[neighbor];
}

long Region::getAllEdgesWeight() {
  long aew = 0;
  for (list<Cell*>::iterator it = cells.begin() ; it != cells.end() ; it++) {
    aew += (*it)->getEWeightToAnotherRegion();
  }
  return aew;
}

double Region::getWeightFraction() {
  //return getAbsoluteLoad() / Cell::getWorldWeight();
  return (double)getRegionWeight() / (double)Cell::getWorldWeight();
}

double Region::getLoadFraction() {
  return (double)getAbsoluteLoad() / (double)Cell::getWorldWeight();
  //return getRegionWeight() / Cell::getWorldWeight();
}

long Region::getAbsoluteLoad() {
  checkNeighborsList();
  updateAllEdges();
  //TODO: corrigir isso (tirar alledgesweight da conta, MAS TEM QUE LEMBRAR Q O SERVER VAI FAZER UPLOAD PRO OUTRO SERVER!!!)
  return getAllEdgesWeight() + getRegionWeight();
}

long Region::getWorldLoad() {
  long wl = 0;
  for (list<Region*>::iterator it = regionList.begin() ; it != regionList.end() ; it++) {
    wl += (*it)->getAbsoluteLoad();
  }
  return wl;
}

long Region::getEdgeCut() {
  long ec = 0;
  for (list<Region*>::iterator it = regionList.begin() ; it != regionList.end() ; it++) {
    ec += (*it)->getAllEdgesWeight();
  }
  return ec;
}

double Region::getRegionOverload() {
  return (double)getRegionWeight() / (double)getServer()->getServerPower();
}

void Region::updateEWeight(Region* neighRegion) {
  long edgew = 0;
  for (list<Cell*>::iterator itrc = cells.begin() ; itrc != cells.end() ; itrc++) {
    edgew += (*itrc)->getEWeightToRegion(neighRegion);
    //list<Cell*> neighCells = (*itrc)->getAllNeighbors();
    //for (list<Cell*>::iterator itnc = neighCells.begin() ; itnc != neighCells.end() ; itnc++)
    //  if (!this->hasCell(*itnc) && neighRegion->hasCell(*itnc))
    //    edgew += (*itrc)->getEWeight(*itnc);
  }
  edgeByRegion[neighRegion] = edgew;
}

void Region::updateAllEdges() {
  edgeByRegion.clear();
  for (list<Region*>::iterator it = neighbors.begin() ; it != neighbors.end() ; it++)
    updateEWeight(*it);
}

void Region::updateAllEdgesAllRegions() {
  list<Region*>::iterator it;
  for (it = regionList.begin() ; it != regionList.end() ; it++) {
    (*it)->updateAllEdges();
  }
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

void Region::drawEdge(SDL_Surface* output, Region* neighbor) {
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
  ldTxt = longToString(getRegionWeight());
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
    capTxt = longToString(/*getRegionCapacity()*/getServer()->getServerPower());
    percTxt = floatToString((float)getRegionWeight() / (float)getServer()->getServerPower());
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
    //if (getServer() && getAbsoluteLoad() > getRegionCapacity()) break;
    if (getServer() && getLoadFraction() > getServer()->getPowerFraction()) break;
    subscribe(c);
    c = Cell::getHighestEdgeFreeNeighbor(getCells());
  }
}

void Region::initRegions(int num_reg) {
  if (!regionList.empty()) Region::disposeRegions();
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

void Region::disposeRegions() {
  list<Region*>::iterator it;
  for (it = regionList.begin() ; it != regionList.end() ; it++) {
    (*it)->unsubscribeAllCells();
    delete *it;
  }
  regionList.clear();
}

void Region::partitionWorld() {
  sortRegionsByServerPower();
  for (list<Region*>::iterator it = regionList.begin() ; it != regionList.end() ; it++) {
    (*it)->getProportionalPartition(Cell::getWorldWeight());
  }
  if (Cell::getOrphanCell()) {
    //distributeOrphanCells();
    //TODO: escolher como as celulas órfãs serão dividas: atribuir a célula órfã mais pesada ao servidor com mais recursos livres
  }
  checkAllRegionsNeighbors();
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

void Region::getProportionalPartition(long weight_to_divide) {
  long weightFraction = approxLong((double)weight_to_divide * getServer()->getPowerFraction());
  long _debug_region_weight;
  Cell* c = NULL;
  
  if(getRegionWeight() > 0 && Cell::getHighestEdgeFreeNeighbor(getCells())) {
    c = Cell::getHighestEdgeFreeNeighbor(getCells());
  } else {
    c = Cell::getHeaviestFreeCell();
  }

  //TODO fazer com que a verificação do peso total permita que TODAS as células sejam selecionadas por alguma região
  //while (c && getRWeight() < Cell::getWorldWeight() / getNumRegions()) { //TODO fazer de forma que não precise fazer subscribe o tempo todo (mas não sei se é realmente um problema)
  while (getRegionWeight() < weightFraction /*|| getRegionOverload() < 1.0f*/) { //TODO fazer de forma que não precise fazer subscribe o tempo todo (mas não sei se é realmente um problema)
    //if (getServer() && getAbsoluteLoad() > getRegionCapacity()) break;
    _debug_region_weight = getRegionWeight();
    if (!c) return;    
    subscribe(c);
    c = Cell::getHighestEdgeFreeNeighbor(getCells());
    if (!c) c = Cell::getHeaviestFreeCell();
    //TODO: corrigir para usar o load, porém corrigido (LEVAR EM CONTA QUE O SERVER FAZ UPLOAD PROS OUTROS SERVERS)
  }
}

void Region::sortRegionsByFreeCapacity() {
  regionList.sort(compareRegionsFreeCapacity);
}

bool Region::compareRegionsFreeCapacity(Region* rA, Region* rB) {
  return rA->getRegionCapacity() - rA->getAbsoluteLoad() > rB->getRegionCapacity() - rB->getAbsoluteLoad();
}

void Region::swapCellsRegions(Cell* c1, Cell* c2, bool fast) {
  Region* r1 = c1->getParentRegion();
  Region* r2 = c2->getParentRegion();
  r1->unsubscribe(c1);
  r2->unsubscribe(c2);  
  r1->subscribe(c2);
  r2->subscribe(c1);
  if (fast) return;
  Region::checkAllRegionsNeighbors();
  cout << "check neigh list OK" << endl;
  r1->updateAllEdges();
  r2->updateAllEdges();
}


bool Region::testCellSwap(Cell* loc, Cell* ext, long& gain) {
  if(!loc->isBorderCell() || !ext->isBorderCell())
    return false;
  Region* other = ext->getParentRegion();
  bool stillBalanced = true;
  gain = getEdgeCut();
  swapCellsRegions(loc, ext, FAST_SWAP);
  gain -= getEdgeCut();
  stillBalanced = !other->getServer()->isDisbalanced() && !this->getServer()->isDisbalanced();
  swapCellsRegions(loc, ext, FAST_SWAP);
  return stillBalanced;
}

void Region::refinePartitioningGlobal(int passes) {
  bool mobile_state = Avatar::setMobility(false);
  int debug = -1;
  int passcount = 0;
  Cell* c1 = NULL;
  Cell* c2 = NULL;
  Cell* _c1;
  Cell* _c2;
  long gain = 0;
  long new_gain = 0;
  cout << endl << "Global partitioning refinement started..." << endl;
  while (!passes || passcount < passes) {
    cout << "\nExecuting pass number " << ++debug << "..." << endl;
    c1 = c2 = NULL;
    passcount++;
    for (list<Region*>::iterator it_ri = regionList.begin() ; it_ri != regionList.end() ; it_ri++) {
      for (list<Region*>::iterator it_rj = regionList.begin() ; it_rj != regionList.end() ; it_rj++) {                
        if (it_rj == it_ri) continue;
        getBestCellPair(*it_ri, *it_rj, _c1, _c2, &new_gain);
        if (_c1 && _c2 && new_gain > gain) {
          cout << "\tNew best pair found. Gain = " << new_gain << endl;
          gain = new_gain;
          c1 = _c1;
          c2 = _c2;
        }        
      }
    }
    cout << "\tPass " << debug << " executed [OK]" << endl;
    if (c1 && c2) swapCellsRegions(c1, c2);
    else break;
  }
  checkAllRegionsNeighbors();
  Avatar::setMobility(mobile_state);
  cout << "Global partitioning refinement concluded." << endl;
}

void Region::refinePartitioningLocal(Region* other, int passes) {
  bool mobile_state = Avatar::setMobility(false);
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
  Avatar::setMobility(mobile_state);
}

bool Region::refineKL_kwise(list<Region*> &regionsToRefine, int passes) {
  bool mobile_state = Avatar::setMobility(false);
  list<Region*>::iterator it_r1, it_r2;
  bool swapped = true;
  for (int i = 0 ; i < passes && swapped; i++) {
    swapped = false;
    for (it_r1 = regionsToRefine.begin() ; it_r1 != regionsToRefine.end() ; it_r1++) {
      for (it_r2 = regionsToRefine.begin() ; it_r2 != regionsToRefine.end() ; it_r2++) {
        if (*it_r1 == *it_r2) continue;
        if (refineKL_pairwise(*it_r1, *it_r2))
          swapped = true;
      }
    }
  }
  Avatar::setMobility(mobile_state);
  return swapped;
}

// KL - SIMPLIFICADO:
//
//enquanto (quiser repetir) {
//  listc1;
//  listc2;
//  ordenar listas;
//  enquanto (listas nao vazias) {
//    para cada c1 {
//      para cada c2 {
//        ver ganho e ver se vale a pena trocar
//      }
//    }
//    trocar, se houver ganho;
//    remove c1 e c2 das listas;
//  }
//}

bool Region::refineKL_pairwise(Region* r1, Region* r2) {
  bool mobile_state = Avatar::setMobility(false);
  list<Cell*> cell_list_1, cell_list_2;
  list<Cell*>::iterator it_c1, it_c2;
  long max_gain = 0;
  long new_gain = 0;
  Cell* max_c1 = NULL;
  Cell* max_c2 = NULL;
  bool swapped_in_this_pass = false;
  bool swapped = false;
  bool swapping = true;

  
  do { //início do kl pairwise
    cell_list_1 = r1->getCells();
    cell_list_2 = r2->getCells();
    swapped_in_this_pass = false;
    while (!cell_list_1.empty() && !cell_list_2.empty()) { //início de um passe
      Cell::sortByDesireToSwap(cell_list_1, r2);
      Cell::sortByDesireToSwap(cell_list_2, r1);
      swapping = false;
      max_gain = 0;
      for (it_c1 = cell_list_1.begin() ; it_c1 != cell_list_1.end() ; it_c1++) {
        cout << "C1_desire = " << (*it_c1)->getDesireToSwap(r2) << endl;
        for (it_c2 = cell_list_2.begin() ; it_c2 != cell_list_2.end() ; it_c2++) {
          cout << "C2_desire = " << (*it_c2)->getDesireToSwap(r1) << endl;
          new_gain = Cell::getSwapGain(*it_c1, *it_c2);
          if (new_gain > max_gain  && getBalancingImprovement(*it_c1, *it_c2) >= MIN_BAL_IMPROVEMENT) { //TODO: verificar esse método de pegar o balchange
            max_gain = new_gain;
            max_c1 = *it_c1;
            max_c2 = *it_c2;
            swapping = true;
          }
          else if ((*it_c1)->getDesireToSwap(r2) + (*it_c2)->getDesireToSwap(r1) < max_gain) {
            break;
          }
        } //end for
        if (it_c2 != cell_list_2.end() && (*it_c1)->getDesireToSwap(r2) + (*it_c2)->getDesireToSwap(r1) < max_gain) {
          break;
        }
      } //end for
      if (swapping) {
        swapCellsRegions(max_c1, max_c2);
        cell_list_1.remove(max_c1);
        cell_list_2.remove(max_c2);
        swapped_in_this_pass = true;
        swapped = true;
      }
      else break; //não conseguiu achar mais pares. encerre este passe.
    } //final do passe
  } while (swapped_in_this_pass);
  Avatar::setMobility(mobile_state);
  return swapped;
}

double Region::getBalancingImprovement(Cell* c1, Cell* c2) {
  double ideal_overload = (double)Cell::getWorldWeight() / (double)Server::getMultiserverPower();
  Region* r1 = c1->getParentRegion();
  Region* r2 = c2->getParentRegion();
  double overload_r1_before = (double)r1->getRegionWeight() / (double)r1->getServer()->getServerPower();
  double overload_r2_before = (double)r2->getRegionWeight() / (double)r2->getServer()->getServerPower();
  double overload_r1_after = (double)(r1->getRegionWeight() - c1->getCellWeight() + c2->getCellWeight()) / (double)r1->getServer()->getServerPower();
  double overload_r2_after = (double)(r2->getRegionWeight() - c2->getCellWeight() + c1->getCellWeight()) / (double)r2->getServer()->getServerPower();

  double bal_change_r1 = abs(ideal_overload - overload_r1_before) - abs(ideal_overload - overload_r1_after);
  double bal_change_r2 = abs(ideal_overload - overload_r2_before) - abs(ideal_overload - overload_r2_after);
  /*double power_fraction1 = c1->getParentRegion()->getServer()->getPowerFraction();
  double power_fraction2 = c2->getParentRegion()->getServer()->getPowerFraction();
  double weight_fraction_r1_before = (double)c1->getParentRegion()->getRegionWeight() / (double)Cell::getWorldWeight();
  double weight_fraction_r2_before = (double)c2->getParentRegion()->getRegionWeight() / (double)Cell::getWorldWeight();
  double weight_fraction_r1_after = ((double)c1->getParentRegion()->getRegionWeight() - (double)c1->getCellWeight() + (double)c2->getCellWeight()) / (double)Cell::getWorldWeight();
  double weight_fraction_r2_after = ((double)c2->getParentRegion()->getRegionWeight() - (double)c2->getCellWeight() + (double)c1->getCellWeight()) / (double)Cell::getWorldWeight();
  double bal_change_r1 = abs(power_fraction1 - weight_fraction_r1_before) - abs(power_fraction1 - weight_fraction_r1_after);
  double bal_change_r2 = abs(power_fraction2 - weight_fraction_r2_before) - abs(power_fraction2 - weight_fraction_r2_after);*/
  return bal_change_r1 + bal_change_r2;
}

void Region::improveBalancing_kwise(list<Region*> &regionsToImproveBalancing, int passes) {
  bool mobile_state = Avatar::setMobility(false);  
  Region* rmaxload;

  list<Region*> balregions = regionsToImproveBalancing;

  while (balregions.size() > 1) {
    sortByOverload(balregions);
    rmaxload = balregions.front();
    rmaxload->alleviateOverload(regionsToImproveBalancing);
    balregions.pop_front();
  }

  Avatar::setMobility(mobile_state);
}

Region* Region::improveBalancing_pairwise(Region* rmaxload, Region* rminload) { //retorna a região curada/saturada
  bool mobile_state = Avatar::setMobility(false);
  list<Cell*> cell_list;

  long ideal_weight_rmax = approxLong(rmaxload->getServer()->getPowerFraction() * (double)Cell::getWorldWeight());
  long ideal_weight_rmin = approxLong(rminload->getServer()->getPowerFraction() * (double)Cell::getWorldWeight());
  
  while (rmaxload->getRegionWeight() > ideal_weight_rmax && rminload->getRegionWeight() < ideal_weight_rmin) {
    cell_list = rmaxload->getCells();
    Cell::sortByWeight(cell_list);
    rmaxload->unsubscribe(cell_list.back()); //tira a célula menos pesada
    rminload->subscribe(cell_list.back());   //entrega-a pro servidor menos sobrecarregado
  }

  Avatar::setMobility(mobile_state);

  if (rmaxload->getRegionWeight() <= ideal_weight_rmax)
    return rmaxload;
  if (rminload->getRegionWeight() >= ideal_weight_rmin)
    return rminload;
}

void Region::alleviateOverload(list<Region*> &regionsToImproveBalancing) {
  list<Region*>::iterator it;
  list<Cell*> regcells;
  double maximprovement;
  Region* receiving_reg;
  long ideal_weight = approxLong(this->getServer()->getPowerFraction() * (double)Cell::getWorldWeight());
  regcells = this->getCells();
  Cell::sortByWeight(regcells);

  while (this->getRegionWeight() > ideal_weight) {
    receiving_reg = NULL;
    maximprovement = 0.0f;
    for ( it = regionsToImproveBalancing.begin() ; it != regionsToImproveBalancing.end() ; it++ ) {
      if (*it == this) continue;
      if (getBalancingImprovementForTransfering(regcells.back(), *it) > maximprovement) {
        maximprovement = getBalancingImprovementForTransfering(regcells.back(), *it);
        receiving_reg = *it;
      }      
    }
    if (receiving_reg) {
      this->unsubscribe(regcells.back());
      receiving_reg->subscribe(regcells.back());
      regcells.pop_back();
    }
    else break; //qualquer outra mudança de células só vai prejudicar o balanceamento
  }
}

void Region::sortByOverload(list<Region*> &regionList) {
  regionList.sort(Region::compareRegionsOverload);
}

bool Region::compareRegionsOverload(Region* rA, Region* rB) {
  return (rA->getRegionWeight() / rA->getServer()->getServerPower() > rB->getRegionWeight() / rB->getServer()->getServerPower());
}

double Region::getDisbalanceAfterAddingCell(Cell* c) {
  double resulting_overload = (double)(getRegionWeight() + c->getCellWeight()) / (double)Cell::getWorldWeight();
  double ideal_overload = getServer()->getPowerFraction();
  return resulting_overload - ideal_overload;
}

double Region::getDisbalanceAfterRemovingCell(Cell* c) {
  double resulting_overload = (double)(getRegionWeight() - c->getCellWeight()) / (double)Cell::getWorldWeight();
  double ideal_overload = getServer()->getPowerFraction();
  return resulting_overload - ideal_overload;
}

double Region::getBalancingImprovementForTransfering(Cell* c, Region* r) {
  double ideal_overload = (double)Cell::getWorldWeight() / (double)Server::getMultiserverPower();
  double overload_r1_before = (double)this->getRegionWeight() / (double)this->getServer()->getServerPower();
  double overload_r2_before = (double)r->getRegionWeight() / (double)r->getServer()->getServerPower();
  double overload_r1_after = (double)(this->getRegionWeight() - c->getCellWeight()) / (double)this->getServer()->getServerPower();
  double overload_r2_after = (double)(r->getRegionWeight() + c->getCellWeight()) / (double)r->getServer()->getServerPower();

  double bal_change_r1 = abs(ideal_overload - overload_r1_before) - abs(ideal_overload - overload_r1_after);
  double bal_change_r2 = abs(ideal_overload - overload_r2_before) - abs(ideal_overload - overload_r2_after);

  //double power_fraction1 = this->getServer()->getPowerFraction(); //1 = this; 2 = r
  //double power_fraction2 = r->getServer()->getPowerFraction();
  //double weight_fraction_r1_before = (double)this->getRegionWeight() / (double)Cell::getWorldWeight();
  //double weight_fraction_r2_before = (double)r->getRegionWeight() / (double)Cell::getWorldWeight();
  //double weight_fraction_r1_after = ((double)getRegionWeight() - (double)c->getCellWeight()) / (double)Cell::getWorldWeight();
  //double weight_fraction_r2_after = ((double)r->getRegionWeight() + (double)c->getCellWeight()) / (double)Cell::getWorldWeight();
  //double bal_change_r1 = abs(power_fraction1 - weight_fraction_r1_before) - abs(power_fraction1 - weight_fraction_r1_after);
  //double bal_change_r2 = abs(power_fraction2 - weight_fraction_r2_before) - abs(power_fraction2 - weight_fraction_r2_after);
  return bal_change_r1 + bal_change_r2;
}

void Region::improveBalancing_v2(list<Region*> &regionsToImproveBalancing) {
  Region* rmaxload;
  list<Region*> balregions = regionsToImproveBalancing;
  sortByOverload(balregions);
  for (list<Region*>::iterator it = balregions.begin() ; it != balregions.end() ; it++) {
    (*it)->alleviateOverload_v2(balregions);
  }
}

void Region::alleviateOverload_v2(list<Region*> &regionsToReceive) {
  list<Region*> balregions = regionsToReceive;  
  Region* recv_reg;
  long weight_to_lose = getRegionWeight() - approxLong((double)Cell::getWorldWeight() * getServer()->getPowerFraction());
  long recv_server_free_capacity;

  balregions.remove(this);
  while (!balregions.empty()) {
    recv_reg = balregions.back();
    recv_server_free_capacity = approxLong(recv_reg->getServer()->getPowerFraction()*(double)Cell::getWorldWeight()) - recv_reg->getRegionWeight();
    weight_to_lose = getRegionWeight() - approxLong((double)Cell::getWorldWeight() * getServer()->getPowerFraction());
    long weight_to_this_reg = minimum(weight_to_lose, recv_server_free_capacity);
    while (weight_to_this_reg > 0) {
      Cell* c = getCellWithWeightLowerThanButClosestTo(weight_to_this_reg);
      if (c) {
        unsubscribe(c);
        recv_reg->subscribe(c);
        weight_to_this_reg -= c->getCellWeight();
      }
      else break; // não achou célula que coubesse nesse outro servidor
    }
    balregions.pop_back();
  }
}

Cell* Region::getCellWithWeightLowerThanButClosestTo(long weight) {
  list<Cell*> cell_list = this->getCells();
  Cell::sortByWeight(cell_list);
  for (list<Cell*>::iterator it = cell_list.begin() ; it != cell_list.end() ; it++) {
    if ((*it)->getCellWeight() <= weight) return (*it);
  }
  return NULL;
}

void Region::improveBalancing_v3(list<Region*> regionsToImproveBalancing) {
  long weight_to_divide = 0;
  for (list<Region*>::iterator it = regionsToImproveBalancing.begin() ; it != regionsToImproveBalancing.end() ; it++) {
    weight_to_divide += (*it)->getRegionWeight();
    Cell* c = (*it)->getHeaviestCell(); //pegar a célula mais pesada da região -> c
    (*it)->unsubscribeAllCells(); //limpar a região
    if (c) {
      (*it)->subscribe(c); //atribuir c de volta à região
    }
  }
  for (list<Region*>::iterator it = regionsToImproveBalancing.begin() ; it != regionsToImproveBalancing.end() ; it++) {
    (*it)->getProportionalPartition(weight_to_divide); //fazer novament o ggp na região, partindo de cada c.
  }
}

void Region::improveBalancing_v4(list<Region*> regionsToImproveBalancing) {
  //LIBERANDO O PESO DE SERVIDORES SOBRECARREGADOS
  long weight_to_divide = 0;
  Region::sortByOverload(regionsToImproveBalancing);
  for (list<Region*>::iterator it = regionsToImproveBalancing.begin() ; it != regionsToImproveBalancing.end() ; it++) {
    weight_to_divide += (*it)->getRegionWeight();
    list<Cell*> celllist = (*it)->getCells();
    Cell::sortByWeight(celllist);
    while (!celllist.empty() && (*it)->getWeightFraction() > (*it)->getServer()->getPowerFraction()) {
      (*it)->unsubscribe(celllist.back());
      celllist.pop_back();
    }
  }
  
  //COLETANDO AS CÉLULAS LIVRES
  Region::sortByOverload(regionsToImproveBalancing);
  while (!regionsToImproveBalancing.empty()) {
    regionsToImproveBalancing.back()->getProportionalPartition(weight_to_divide);
    regionsToImproveBalancing.pop_back();
  }
}

void Region::improveBalancing_repart(list<Region*> regionsToImproveBalancing) {
  long weight_to_divide = 0;
  for (list<Region*>::iterator it = regionsToImproveBalancing.begin() ; it != regionsToImproveBalancing.end() ; it++) {
    weight_to_divide += (*it)->getRegionWeight();
    (*it)->unsubscribeAllCells();
  }
  Region::sortRegionsByServerPower(regionsToImproveBalancing);
  for (list<Region*>::iterator it = regionsToImproveBalancing.begin() ; it != regionsToImproveBalancing.end() ; it++) {
    (*it)->getProportionalPartition(weight_to_divide);
  }
}

void Region::startLocalBalancing() {
  list<Region*> local_group;
  local_group.push_back(this);
  long total_weight = this->getRegionWeight();
  long total_capacity = this->getServer()->getServerPower();
  double average_overload = (double)total_weight / (double)total_capacity;
  while (average_overload > 1.0f && average_overload > Cell::getWorldWeight()/Server::getMultiserverPower()) {
    Region* next_region = getLightestNeighbor(local_group);
    if (!next_region) break;
    total_weight += next_region->getRegionWeight();
    total_capacity += next_region->getServer()->getServerPower();
    average_overload = (double)total_weight / (double)total_capacity;
    local_group.push_back(next_region);    
  }
  Region::improveBalancing_repart(local_group);
  //Region::improveBalancing_kwise(local_group);
}

Region* Region::getLightestNeighbor(list<Region*> region_list) {
  list<Region*> all_neighbors;
  for (list<Region*>::iterator it = region_list.begin() ; it != region_list.end() ; it++) {
    (*it)->checkNeighborsList();
    list<Region*> next_neighs = (*it)->getNeighbors();
    all_neighbors.merge(next_neighs);
  }
  for (list<Region*>::iterator it = region_list.begin() ; it != region_list.end() ; it++) {    
    all_neighbors.remove(*it);
  }
  Region::sortByOverload(all_neighbors);
  cout << "Region::getLightestNeighbor {all_neighbors:\n\t";
  for (list<Region*>::iterator it = all_neighbors.begin() ; it != all_neighbors.end() ; it++) {    
    cout << (double)(*it)->getRegionWeight() / (double)(*it)->getServer()->getServerPower() << "\t";
  }
  cout << "\n}" << endl;
  if (all_neighbors.empty()) return NULL;
  return all_neighbors.back(); //retorna o ultimo da lista ordenada por overload, ou seja, o menos overloaded
}

void Region::checkBalancing() {
  if (!getServer()) return;
  double overload = (double)getRegionWeight() / (double)getServer()->getServerPower();
  long ideal_weight = approxLong(getServer()->getPowerFraction() * (double)Cell::getWorldWeight());
  if (overload > DISBAL_TOLERANCE && getRegionWeight() > approxLong((double)ideal_weight * DISBAL_TOLERANCE))
    startLocalBalancing();
}

void Region::getBestCellPair(Region* r1, Region* r2, Cell*& c1, Cell*& c2, long* gain) {
  list<Cell*> r1_cells(r1->getCells());
  list<Cell*> r2_cells(r2->getCells());
  
  long _new_gain = 0;
  long _gain = 0;
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

bool Region::compareServerPower(Region* rA, Region* rB) {
  return (rA->getServer()->getServerPower() > rB->getServer()->getServerPower());
}

void Region::sortRegionsByServerPower() {
  regionList.sort(Region::compareServerPower);
}

void Region::sortRegionsByServerPower(list<Region*> &region_list) {
  region_list.sort(Region::compareServerPower);
}