#pragma once

#include <list>
#include <vector>

#if defined(_WIN32)// || defined(__APPLE__)
#include <SDL_gfxPrimitives.h>
#else //linux or mac
#include <SDL/SDL_gfxPrimitives.h>
#endif


using namespace std;

class Avatar;
class Server;

#define X_NODE 0
#define Y_NODE 1
#define LEAF_NODE 2
#define RECURSIVE 1

class BSPTree {
	
	public:
		BSPTree();
		BSPTree(int _node_id);
		BSPTree(list<Server*> _server_list, list<Avatar*> _avatar_list);
		~BSPTree();
		void splitLeaf();
		void setSplitCoordinate(int _coord); //somente se esse nodo tiver duas sub-árvores
		void setLimits(int _lvl = 0);
		void checkBalance(short recursive = 0);
		void checkBalanceFromRoot();
		void balanceLoad();
		
		void setScreen(SDL_Surface* _screen);
		void drawTree();
		void runTree();
		
		unsigned long long getWeight();
		long getPower();
		list<Avatar*> getAvList();
		void removeAvatar(Avatar* _av);
		void insertAvatar(Avatar* _av);
		void clearAvList();
		static BSPTree* getRoot();
		
		void setServer(Server* _server);
    Server* getServer();
		
		void getLimits(int& _xmin, int& _xmax, int& _ymin, int& _ymax);
		static void toggleDisplayRegions();
		static void toggleDisplayRegionsWeights();
	
	protected:		
		void buildTree(list<Server*> _server_list, int _server_number, int _tree_lvl, list<Avatar*> _avatars, short _split_lvl);
		void reckonCapacity();
		void reckonLoad();
		void reckonRects();
		short split_axis;
		BSPTree *parent, *schild, *bchild;
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
							// filhos, e a capacidade de cada nodo folha
							// é igual à capacidade do servidor correspondente
		static SDL_Surface* screen;
		static BSPTree* root;
		static bool drawing_regions;
		static bool drawing_loads;
};
