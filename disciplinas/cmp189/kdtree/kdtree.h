#pragma once

#include <list>
#include <vector>
#include "SDL/SDL_gfxPrimitives.h"

using namespace std;

class Avatar;
class Server;

typedef struct {
	int x,y;
} point;

#define X_NODE 0
#define Y_NODE 1

class KDTree {
	
	public:
		KDTree();
		KDTree(int _node_id);
		KDTree(int _num_servers, list<Avatar*> &_avatar_list);
		~KDTree();
		void splitLeaf();
		void moveSplitCoordinate(); //somente se esse nodo tiver duas sub-árvores
		void balanceLoad();
		
		void setScreen(SDL_Surface* _screen);
		void drawTree();
		void runTree();
	
	protected:
		void drawTree(int _xmin, int _xmax, int _ymin, int _ymax, short _split_lvl);
		void buildTree(int _num_servers, int _server_number, int _tree_lvl, vector<Avatar*> _sorted_x, vector<Avatar*> _sorted_y, short _split_lvl);
		void reckonCapacity();
		void reckonLoad();
		void reckonRects();
		KDTree *parent, *schild, *bchild;
		int split_coordinate, xmin, xmax, ymin, ymax;
		list<Avatar*> avList;
		int node_id;
		int load;
							// a carga (load) de um nodo é igual à soma das cargas de
							// seus filhos; se forem folhas, os filhos são avatares,
							// cujo peso é calculado pela aplicação.
		int capacity; 
							// cada nodo no penúltimo nível da árvore corresponde a um
							// servidor, e seus filhos são os avatares dos jogadores.
							// a capacidade de um nodo é igual à capacidade de seus
							// filhos, e a capacidade de cada nodo no penúltimo nível
							// é igual à capacidade do servidor correspondente
		static SDL_Surface* screen;
};
