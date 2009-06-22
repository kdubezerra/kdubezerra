
#pragma once
#ifndef _KDTREE_H_
#define _KDTREE_H_
#include <list>
#include <vector>

using namespace std;

class Avatar;

typedef struct {
	int x,y;
} point;

#define X_NODE 0;
#define Y_NODE 1;

bool comparePointsX(point a, point b);

bool comparePointsY(point a, point b);

class KDTree {
	
	public:
		KDTree();
		KDTree(int _num_servers, list<Avatar*> &_avatar_list);
		~KDTree();
		void splitLeaf();
		void moveSplitCoordinate(); //somente se esse nodo tiver duas sub-árvores
		void balanceLoad();
		void drawTree();
	
	protected:
		void buildTree(int _num_servers, int _server_number, int _sorted_x, int _sorted_y, short _split_lvl);
		void reckonCapacity();
		void reckonLoad();
		void reckonRects();
		KDTree *parent, *schild, *bchild;
		int split_coordinate, xmin, xmax, ymin, ymax;
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
};
#endif
