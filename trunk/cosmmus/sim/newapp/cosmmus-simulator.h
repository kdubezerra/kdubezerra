#pragma once

#include <stdio.h>
#include <string.h>
#include "agent.h"
#include "timer-handler.h"
#include "AvatarPack.h"
		
class CSim; //CSim for "Cosmmus Simulator"
				
				
				
class CStepper : public TimerHandler { //CStepper for "CSim Stepper"
	public:
		CStepper(CSim* t) : TimerHandler(), t_(t) {}
		inline virtual void expire(Event*);
	protected:
		CSim* t_;
};
				
				

class CSim : public TclObject {
	public:
		CSim();
		void   MyPrivFunc(void);	
		int command(int argc, const char*const* argv);
		void setNPlayers(long unsigned n);
		int startSimulation(long unsigned interval, char avatar_type = 3);
		int stopSimulation();
		void worldStep();
		double relevanceRelation(int playerA, int playerB);
		long unsigned step_interval;
		bool isRunning;
		long unsigned get_num_players() {
			return nPlayers;
		}
		long unsigned get_ww() {
			return ww;
		}
	private:
		int    my_var1;
		double my_var2;
		
		void	sayName(void);
		int 	myName;				
		long unsigned nPlayers, ww;
		
		Avatar* playerList;
		
		CStepper step_timer;
};







static class CSimClass : public TclClass {
	public:
		CSimClass() : TclClass("CosmmusSimulator") {}
		TclObject* create(int, const char*const*) {
			return(new CSim());
		}
} class_csim;



CSim::CSim() : TclObject(), step_timer(this), isRunning(false), step_interval(10), nPlayers(1000), ww(1000) {
	myName = rand();
	bind("my_var1_otcl", &my_var1);
	bind("my_var2_otcl", &my_var2);
	
	//step_timer.resched(1.0f);
}



int CSim::command(int argc, const char*const* argv) {
	if(argc == 2) {
		
		if(strcmp(argv[1], "call-my-priv-func") == 0) {
			MyPrivFunc();
			return(TCL_OK);
		}
		
		if(strcmp(argv[1], "iskedule") == 0) {
			step_timer.resched(0.2f);
			return(TCL_OK);
		}
		if(strcmp(argv[1], "stop") == 0) {
			stopSimulation();
			return(TCL_OK);
		}
		
	}
	
	if(argc == 3) {
		if(strcmp(argv[1], "call-brother") == 0) {
			CSim* bro = (CSim*) TclObject::lookup(argv[2]);
			sayName();
			bro->sayName();
			Tcl::instance().evalf("puts \"%s\"", argv[2]);
			return(TCL_OK);
		}
		if(strcmp(argv[1], "start") == 0) {
			step_interval = atoi(argv[2]);
			cout << "OK til here: " << step_interval << "ms interval." << endl;		
			startSimulation(step_interval);
			Tcl::instance().evalf("puts \"Simulation started with %ld ms step interval\"", step_interval);
			return(TCL_OK);
		}
		if(strcmp(argv[1], "set_num_players") == 0) {
			nPlayers = atoi(argv[2]);
			setNPlayers(nPlayers);
			Tcl::instance().evalf("puts \"Number of players set to %ld\"", nPlayers);
			return(TCL_OK);
		}
		if(strcmp(argv[1], "set_world_size") == 0) {
			ww = atoi(argv[2]);
			return(TCL_OK);
		}
	}
	
	if(argc == 5) {
		if(strcmp(argv[1], "start") == 0) {
			step_interval = atoi(argv[2]);
			char atype = atoi(argv[3]);			
			startSimulation(step_interval, atype);
			Tcl::instance().evalf("puts \"Simulation started with %ld ms step interval and %d avatar\"", step_interval, atype);
			return(TCL_OK);
		}
	}
	
	return(TclObject::command(argc, argv));
}



void CSim::MyPrivFunc(void) {
	Tcl& tcl = Tcl::instance();
	tcl.eval("puts \"Go, Eddie! Go! Go! Go!\"");
	tcl.evalf("puts \"     my_var1 = %d\"", my_var1);
	tcl.evalf("puts \"     my_var2 = %f\"", my_var2);
}



void CSim::sayName(void) {
	Tcl& tcl = Tcl::instance();
	tcl.evalf("puts \"My name is %d\"", myName);
}



//avatar_type: 0: EveryoneAvatar; 1: SquareAOI; 2: RoundAOI; 3: RoundSmooth; 4: SemicircleAOIAvatar 5: CosmmusAvatar; 6:CosmmusSmoothAOIAvatar
int CSim::startSimulation(long unsigned interval, char avatar_type) {
	if (isRunning)
		return 1;
	
	step_interval = interval;
	
	switch (avatar_type) {
		case 0: {
			playerList = new EveryoneAvatar[nPlayers];
			break;
		}
		case 1: {
			playerList = new SquareAOIAvatar[nPlayers];
			break;
		}
		case 2: {
			playerList = new RoundAOIAvatar[nPlayers];
			break;
		}
		case 3: {
			playerList = new RoundSmoothAOIAvatar[nPlayers];
			break;
		}
		case 4: {
			playerList = new SemicircleAOIAvatar[nPlayers];
			break;
		}
		case 5: {
			playerList = new CosmmusAvatar[nPlayers];
			break;
		}
		case 6: {
			playerList = new CosmmusSmoothAvatar[nPlayers];
			break;
		}
	}
	
	isRunning = true;
	
// 	cout << "OK til here (2): " << (unsigned short) avatar_type << endl;
	worldStep();
	return 0;
}



void CSim::worldStep() {
	
	for (int i = 0 ;  i < nPlayers ; i++) {
// 		cout << "Stepping player[" << i << "]" << endl;
		playerList[i].step(step_interval);
	}
	
// 	cout << "OK til here (3)" << endl;
	
	Tcl& tcl = Tcl::instance();
// 	tcl.evalf("puts \"   X = %f; Y = %f \"", playerList[0].GetX(), playerList[0].GetY());
	
	double next_sched = ((double)(step_interval))/1000.0f;	
	if (isRunning)	step_timer.resched(next_sched);
	

}



void CStepper::expire(Event*)
{
	t_->worldStep();
}



int CSim::stopSimulation() {
	isRunning = false;
}



void CSim::setNPlayers(long unsigned n) {
	nPlayers = n;
}



double CSim::relevanceRelation(int a, int b) {
	return playerList[a].OtherRelevance(&playerList[b]);
}
