#pragma once
#include <list>

class Avatar;

typedef struct {
	int x,y;
} point;

bool comparePointsX(point a, point b) {
	return a.x < b.x;
}

bool comparePointsY(point a, point b) {
	return a.y < b.y;
}

class KDTree {
	
	public:
		KDTree();
		KDTree(list<Avatar> _avatar_list);
		~KDTree();
		void splitLeaf();
		void moveSplitCoordinate(); //somente se esse nodo tiver duas sub-árvores
		void balanceLoad();
		void drawTree();
	
	protected:
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
