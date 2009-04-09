#ifdef _WIN32
#include "../../myutils.h"
#else
#include "myutils.h"
#endif

#include "Region.h"
#include "Cell.h"
#include "Avatar.h"

//===========================================static members

Cell*** Cell::cellMatrix = NULL;
Region* Cell::regionUnderComparison = NULL;
int Cell::cells_on_a_row = 0;
long Cell::worldWeight = 0;
bool Cell::showv = false;
bool Cell::showe = false;
SDL_Surface* Cell::surface_vertex_weight = NULL;
SDL_Surface* Cell::surface_edge_weight = NULL;

//================================cons/des-truction methods

Cell::Cell(){
}

Cell::Cell(int coord_x, int coord_y) {
  cellposition = new coord();
  cellposition->X = coord_x;
  cellposition->Y = coord_y;
  for (short neigh = 0 ; neigh < NUM_NEIGH ; neigh++)
    edgeWeight[neigh] = 0;
  totalEdgeWeight = 0;
  vertexWeight = 0;
  parentRegion = NULL;
}
    
Cell::~Cell() {
  delete cellposition;
}    
    
//============================================other methods

void Cell::subscribe(Avatar* av) {
  avatars.push_back(av);
}

void Cell::unsubscribe(Avatar* av) {
  avatars.remove(av);
}

list<Avatar*> &Cell::getAvatars() {
  return avatars;
}

long Cell::getVWeight() {
  return vertexWeight;
}

void Cell::updateVWeight() {
  list<Avatar*>::iterator it1, it2;
  long weight = 0;
  for (it1 = avatars.begin() ; it1 != avatars.end() ; it1++)
    for (it2 = avatars.begin() ; it2 != avatars.end() ; it2++) {
      if (*it1 == *it2) continue;
      weight += (long)(*it1)->OtherRelevance(*it2);    
    }
  vertexWeight = weight;
  //somar com o peso de todas as arestas. (ou deixar assim porque � mais f�cil e costurar o resto do prot�tipo pra usar VWeight + EWeight como peso das c�lulas)
}

long Cell::getEWeight(short neighbor) {
  if (neighbor == -1) return 0;
  return edgeWeight[neighbor];
}

long Cell::getEWeight(Cell* neighbor) {
  short neigh_code = getNeighbor(neighbor);
  return getEWeight(neigh_code);
}

long Cell::getAllEdgesWeight() {
  long aew = 0;
  for (short neigh = 0 ; neigh < getNumNeigh() ; neigh++) {
    aew += getEWeight(neigh);
  }
  return aew;
}

long Cell::getCellWeight() {
  return getAllEdgesWeight() + getVWeight();
}

long Cell::getEWeightToSameRegion() {
  long wtsr = 0;
  Cell* neighCell;
  for (short neigh = 0 ; neigh < getNumNeigh() ; neigh++) {
    neighCell = getNeighbor(neigh);
    if (neighCell && neighCell->getParentRegion() == this->getParentRegion())
      wtsr += getEWeight(neigh);
  }
  return wtsr;
}

long Cell::getEWeightToAnotherRegion() {
  long wtar = 0;
  Cell* neighCell;
  for (short neigh = 0 ; neigh < getNumNeigh() ; neigh++) {
    neighCell = getNeighbor(neigh);
    if (neighCell && neighCell->getParentRegion() != this->getParentRegion())
      wtar += getEWeight(neigh);
  }
  return wtar;
}

long Cell::getEWeightToRegion(Region* reg) {
  long wtr = 0;
  Cell* neighCell;
  for (short neigh = 0 ; neigh < getNumNeigh() ; neigh++) {
    neighCell = getNeighbor(neigh);
    if (neighCell && neighCell->getParentRegion() == reg)
      wtr += getEWeight(neigh);
  }
  return wtr;
}

long Cell::getWorldWeight() {
  return worldWeight;
}

long Cell::getDesireToSwap(Region* r) {
  if (!r) r = regionUnderComparison;
  return getEWeightToRegion(r) - getEWeightToRegion(getParentRegion());
}

void Cell::sortByDesireToSwap(list<Cell*>& cell_list, Region* r) {
  regionUnderComparison = r;
  cell_list.sort(Cell::compareCellDesireToSwap);
}

void Cell::sortByWeight(list<Cell*>& cell_list) {
  cell_list.sort(Cell::compareCellLoad);
}

bool Cell::compareCellDesireToSwap(Cell* cA, Cell* cB) {
  return (cA->getDesireToSwap() > cB->getDesireToSwap());
}

long Cell::getSwapGain(Cell *c1, Cell *c2) {
  return 
    c1->getDesireToSwap(c2->getParentRegion()) + c2->getDesireToSwap(c1->getParentRegion()) - c1->getEWeight(c2) - c2->getEWeight(c1);
}

long Cell::updateEWeight(short neighbor) {
  long edgew = 0;
  Cell* neighborCell = getNeighbor(neighbor);
  if (!neighborCell)
    return -1;
  for (list<Avatar*>::iterator it = avatars.begin() ; it != avatars.end() ; it++) {
    edgew += (*it)->getInteraction(neighborCell);
  }
  edgeWeight[neighbor] = edgew;
  return edgew;
}

void Cell::updateAllEdges() {
  totalEdgeWeight = 0;
  for (int edge = 0 ; edge < NUM_NEIGH ; edge++)
    totalEdgeWeight += updateEWeight(edge);
}

void Cell::updateAllEdgesAndVertexWeights() {
  worldWeight = 0;
  for (int i = 0 ; i < cells_on_a_row ; i++)
    for (int j = 0 ; j < cells_on_a_row ; j++) {
      cellMatrix[i][j]->updateVWeight();
      cellMatrix[i][j]->updateAllEdges();
      //worldWeight += cellMatrix[i][j]->getVWeight();
      //worldWeight += cellMatrix[i][j]->getAllEdgesWeight();
      //TODO: esse aqui talvez n�o seja necess�rio, se corrigir o VWeight pra conter a intera��o fora da c�lula
      worldWeight += cellMatrix[i][j]->getCellWeight();
    }
}

Cell* Cell::getCell(int X, int Y) {
  return cellMatrix[X][Y];
}

Cell* Cell::getOrphanCell() {
  for (int i = 0 ; i < getRowLength() ; i++)
    for (int j = 0 ; j < getRowLength() ; j++) {
      if (!cellMatrix[i][j]->getParentRegion())
        return cellMatrix[i][j];
    }
  return NULL;
}

list<Cell*> Cell::getOrphansSortedByTotalWeight() {
  list<Cell*> orphanList; 
  for (int i = 0 ; i < getRowLength() ; i++)
    for (int j = 0 ; j < getRowLength() ; j++) {
      if (!cellMatrix[i][j]->getParentRegion())
        orphanList.push_back(cellMatrix[i][j]);
    }
  orphanList.sort(compareCellLoad);
  return orphanList;
}

bool Cell::compareCellLoad(Cell* cA, Cell* cB) {
  return cA->getCellWeight() > cB->getCellWeight();
}

Cell* Cell::getNeighbor(short neighbor) {
  Cell* neighborCell;
  int nX = cellposition->X;
  int nY = cellposition->Y;
  switch (neighbor) {
    case UP :
      nY--;
      break;
    case UP_RIGHT :
      nX++;
      nY--;
      break;
    case RIGHT :
      nX++;
      break;
    case DOWN_RIGHT :
      nX++;
      nY++;
      break;
    case DOWN :
      nY++;
      break;
    case DOWN_LEFT :
      nX--;
      nY++;      
      break;
    case LEFT :
      nX--;
      break;
    case UP_LEFT :
      nX--;
      nY--;
      break;
  }
  if (nX < 0 || nY < 0 || nX >= cells_on_a_row || nY >= cells_on_a_row)
    return NULL;
  neighborCell = cellMatrix[nX][nY];
  return neighborCell;
}

short Cell::getNeighbor(Cell* neigh) {
  for (short ncode = 0 ; ncode < NUM_NEIGH ; ncode++)
    if (getNeighbor(ncode) == neigh)
      return ncode;
  return -1;  
}

Cell* Cell::getHighestEdgeFreeNeighbor() {
  Cell* highest_cell = NULL;
  Cell* c;
  long highest_edge = 0;
  for (short neigh = 0 ; neigh < NUM_NEIGH ; neigh++) {
    c = getNeighbor(neigh);
    if (c && !c->getParentRegion() && getEWeight(c) >= highest_edge) {
      highest_cell = c;
      highest_edge = getEWeight(c);
    }
  }
  return highest_cell;    
}

Cell* Cell::getHighestEdgeFreeNeighbor(list<Cell*> &cellList) {
  Cell* highest_cell = NULL;
  Cell* c;
  long highest_edge = 0;
  for (list<Cell*>::iterator it = cellList.begin() ; it != cellList.end() ; it++) {
    c = (*it)->getHighestEdgeFreeNeighbor();
    if (c && (*it)->getEWeight(c) >= highest_edge) {
      highest_cell = c;
      highest_edge = (*it)->getEWeight(c);
    }
  }
  return highest_cell;
}

list<Cell*> Cell::getAllCells(bool mustBeFree) {
  list<Cell*> cell_list;
  for (int i = 0 ; i < getRowLength() ; i++)
    for (int j = 0 ; j < getRowLength() ; j++) {
      if (mustBeFree && cellMatrix[i][j]->getParentRegion()) continue;
      cell_list.push_back(cellMatrix[i][j]);
    }
  return cell_list;
}

Cell* Cell::getHeaviestCell(bool mustBeFree) {
  list<Cell*> cell_list = getAllCells(mustBeFree);
  long highest_weight = 0;
  Cell* heaviest_cell = NULL;
  for (list<Cell*>::iterator it = cell_list.begin() ; it != cell_list.end() ; it++)
    if ((*it)->getCellWeight() >= highest_weight) {
      heaviest_cell = *it;
      highest_weight = (*it)->getCellWeight();
    }
  return heaviest_cell;    
}

list<Cell*> Cell::getAllFreeCells() {
  return getAllCells(true);
}

Cell* Cell::getHeaviestFreeCell() {
  return getHeaviestCell(true);
}

list<Cell*> Cell::getAllNeighbors() {
  list<Cell*> neighlist;
  for (short i = 0 ; i < NUM_NEIGH ; i++) {
    if (getNeighbor(i))
      neighlist.push_back(getNeighbor(i));
  }
  return neighlist;
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
  for (int x = 0 ; x < cells_on_a_row ; x++) {
    for (int y = 0 ; y < cells_on_a_row ; y++) {
      cellMatrix[x][y] = new Cell(x,y);
    }
  }  
}

void Cell::drawCells(SDL_Surface* output) {
  coord cell;
  float alpha;
  
  for (int i = 0 ; i < cells_on_a_row ; i++) {
    for (int j = 0 ; j < cells_on_a_row ; j++) {
      if (showv) {
        alpha = convertToScale((float)cellMatrix[i][j]->getCellWeight(), 0, WW*100/10, 0, 255);
        alpha = alpha>255?255:alpha;
        SDL_SetAlpha( surface_vertex_weight , SDL_SRCALPHA, approx(alpha) );
        apply_surface( i*CELL_LENGTH, j*CELL_LENGTH , surface_vertex_weight, output );
      }

      if (showe) {
        for (short neigh = 0 ; neigh < NUM_NEIGH ; neigh++)
          cellMatrix[i][j]->drawEdge(neigh, output);
      }
    }
  }
}

void Cell::drawEdge(short neighbor, SDL_Surface* output) {
  int x = cellposition->X * CELL_LENGTH;
  int y = cellposition->Y * CELL_LENGTH;
  
  switch (neighbor) {
    case UP :
      x += EDGE_POS_MIDDLE; //imagem de 6x6
      break;
    case UP_RIGHT :
      x += EDGE_POS_CORNER;
      break;
    case RIGHT :
      x += EDGE_POS_CORNER;
      y += EDGE_POS_MIDDLE;
      break;
    case DOWN_RIGHT :
      x += EDGE_POS_CORNER;
      y += EDGE_POS_CORNER;
      break;
    case DOWN :
      x += EDGE_POS_MIDDLE;
      y += EDGE_POS_CORNER;
      break;
    case DOWN_LEFT :
      y += EDGE_POS_CORNER;
      break;
    case LEFT :
      y += EDGE_POS_MIDDLE;
      break;
    case UP_LEFT:
      //do nothing. x and y are the same of the cell's
      break;
  }  
  float alpha = convertToScale((float)getEWeight(neighbor), 0, WW*100/10, 0, 255);
  alpha = alpha>255?255:alpha;
  SDL_SetAlpha( surface_edge_weight , SDL_SRCALPHA, approx(alpha) );
  apply_surface( x, y, surface_edge_weight, output );
}

void Cell::drawBorder(SDL_Surface* output, short border, Uint32 bordercolor) {
  int x1 = cellposition->X * CELL_LENGTH;
  int y1 = cellposition->Y * CELL_LENGTH;
  int x2,y2,x3,y3,x4,y4;
  bool shouldDraw = true;  
  switch (border) {
    case UP :
      x2 = x1 + CELL_LENGTH - 1;
      y2 = y1;
      x3 = x1 + 1;
      y3 = y1 + 1;
      x4 = x2 - 1;
      y4 = y2 + 1;
      break;
    case RIGHT :
      x2 = x1 = x1 + CELL_LENGTH - 1;
      y2 = y1 + CELL_LENGTH - 1;
      x3 = x1 - 1;
      y3 = y1 + 1;
      x4 = x2 - 1;
      y4 = y2 - 1;
      break;
    case DOWN :
      y2 = y1 = y1 + CELL_LENGTH - 1;
      x2 = x1 + CELL_LENGTH - 1;
      x3 = x1 + 1;
      y3 = y1 - 1;
      x4 = x2 - 1;
      y4 = y2 - 1;
      break;
    case LEFT :
      x2 = x1;
      y2 = y1 + CELL_LENGTH - 1;
      x3 = x1 + 1;
      y3 = y1 + 1;
      x4 = x2 + 1;
      y4 = y2 - 1;
      break;
    default :
      shouldDraw = false;
      break;      
  }
  if (shouldDraw) drawLineBresenham(output, x1, y1, x2, y2, bordercolor);
  if (shouldDraw) drawLineBresenham(output, x3, y3, x4, y4, bordercolor);
}


void Cell::drawAllBorders(SDL_Surface* output, Uint32 bordercolor) {
  for (short n = 0 ; n < getNumNeigh() ; n++) {
    if (!this->getNeighbor(n) || this->getParentRegion() != getNeighbor(n)->getParentRegion())
      this->drawBorder(output, n, bordercolor);
  }
}

void Cell::toggleShowVertexWeight() {
  showv = !showv;
}

void Cell::toggleShowEdgeWeight() {
  showe = !showe;
}

void Cell::setCellSurfaces (string vertex_weight_file, string edge_weight_file) {
  surface_vertex_weight = load_image (vertex_weight_file);
  surface_edge_weight = load_image (edge_weight_file);
}

Region* Cell::getParentRegion() {
  return parentRegion;
}

void Cell::setParentRegion(Region* region) {
  parentRegion = region;
}

void Cell::releaseCellFromRegion() {
  parentRegion = NULL;
}

bool Cell::isBorderCell() {
  Cell* neigh;
  for (short n = 0 ; n < getNumNeigh() ; n++) {
    neigh = getNeighbor(n);
    if (neigh && neigh->getParentRegion() != this->getParentRegion())
      return true;
  }
  return false;
}

coord Cell::getCellMatrixPosition() {
  coord position;
  position = *cellposition;
  return position;
}

coord Cell::getAbsolutePosition() {
  coord abspos = *cellposition;
  abspos.X *= getCellLength();
  abspos.Y *= getCellLength();
  return abspos;
}

int Cell::getCellLength() {
  return CELL_LENGTH;
}

int Cell::getRowLength(void) {
  return cells_on_a_row;
}

int Cell::getNumNeigh() {
  return NUM_NEIGH;
}

list<Cell*> Cell::getCluster() {
  list<Cell*> cluster;
  return getCluster(cluster);
}

list<Cell*> Cell::getCluster(list<Cell*> alreadyTaken) {
  list<Cell*> taken = alreadyTaken;
  taken.push_back(this);
  Cell* _cell = NULL;
  Region* parent = getParentRegion();
  
  for (short neigh = 0 ; neigh < 8 ; neigh++) {
    _cell = getNeighbor(neigh)
    if (_cell && _cell->getParentRegion() == parent) {
      
         // verificando se já está na lista de taken
            containsCell = false;
            for (list<Cell*>::iterator it = taken.begin() ; it != taken.end() ; it++)
               if (*it == _cell)
                  containsCell = true;
         // fim verif.
      
      if (!containsCell) {
        taken.merge(_cell->getCluster(taken));
      }
    }
  }
  
  taken.sort();
  taken.unique();
  return taken;
}