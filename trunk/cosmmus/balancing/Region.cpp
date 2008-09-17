#include "Region.h"
#include "Cell.h"
#include "myutils.h"

//===========================================static members

list<Region*> Region::regionList;
bool Region::showr;
bool Region::showe;
int Region::numRegions;

//================================cons/des-truction methods

Region::Region(){
  borderColor.R = 128 + rand()%128;
  borderColor.G = 128 + rand()%128;
  borderColor.B = 128 + rand()%128;
  showr = showe = false;
}

Region::~Region() {
}    
    
//============================================other methods

void Region::subscribe(Cell* c) {
  cells.push_back(c);
}

void Region::unsubscribe(Cell* c) {
  cells.remove(c);
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

bool Region::hasCell(Cell* c) {
  for (list<Cell*>::iterator it = cells.begin() ; it != cells.end() ; it++)
    if (*it == c)
      return true;
  return false;
}

float Region::getRWeight() {
  list<Cell*>::iterator it;
  float weight = 0.0f;
  for (it = cells.begin() ; it != cells.end() ; it++)
    weight += (*it)->getVWeight();      
  return weight;
}

float Region::getEWeight(int neighbor) {
  if (neighbor >= getNumberOfNeighbors())
    return 0.0f;
  list<float>::iterator it = edgeWeight.begin();
  for (int i = 0 ; i < neighbor ; i++) it++;
  return *it;
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

Region* Region::getNeighbor(int neighbor) {
  list<Region*>::iterator it = neighbors.begin();
  for (int i = 0 ; i < neighbor ; i++) it++;
  return *it;
}

void Region::drawRegion(SDL_Surface* output) {
  for (list<Cell*>::iterator itrc = cells.begin() ; itrc != cells.end() ; itrc++) {
    list<Cell*> neighCells = (*itrc)->getAllNeighbors();
    for (list<Cell*>::iterator itnc = neighCells.begin() ; itnc != neighCells.end() ; itnc++)
      if (!this->hasCell(*itnc))
        (*itrc)->drawCellBorder(output, *itnc, borderColor);
  }
}

void Region::drawAllRegions(SDL_Surface* output) {
  Color bli;
  bli.R = 0Xff;
  bli.G = 0x99;
  bli.B = 0x00;
  if (showr)
//     for (list<Region*>::iterator itr = regionList.begin() ; itr != regionList.end() ; itr++)
//       (*itr)->drawRegion(output);          
    drawLineBresenham(output, 100, 100, 100, 300, bli);
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

void Region::toggleShowRegions() {
  showr = !showr;
}

void Region::toggleShowEdges() {
  showe = !showe;
}

void Region::divideWorld(int num_reg) {
  numRegions = num_reg;
  for (int r = 0 ; r < num_reg ; r++)
    regionList.push_back(new Region());
  balanceRegions(); //TODO talvez o divideWorld nao devesse chamar balanceRegions, já que este tem em vista ajustar um balanceamento que foi feito antes
}

void Region::balanceRegions() {
  //TODO - não sempre fazer do zero, mas aproveitar as partições que já existem
  for (list<Region*>::iterator it = regionList.begin() ; it != regionList.end() ; it++) {
    (*it)->getWorldPartition();
  }
}

void Region::getWorldPartition() {
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
  for (int i = sX ; i < sX + Cell::getRowLength()/2 ; i++)
    for (int j = sY ; j < sY + Cell::getRowLength()/2 ; j++)
      subscribe(Cell::getCell(i,j));
}
