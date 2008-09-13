#include "Cell.h"
#include "Avatar.h"
#include "myutils.h"

//===========================================static members

Cell*** Cell::cellMatrix = NULL;
int Cell::cells_on_a_row = 0;
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

float Cell::getVWeight() {//TODO mudar para ser a soma dos pesos individuais de cada avatar, interagindo DENTRO da celula
  return avatars.size();
}

float Cell::getEWeight(short neighbor) {
  return edgeWeight[neighbor];
}

int Cell::updateEWeight(short neighbor) {
  float totalw = 0.0f;
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
    return 1;
  neighborCell = cellMatrix[nX][nY];      
  for (list<Avatar*>::iterator it = avatars.begin() ; it != avatars.end() ; it++) {
    totalw += (*it)->getInteraction(neighborCell);
  }
  edgeWeight[neighbor]=totalw;
  return 0;
}

void Cell::udpdateAllEdges() {
  for (int edge = 0 ; edge < NUM_NEIGH ; edge++)
    updateEWeight(edge);
}

Cell* Cell::getCell(int X, int Y) {
  return cellMatrix[X][Y];
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

void Cell::drawCells(SDL_Surface* output) {
  coord cell;
  float alpha;
  
  for (int i = 0 ; i < cells_on_a_row ; i++) {
    for (int j = 0 ; j < cells_on_a_row ; j++) {       
//       alpha = 255 * vweight_matrix[i][j] / total_weight;
      if (showv) {
        alpha = convertToScale(cellMatrix[i][j]->getVWeight(), 0, WW/10, 0, 255);
        alpha = alpha>255?255:alpha;
        SDL_SetAlpha( surface_vertex_weight , SDL_SRCALPHA, approx(alpha) );
        apply_surface( i*CELL_LENGTH, j*CELL_LENGTH , surface_vertex_weight, output );
      }

      if (showe) {
        alpha = convertToScale(cellMatrix[i][j]->getEWeight(UP), 0, WW/10, 0, 255);
        alpha = alpha>255?255:alpha;
        SDL_SetAlpha( surface_edge_weight , SDL_SRCALPHA, approx(alpha) );
        apply_surface( i*CELL_LENGTH, j*CELL_LENGTH , surface_edge_weight, output );
      }
    }
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

int Cell::getRowLength(void) {
  return cells_on_a_row;
}