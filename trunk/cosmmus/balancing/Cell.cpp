#include "Cell.h"
#include "Avatar.h"

//===========================================static members

Cell*** Cell::cellMatrix = NULL;
int Cell::cells_on_a_row = 0;

//================================cons/des-truction methods

Cell::Cell();

Cell::Cell(int coord_x, int coord_y);
    
Cell::~Cell() {
  if (cellMatrix) {
    for (int i = 0 ; i < cells_on_a_row ; i++)
      delete [] cellMatrix[i];
  }
  delete [] cellMatrix;
}    
    
//============================================other methods

void Cell::subscribe(Avatar* av) {
  avatars[av] = av;
}

void Cell::unsubscribe(Avatar* av) {
  avatars.erase(av);
}

list<Avatar*> Cell::getAvatars() {
  list<Avatar*> ret;  
  ret.insert(ret.begin(), avatars.begin(), avatars.end());
  return ret;
}

float Cell::getVWeight() {
  return avatars.size();
}

float Cell::getEWeight(Cell* neighbor) {
  float totalw = 0.0f;
  for (map<Avatar*,Avatar*>::iterator it = avatars.begin() ; it != avatars.end() ; it++) {
    totalw += it->second->getWeightE();
  }
  return totalw;
}

void Cell::allocCellMatrix(int row) {
  if (cellMatrix) {
    for (int i = 0 ; i < cells_on_a_row ; i++)
      delete [] cellMatrix[i];
  }
  delete [] cellMatrix;  
  cells_on_a_row = row;  
  cellMatrix = new Cell** [row];
  for (int i = 0 ; i < row ; i++)
    cellMatrix[i] = new Cell* [row];
}