#include <list>
#include <map>

#define NUM_NEIGH 8

//NEIGHBORS
#define UP 0
#define UP_RIGHT 1
#define DOWN_RIGHT 2
#define DOWN 3
#define DOWN_LEFT 4
#define UP_LEFT 5
//when using hexagons, or shifted squares, there is no need for right and left neighbors
#define RIGHT 6
#define LEFT 7

//parameters for the cells
#define CELLS_ON_A_ROW 15
#define CELL_LENGTH int(WW/CELLS_ON_A_ROW)

class Avatar;

class Cell {
    
  public:
    
    Cell();    
    Cell(int coord_x, int coord_y);
    
    ~Cell();
    
    void subscribe(Avatar* av);
    
    void unsubscribe(Avatar* av);
    
    map<Avatar*, Avatar*> &getAvatars();
    
    float getVWeight();
    float getEWeight(short neighbor);
    int updateEWeight(Cell* neighbor);
    void udpdateAllEdges();
    
    Cell* getNeighbor(short neigh);
    
    static void allocCellMatrix(int cells_on_a_row);
    static void drawCells();
    
  protected:
    
    coord cellposition;
    float edgeWeight[NUM_NEIGH];
    map<Avatar*, Avatar*> avatars;
    static Cell*** cellMatrix;
    static int cells_on_a_row;    
};