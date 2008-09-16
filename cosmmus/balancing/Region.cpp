#include "Region.h"
#include "Region.h"
#include "myutils.h"

//===========================================static members

list<Region*> Region::regionList;
bool Region::showr, showe;

//================================cons/des-truction methods

Region::Region(){  
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

list<Cell*> &Region::getNeighbors() {
  return neighbors;
}

int Region::getNumberOfNeighbors() {
  return neighbors.size();
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
  float edgew = 0.0f;
  for (list<Cell*>::iterator it = cells.begin() ; it != cell.end() ; it++)
    
  
}

int Region::updateEWeight(short neighbor) {
  float totalw = 0.0f;
  Region* neighborRegion;
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
    return 1;
  neighborRegion = cellMatrix[nX][nY];      
  for (list<Cell*>::iterator it = cells.begin() ; it != cells.end() ; it++) {
    totalw += (*it)->getInteraction(neighborRegion);
  }
  edgeWeight[neighbor] = totalw;
  return 0;
}

void Region::updateAllEdges() {
  for (int edge = 0 ; edge < NUM_NEIGH ; edge++)
    updateEWeight(edge);
}

Region* Region::getRegion(int X, int Y) {
  return cellMatrix[X][Y];
}

void Region::allocRegionMatrix(int row) {
  if (cellMatrix) {
    for (int i = 0 ; i < cells_on_a_row ; i++)
      delete [] cellMatrix[i];
  }
  delete [] cellMatrix;  
  cells_on_a_row = row;  
  cellMatrix = new Region** [row];
  for (int i = 0 ; i < row ; i++)
    cellMatrix[i] = new Region* [row];
  
  for (int x = 0 ; x < cells_on_a_row ; x++) {
    for (int y = 0 ; y < cells_on_a_row ; y++) {
      cellMatrix[x][y] = new Region(x,y);
    }
  }
  
}

void Region::drawRegions(SDL_Surface* output) {
  coord cell;
  float alpha;
  
  for (int i = 0 ; i < cells_on_a_row ; i++) {
    for (int j = 0 ; j < cells_on_a_row ; j++) {
      if (showv) {
        alpha = convertToScale(cellMatrix[i][j]->getVWeight(), 0, WW/20, 0, 255);
        alpha = alpha>255?255:alpha;
        SDL_SetAlpha( surface_vertex_weight , SDL_SRCALPHA, approx(alpha) );
        apply_surface( i*CELL_LENGTH, j*CELL_LENGTH , surface_vertex_weight, output );
      }

      if (showe) {
        cellMatrix[i][j]->updateAllEdges();
        for (short neigh = 0 ; neigh < NUM_NEIGH ; neigh++)
          cellMatrix[i][j]->drawEdge(neigh, output);
      }
    }
  }
}

void Region::drawEdge(short neighbor, SDL_Surface* output) {
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
  float alpha = convertToScale(getEWeight(neighbor), 0, WW/20, 0, 255);
  alpha = alpha>255?255:alpha;
  SDL_SetAlpha( surface_edge_weight , SDL_SRCALPHA, approx(alpha) );
  apply_surface( x, y, surface_edge_weight, output );
}

void Region::toggleShowVertexWeight() {
  showv = !showv;
}

void Region::toggleShowEdgeWeight() {
  showe = !showe;
}

void Region::setRegionSurfaces (string vertex_weight_file, string edge_weight_file) {
  surface_vertex_weight = load_image (vertex_weight_file);
  surface_edge_weight = load_image (edge_weight_file);
}

int Region::getRowLength(void) {
  return cells_on_a_row;
}
