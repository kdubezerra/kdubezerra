#include "kdtree.h"
#include "Avatar.h"
#include "myutils.h"


bool comparePointsX(point a, point b) {
	return a.x < b.x;
}

bool comparePointsY(point a, point b) {
	return a.y < b.y;
}

KDTree::KDTree() {
	parent = schild = bchild = NULL;
}

KDTree::KDTree(int _num_servers, list<Avatar*> &_avatar_list) {
	parent = schild = bchild = NULL;
	
	vector<Avatar*> sorted_x, sorted_y;
	
	_avatar_list.sort(Avatar::compareX);
	sorted_x.insert(sorted_x.begin(), _avatar_list.begin(), _avatar_list.end());
	
	_avatar_list.sort(Avatar::compareY);
	sorted_y.insert(sorted_y.begin(), _avatar_list.begin(), _avatar_list.end());
	
	buildTree(_num_servers, 0, 0, sorted_x, sorted_y, X_NODE);
	
}

KDTree::~KDTree() {
}

/*
		void splitLeaf();
		void moveSplitCoordinate(); //somente se esse nodo tiver duas sub-árvores
		void balanceLoad();
		void drawTree();
	
	protected:
*/	

void KDTree::buildTree(int _num_servers, int _server_number, int _tree_lvl, vector<Avatar*> _sorted_x, vector<Avatar*> _sorted_y, short _split_lvl) {
	if (_num_servers == 0) return;
	if (_tree_lvl > 0 && _server_number + intPow(2, _tree_lvl - 1) >= _num_servers) return;
	schild = new KDTree();	
	bchild = new KDTree();
	schild->parent = bchild->parent = this;
	schild->buildTree(_num_servers, _server_number, _tree_lvl + 1, _sorted_x, _sorted_y, (_split_lvl == X_NODE) ? Y_NODE : X_NODE);
}
/*
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

*/
