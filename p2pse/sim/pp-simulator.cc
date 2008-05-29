#include <stdio.h>
#include <string.h>
//#include "AvatarPack.h"
#include "pp-avatar.h"
#include "pp-simulator.h"
#include "pp-app.h"

PPSim::PPSim() : TclObject(), step_timer(this), isRunning(false), step_interval(10), nPlayers(1000), ww(1000), isCS(false), usesAOI(true) {
	//...
}



int PPSim::command(int argc, const char*const* argv) {
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
    if(strcmp(argv[1], "ppl") == 0) {
      print_pl_pos();
      return(TCL_OK);
    }
		
  }
	
  if(argc == 3) {
    if(strcmp(argv[1], "call-brother") == 0) {
      PPSim* bro = (PPSim*) TclObject::lookup(argv[2]);
      sayName();
      bro->sayName();
      Tcl::instance().evalf("puts \"%s\"", argv[2]);
      return(TCL_OK);
    }
    if(strcmp(argv[1], "start") == 0) {
      step_interval = atoi(argv[2]);
      md_step_interval = ((double)(step_interval))/1000.0f;
// 			cout << "OK til here: " << step_interval << "ms interval." << endl;		
      startSimulation(step_interval);
      Tcl::instance().evalf("puts \"Simulation started with %.5lf ms step interval\"", md_step_interval);
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
    if(strcmp(argv[1], "set_client_server") == 0) {			
      isCS = atoi(argv[2]) ? true : false;
      cout << boolalpha;
      cout << "isCS = " << isCS << endl;
      return(TCL_OK);
    }
    if(strcmp(argv[1], "set_uses_aoi") == 0) {			
      usesAOI = atoi(argv[2]) ? true : false;
      cout << "usesAOI = " << usesAOI << endl;
      return(TCL_OK);
    }
  }
	
  if(argc == 4) {
    if(strcmp(argv[1], "start") == 0) {
      step_interval = atoi(argv[2]);
      md_step_interval = ((double)(step_interval))/1000.0f;
      char atype = atoi(argv[3]);			
      startSimulation(step_interval, atype);
      Tcl::instance().evalf("puts \"Simulation started with %.5lf ms step interval and PPAvatar\"", md_step_interval);
      return(TCL_OK);
    }
  }
	
  return(TclObject::command(argc, argv));
}



void PPSim::MyPrivFunc(void) {
  Tcl& tcl = Tcl::instance();
  tcl.eval("puts \"Go, Eddie! Go! Go! Go!\"");
  tcl.evalf("puts \"     my_var1 = %d\"", my_var1);
  tcl.evalf("puts \"     my_var2 = %f\"", my_var2);
}



void PPSim::sayName(void) {
  Tcl& tcl = Tcl::instance();
  tcl.evalf("puts \"My name is %d\"", myName);
}



//avatar_type: 0: EveryoneAvatar; 1: SquareAOI; 2: RoundAOI; 3: RoundSmooth; 4: SemicircleAOIAvatar 5: CosmmusAvatar; 6:CosmmusSmoothAOIAvatar
int PPSim::startSimulation(long unsigned interval, char avatar_type) {
  if (isRunning)
    return 1;

  step_interval = interval;

/*	switch (avatar_type) {
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
// 			cout << "These avatar are round smooth AOI avatars" << endl;
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
	*/
  isRunning = true;
	
// 	cout << "OK til here (2): " << (unsigned short) avatar_type << endl;
  worldStep();
  return 0;
}



void PPSim::worldStep() {	
  for (int i = 0 ;  i < nPlayers ; i++) {
    playerList[i].step(step_interval);		
  }
	
   // 	double next_sched = ((double)(step_interval))/1000.0f;
  if (isRunning)	step_timer.resched(md_step_interval);
}



void PPStepper::expire(Event*)
{
  t_->worldStep();
}


int PPSim::stopSimulation() {
  isRunning = false;
}


void PPSim::setNPlayers(long unsigned n) {
  nPlayers = n;
  cout << "Avatar type is PPAvatar (CosmmusSmoothAvatar's child class); " << nPlayers << " avatars." << endl;
  playerList = new PPAvatar[nPlayers];

  for (int i = 0 ;  i < nPlayers ; i++) {
    playerList[i].setSimulator(this);
    playerList[i].setPlayerId(i);
//     cout << "Avatar " << i << " set its simulator to " << this << endl;
  }
}



double PPSim::relevanceRelation(int a, int b) {
  if (!usesAOI)
    return 1.0f;
  else
    return playerList[a].OtherRelevance(&playerList[b]);
}

void PPSim::print_pl_pos() {
  for (int i = 0 ; i < nPlayers ; i++) {
// 		cout << "X = " << playerList[i].GetX() << " ; Y = " << playerList[i].GetY() << endl;
  }
}

void PPSim::setServerApp (PPApp* pp_app_) {
  pp_server_app = pp_app_;
}

void PPSim::setClientApp (PPApp* pp_app_) {
  pp_client_app = pp_app_;
}

void PPSim::spaceChangeCB (pp_state from_state, pp_state to_state, long unsigned pl_id) {
//   cout << "Received state change" << endl;
  pp_server_app->appChangeCB (from_state, to_state, pl_id);
  pp_client_app->appChangeCB (from_state, to_state, pl_id);
}

pp_state PPSim::getStatus (long unsigned player_id) {
  return playerList[player_id].getStatus();
}
