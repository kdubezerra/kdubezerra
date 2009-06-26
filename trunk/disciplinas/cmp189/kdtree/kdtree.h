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
#define LEAF_NODE 2
#define RECURSIVE 1

class KDTree {
	
	public:
		KDTree();
		KDTree(int _node_id);
		KDTree(list<Server*> _server_list, list<Avatar*> &_avatar_list);
		~KDTree();
		void splitLeaf();
		void setSplitCoordinate(int _coord); //somente se esse nodo tiver duas sub-árvores
		void setLimits();
		void checkBalance(short recursive = 0);
		void checkBalanceFromRoot();
		void balanceLoad();
		
		void setScreen(SDL_Surface* _screen);
		void drawTree();
		void runTree();
		
		long getWeight();
		long getPower();
		list<Avatar*> getAvList();
		void removeAvatar(Avatar* _av);
		void insertAvatar(Avatar* _av, short _split_lvl = X_NODE);
		void clearAvList();
		static KDTree* getRoot();
		
		void setServer(Server* _server);
		
		void getLimits(int& _xmin, int& _xmax, int& _ymin, int& _ymax);
	
	protected:		
		void buildTree(list<Server*> _server_list, int _server_number, int _tree_lvl, list<Avatar*> _avatars, short _split_lvl);
		void reckonCapacity();
		void reckonLoad();
		void reckonRects();
		short split_axis;
		KDTree *parent, *schild, *bchild;
		Server *server;
		list<Avatar*> avList;
		int split_coordinate, xmin, xmax, ymin, ymax;		
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
		static KDTree* root;
};
