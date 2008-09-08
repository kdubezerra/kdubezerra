#include <list>
#include <map>

#define NUM_NEIGH 8

//NEIGHBORS
#define UP 0
#define UP_RIGHT 1
#define RIGHT 2
#define DOWN_RIGHT 3
#define DOWN 4
#define DOWN_LEFT 5
#define LEFT 6
#define UP_LEFT 7

Class Avatar;

class Cell {
    
  public:
    
    Cell();    
    Cell(int coord_x, int coord_y);
    
    ~Cell();
    
    void subscribe(Avatar* av);
    
    void unsubscribe(Avatar* av);
    
    list<Avatar*> getAvatars();
    
    float getVWeight();
    float getEWeight(Cell* neighbor);
    
    Cell* getNeighbor(short neigh);
    
    static void allocCellMatrix(int cells_on_a_row);
    
  protected:
    
    coord cellposition;
    map<Avatar*, Avatar*> avatars;
    static Cell*** cellMatrix;
    static int cells_on_a_row;
  
};