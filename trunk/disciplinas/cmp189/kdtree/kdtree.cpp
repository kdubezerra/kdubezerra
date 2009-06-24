#include "kdtree.h"
#include "Avatar.h"
//#include "Server.h"
#include "myutils.h"


SDL_Surface* KDTree::screen = NULL;

KDTree::KDTree() {
	parent = schild = bchild = NULL;
  split_coordinate = -1;
}

KDTree::KDTree(int _node_id) {
	parent = schild = bchild = NULL;
  split_coordinate = -1;
  node_id = _node_id;
}

KDTree::KDTree(int _num_servers, list<Avatar*> &_avatar_list) {
  split_coordinate = -1;
	parent = schild = bchild = NULL;
	
	vector<Avatar*> sorted_x, sorted_y;
	
	_avatar_list.sort(Avatar::compareX);
	sorted_x.insert(sorted_x.begin(), _avatar_list.begin(), _avatar_list.end());
	
	_avatar_list.sort(Avatar::compareY);
	sorted_y.insert(sorted_y.begin(), _avatar_list.begin(), _avatar_list.end());
	
	buildTree(_num_servers, 0, 0, sorted_x, sorted_y, X_NODE);
  
  runTree();
	
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
	if (_server_number + intPow(2, _tree_lvl) >= _num_servers) return;
	schild = new KDTree(_server_number);	
	bchild = new KDTree(_server_number + intPow(2, _tree_lvl));
	schild->parent = bchild->parent = this;
	vector<Avatar*> svector, bvector;
	int i;
	vector<Avatar*>::iterator it;

	if (_split_lvl == X_NODE) {
		int _middle_index = _sorted_x.size() / 2;
		cout << "middle index X = " << _middle_index << endl;
		split_coordinate = (int)_sorted_x[_middle_index]->GetX();
		
		for (i = 0, it = _sorted_x.begin() ; it != _sorted_x.end() ; it++, i++) {
			if (i < _middle_index) svector.push_back(*it);
			else bvector.push_back(*it);
		}
		
		schild->buildTree(_num_servers, _server_number, _tree_lvl + 1, svector, _sorted_y, Y_NODE);
		bchild->buildTree(_num_servers, _server_number + intPow(2, _tree_lvl), _tree_lvl + 1, bvector, _sorted_y, Y_NODE);
	}

	if (_split_lvl == Y_NODE) {
		int _middle_index = _sorted_y.size() / 2;
		cout << "middle index Y = " << _middle_index << endl;
		split_coordinate = (int)_sorted_y[_middle_index]->GetY();
		
		for (i = 0, it = _sorted_y.begin() ; it != _sorted_y.end() ; it++, i++) {
			if (i < _middle_index) svector.push_back(*it);
			else bvector.push_back(*it);
		}
		
		schild->buildTree(_num_servers, _server_number, _tree_lvl + 1, _sorted_x, svector, X_NODE);
		bchild->buildTree(_num_servers, _server_number + intPow(2, _tree_lvl), _tree_lvl + 1, _sorted_x, bvector, X_NODE);
	}


}

void KDTree::setScreen(SDL_Surface* _screen) {
	screen = _screen;	
}

void KDTree::drawTree() {
	drawTree(0, WW - 1, 0, WW - 1, X_NODE);
}

///TODO: fazer a questão do (min - split-1 ; split - max) não dar pau
void KDTree::drawTree(int _xmin, int _xmax, int _ymin, int _ymax, short _split_lvl) {
	if (schild) {
		switch (_split_lvl) {
			case X_NODE: {
				schild->drawTree(_xmin, split_coordinate -1, _ymin, _ymax, Y_NODE);
				bchild->drawTree(split_coordinate, _xmax, _ymin, _ymax, Y_NODE);
        break;
			}
			case Y_NODE: {
				schild->drawTree(_xmin, _xmax, _ymin, split_coordinate - 1, X_NODE);
				bchild->drawTree(_xmin, _xmax, split_coordinate, _ymax, X_NODE);
        break;
			}
		}
	}	else {
		/**
		SDL_Rect l_node_area;
		l_node_area.x = _xmin;
		l_node_area.y = _ymin;
		l_node_area.w = _xmax - _xmin + 1;
		l_node_area.h = _ymax - _ymin + 1;
		**/
		Uint32 color = colorTable(node_id);
		
//		cout <<	screen << " " <<	_xmin << " " <<	_ymin << " " <<	_xmax << " " <<	_ymax << " " <<	screen << " " << (Uint8) ((color & 0xFF0000) >> 16) << " " << (Uint8) ((color & 0x00FF00) >> 8) << " " <<	(color & 0x0000FF) << " " << 0xFF << endl;
		//cout <<	screen << " (" <<	_xmin << ", " <<	_ymin <<  ") : (" << _xmax << ", " <<	_ymax << ") " << screen << " " << rand()%255 << " " << rand()%255 << " " <<	rand()%255 << " " << 0xFF << endl;
		
		boxRGBA(screen, _xmin, _ymin, _xmax, _ymax, (Uint8) ((color & 0xFF0000) >> 16), (Uint8) ((color & 0x00FF00) >> 8), (Uint8) (color & 0x0000FF) , 0xFF);
//  cout << "Should print the freaking box" << endl;
//  boxRGBA(screen, 20, 20, 200, 200, 255, 127, 63, 255);

	}
}

void KDTree::runTree() {
  cout << "Node id #" << node_id << ": " << split_coordinate << endl;
  if (schild) {
    schild->runTree();
    bchild->runTree();
  }
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
