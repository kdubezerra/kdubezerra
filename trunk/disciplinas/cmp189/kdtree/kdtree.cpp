#include "kdtree.h"
#include "Avatar.h"
#include "Server.h"
#include "myutils.h"


SDL_Surface* KDTree::screen = NULL;
KDTree* KDTree::root = NULL;

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
  KDTree::root = this;
  split_coordinate = -1;
	parent = schild = bchild = NULL;
  server = NULL;
	buildTree(_server_list, 0, 0, _avatar_list, X_NODE);  
  runTree();	
}

KDTree::~KDTree() {
}

void KDTree::setSplitCoordinate(int _coord) {
  if (split_axis == X_NODE) {
    if (xmax - xmin <= 1) return;
    if (_coord <= xmin) _coord = xmin + 1;
    if (_coord > xmax) _coord = xmax;    
  }
  
  if (split_axis == Y_NODE) {
    if (ymax - ymin <= 1) return;
    if (_coord <= ymin) _coord = ymin + 1;
    if (_coord > ymax) _coord = ymax;
  }
  
  split_coordinate = _coord;
  if (schild) {
    schild->setLimits();
    bchild->setLimits();
  }
}

void KDTree::setLimits(int _lvl) {  
  xmin = parent->xmin;
  xmax = parent->xmax;
  ymin = parent->ymin;
  ymax = parent->ymax;
  if (this == parent->schild) {    
    if (parent->split_axis == Y_NODE) {
      ymax = parent->split_coordinate - 1;
    }
    else {
      xmax = parent->split_coordinate - 1;
    }
  }
  else {
    if (parent->split_axis == Y_NODE) {
      ymin = parent->split_coordinate;
    }
    else {
      xmin = parent->split_coordinate;
    }
  }
  
  if (_lvl > 0) {
    if (split_axis == X_NODE) {
      if (split_coordinate <= xmin || split_coordinate > xmax) split_coordinate = (xmin + xmax) / 2;
    }
    else {
      if (split_coordinate <= ymin || split_coordinate > ymax) split_coordinate = (ymin + ymax) / 2;
    }
  }
  
  if (schild) {
    schild->setLimits(_lvl+1);
    bchild->setLimits(_lvl+1);
  }
}

void KDTree::checkBalance(short recursive) {
  if (!schild && getWeight() >= DISBAL_TOLERANCE * (float) server->getServerPower())
    balanceLoad();
  if (recursive && schild) {
    schild->checkBalance(RECURSIVE);
    bchild->checkBalance(RECURSIVE);
  }
}

void KDTree::checkBalanceFromRoot() {
  root->checkBalance(RECURSIVE);
}

void KDTree::balanceLoad() {
  if (!parent) return;
  if (parent->getPower() < parent->getWeight())
    parent->balanceLoad();
  
  list<Avatar*>::iterator it;
  list<Avatar*> avs = parent->getAvList();
  list<Avatar*> my_share;
  avs.sort((parent->split_axis == X_NODE) ? (Avatar::compareX) : (Avatar::compareY));
  
  float power_share = (float) getPower() / (float) parent->getPower();
  long weight_share = power_share * (float) parent->getWeight();
  long current_share = 0l;
  
  if (this == parent->bchild) avs.reverse();
  
  ///long new_share = 0l;
  for (it = avs.begin() ; it != avs.end() && current_share < weight_share ; it++) {
    ///current_share = new_share;
    ///new_share = current_share + (*it)->getWeight();
    current_share += (*it)->getWeight();
  }
  ///if qual eh o share mais proximo de weight_share? com ou sem o ultimo avatar? nao precisa ser otimo...
    
  parent->setSplitCoordinate( (parent->split_axis == X_NODE) ? (*it)->GetX() : (*it)->GetY() );
  ///cout << "split_coordinate: " << parent->split_coordinate << endl;
  
  parent->clearAvList();/**
  for (it = avs.begin() ; it != avs.end() && node_id == 3 ; it++) {
    if (parent->split_axis == X_NODE) cout << "x = " << (int) ((*it)->GetX()) << endl;
    else cout << "y = " << (int) ((*it)->GetY()) << endl;
  }*/
  for (it = avs.begin() ; it != avs.end() ; it++) {
    parent->insertAvatar(*it);
  }
}

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
    split_axis = LEAF_NODE;
    return;
  }
  ///(FIM) NOH FOLHA
  ///**********************************
  
  ///******************************************************
  ///(INICIO) NOH INTERMEDIARIO + RECURSAO
  split_axis = _split_lvl;
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
    color = color ? color : 0xFFFFFF;
    color &= 0xFFFFFF88;
    boxColor(screen, xmin, ymin, xmax, ymax, color);    
    string power = longToString(server->getServerPower());
    string load = longToString(getWeight());
    stringColor(screen, xmin, ymin, power.c_str(), 0x000000FF);
    stringColor(screen, xmin, ymin+10, load.c_str(), 0x000000FF);
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
  }
  else {
    long _w = 0;
    for (list<Avatar*>::iterator it = avList.begin() ; it != avList.end() ; it++) {
      _w += (*it)->getWeight();
    }
    return _w;
  }
}

long KDTree::getPower() {
  if (schild) {
    return schild->getPower() + bchild->getPower();
  }
  else {
    return server->getServerPower();
  }
}

list<Avatar*> KDTree::getAvList() {
  if (schild) {
    list<Avatar*> avs, childs;
    childs = schild->getAvList();
    avs.insert(avs.end(), childs.begin(), childs.end());
    childs = bchild->getAvList();
    avs.insert(avs.end(), childs.begin(), childs.end());
    return avs;
  }
  else
    return avList;
}

void KDTree::removeAvatar(Avatar* _av) {
  avList.remove(_av);
}

void KDTree::insertAvatar(Avatar* _av) {
  ///*********************************************************
  ///(INICIO) NOH INTERMEDIARIO: PRECISA DESCER MAIS NA ÁRVORE
  if (schild) {
    if (split_axis == X_NODE) {
      if ((int) _av->GetX() < split_coordinate)
        schild->insertAvatar(_av);
      else
        bchild->insertAvatar(_av);
    }
    else {
      if ((int) _av->GetY() < split_coordinate)
        schild->insertAvatar(_av);
      else
        bchild->insertAvatar(_av);
    }
  }
  ///(FIM) NOH INTERMEDIARIO
  ///*********************************************************
  
  ///*******************************
  ///(INICIO) NOH FOLHA: INSIRA AQUI
  else {
    _av->setParentNode(this);
    avList.push_back(_av);
  }
  ///(FIM) NOH FOLHA
  ///*******************************
}

void KDTree::clearAvList() {
  if (schild) {
    schild->clearAvList();
    bchild->clearAvList();
  }
  avList.clear();
}

KDTree* KDTree::getRoot() {
  return root;
}

void KDTree::setServer(Server* _server) {
  server = _server;
  _server->setNode(this);
}

Server* KDTree::getServer() {
  return server;
}

void KDTree::getLimits(int& _xmin, int& _xmax, int& _ymin, int& _ymax) {
  _xmin = xmin;
  _xmax = xmax;
  _ymin = ymin;
  _ymax = ymax;
}
