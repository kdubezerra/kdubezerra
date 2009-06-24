#include "kdtree.h"
#include "Avatar.h"
#include "Server.h"
#include "myutils.h"


SDL_Surface* KDTree::screen = NULL;

KDTree::KDTree() {
	parent = schild = bchild = NULL;
  server = NULL;
  split_coordinate = -1;
}

KDTree::KDTree(int _node_id) {
	parent = schild = bchild = NULL;
  server = NULL;
  split_coordinate = -1;
  node_id = _node_id;
}

KDTree::KDTree(list<Server*> _server_list, list<Avatar*> &_avatar_list) {
  split_coordinate = -1;
	parent = schild = bchild = NULL;
  server = NULL;
	buildTree(_server_list, 0, 0, _avatar_list, X_NODE);  
  runTree();	
}

KDTree::~KDTree() {
}

/*
		void splitLeaf();
		void moveSplitCoordinate(); //somente se esse nodo tiver duas sub-árvores
*/

void KDTree::buildTree(list<Server*> _server_list, int _server_number, int _tree_lvl, list<Avatar*> _avatars, short _split_lvl) {
	///***************************************************************************************************
  ///(INICIO) armazenando os limites definidos por cada nodo, para nao necessitar de calculo em run-time
  if (_tree_lvl == 0) {
    xmin = ymin = 0;
    xmax = ymax = WW - 1;
  }
  else {
    xmin = parent->xmin;
    xmax = parent->xmax;
    ymin = parent->ymin;
    ymax = parent->ymax;
    if (this == parent->schild) {    
      if (_split_lvl == X_NODE) {
        ymax = parent->split_coordinate - 1;
      }
      else {
        xmax = parent->split_coordinate - 1;
      }
    }
    else {
      if (_split_lvl == X_NODE) {
        ymin = parent->split_coordinate;
      }
      else {
        xmin = parent->split_coordinate;
      }
    }
  }
  ///(FIM) armazenando os limites definidos por cada nodo, para nao necessitar de calculo em run-time
  ///***************************************************************************************************
  
  ///**********************************
  ///(INICIO) NOH FOLHA
  if (_server_number + intPow(2, _tree_lvl) >= _server_list.size()) { 
    avList = _avatars;
    for (list<Avatar*>::iterator it = avList.begin() ; it != avList.end() ; it++)
      (*it)->setParentNode(this);
    setServer(Server::getServerById(_server_number));
    return;
  }
  ///(FIM) NOH FOLHA
  ///**********************************
  
  ///******************************************************
  ///(INICIO) NOH INTERMEDIARIO + RECURSAO
	schild = new KDTree(_server_number);	
	bchild = new KDTree(_server_number + intPow(2, _tree_lvl));
	schild->parent = bchild->parent = this;
  
  int _middle_index = _avatars.size() / 2;
	list<Avatar*> slist, blist;
	int i;
	list<Avatar*>::iterator it;
  
  if (_split_lvl == X_NODE) _avatars.sort(Avatar::compareX);
  else _avatars.sort(Avatar::compareY);

  for (i = 0, it = _avatars.begin() ; it != _avatars.end() ; it++, i++) {
    if (i == _middle_index) {
      if (_split_lvl == X_NODE) split_coordinate = (*it)->GetX();
      else split_coordinate = (*it)->GetY();
    }
    if (i < _middle_index) slist.push_back(*it);
    else blist.push_back(*it);
  }

  
  
  cout << "xmin = " << xmin << "; xmax = " << xmax << "; ymin = " << ymin << "; ymax = " << ymax << "." << endl;

  schild->buildTree(_server_list, _server_number, _tree_lvl + 1, slist, (_split_lvl == X_NODE) ? Y_NODE : X_NODE);
	bchild->buildTree(_server_list, _server_number + intPow(2, _tree_lvl), _tree_lvl + 1, blist, (_split_lvl == X_NODE) ? Y_NODE : X_NODE);
  ///(FIM) NOH INTERMEDIARIO + RECURSAO
  ///******************************************************
}

void KDTree::setScreen(SDL_Surface* _screen) {
	screen = _screen;
}

void KDTree::drawTree() {
  if (schild) { ///i.e. nao eh noh-folha
    schild->drawTree();
    bchild->drawTree();
  }
  else{
		Uint32 color = colorTable(node_id);
    color &= 0xFFFFFF88;
    boxColor(screen, xmin, ymin, xmax, ymax, color);
    string load = longToString(getWeight());
    string power = longToString(server->getServerPower());
    stringColor(screen, xmin, ymin, power.c_str(), 0x000000FF);
    stringColor(screen, xmin, ymin+20, load.c_str(), 0x000000FF);
	}
}

void KDTree::runTree() {
  cout << "Node id #" << node_id << ": " << split_coordinate << endl;
  if (schild) {
    schild->runTree();
    bchild->runTree();
  }
}

long KDTree::getWeight() {
  if (schild) {
    return schild->getWeight() + bchild->getWeight();
  } else {
///    long _w = 0;
///    for (list<Avatar*>::iterator it = avList.begin() ; it != avList.end() ; it++) {
///      _w += (*it)->getWeight();
///    }
///    return _w;
    return avList.size();
  }
}

long KDTree::getCapacity() {
}

list<Avatar*> KDTree::getAvList() {
  return avList;
}

void KDTree::setServer(Server* _server) {
  server = _server;
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
