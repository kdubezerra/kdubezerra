#pragma once

#include "agent.h"
#include "timer-handler.h"

class PPAvatar;
class PPSim; //PPSim for "P2pse Simulator"
class PPApp;

				
				
class PPStepper : public TimerHandler { //PPStepper for "PPSim Stepper"
	public:
		PPStepper(PPSim* t) : TimerHandler(), t_(t) {}
		inline virtual void expire(Event*);
	protected:
		PPSim* t_;
};
				
				

class PPSim : public TclObject {
	public:
		
		PPSim();
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
		void print_pl_pos();
		void setServerApp (PPApp* pp_app_);
		void setClientApp (PPApp* pp_app_);
		void spaceChangeCB (pp_state from_state, pp_state to_state, long unsigned pl_id);
		pp_state getStatus (long unsigned player_id);
				
		bool isClientServer() {
			return isCS;
		}
		
		void setClientServer(bool isCS_) {
			isCS = isCS_;
		}
		
	private:
		
		int    my_var1;
		double my_var2;
		bool isCS, usesAOI;
		
		void	sayName(void);
		int 	myName;				
		long unsigned nPlayers, ww;
		
		PPAvatar* playerList;
		PPApp* pp_server_app;
		PPApp* pp_client_app;
		
		PPStepper step_timer;
		
};


static class PPSimClass : public TclClass {
	public:
		PPSimClass() : TclClass("P2pseSimulator") {}
		TclObject* create(int, const char*const*) {
			return(new PPSim());
		}
} class_ppsim;

