// Jae Chung 7-13-99
// Example of a aimple and dull Agent that
// illustrates the use of OTcl linkages

// #pragma once

#include <stdio.h>
#include <string.h>
// #include "common/agent.h"
#include "RoundSmoothAOIAvatar.h"
		
class CosmmusSimulator {

	public:
		CosmmusSimulator();
		double getRelevance(int playerA, int playerB);
		void step();

	protected:
		int command(int argc, const char*const* argv);
		int    my_var1;
		double my_var2;
		void   MyPrivFunc(void);
		
		RoundSmoothAOIAvatar* players[nplayers];
};


// static class CosmmusSimulatorClass : public TclClass {
// public:
// 		CosmmusSimulatorClass() : TclClass("CosmmusSimulator") {}
// 		TclObject* create(int, const char*const*) {
// 					return(new CosmmusSimulator());
// 		}
// } class_cosmmus_simulator;


CosmmusSimulator::CosmmusSimulator() {
// 	bind("my_var1_otcl", &my_var1);
// 	bind("my_var2_otcl", &my_var2);
	
	for (int i = 0 ; i < nplayers ; i++) {
		players[i] = new RoundSmoothAOIAvatar();
	}
}


double CosmmusSimulator::getRelevance(int A, int B) {
	players[A]->step();
// 	players[B]->step();
// 	step();
// 	cout << players[A]->OtherRelevance(players[B]) << endl;
	return players[A]->OtherRelevance(players[B]);
}


void CosmmusSimulator::step() {
	for (int i = 0 ; i < nplayers ; i++) {
		players[i]->step();
	}
}


int CosmmusSimulator::command(int argc, const char*const* argv) {
		if(argc == 2) {
			if(strcmp(argv[1], "show_players") == 0) {
						MyPrivFunc();
// 						return(TCL_OK);
						return 0;
			}
		}		
}


void CosmmusSimulator::MyPrivFunc(void) {
// 		Tcl& tcl = Tcl::instance();
// 		tcl.eval("puts \"Message From MyPrivFunc\"");
// 		tcl.evalf("puts \"     number of players = %d\"", nplayers);		
}



int main () {
	CosmmusSimulator bli;
// 	for (int i = 1 ; i < nplayers ; i++) printf ("Relevancia do player %d para o player 0: %lf\n", i, bli.getRelevance(0,i));
	while (1) bli.getRelevance(0,1);
	return 0;
}
