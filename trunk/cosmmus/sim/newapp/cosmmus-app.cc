//  
// Author:    Jae Chung  
// File:      mm-app.cc
// Written:   07/17/99 (for ns-2.1b4a)  
// Modifed:   10/14/01 (for ns-2.1b8a)  
//   
//	Modified by: Carlos Eduardo Bezerra
//	File:	cosmmus-agent.h
//	Written:	03/18/2008			
// Last modified: 03/18/2008
		
#include "cosmmus-app.h"


// MmApp OTcl linkage class
static class CosmmusAppClass : public TclClass {
 public:
  CosmmusAppClass() : TclClass("Application/CosmmusApp") {}
  TclObject* create(int, const char*const*) {
    return (new CosmmusApp);
  }
} class_cosmmus_app;


// When snd_timer_ expires call MmApp:send_mm_pkt()
void CSendTimer::expire(Event*)
{
	t_->check_send_sched();
}


// Constructor (also initialize instances of timers)
CosmmusApp::CosmmusApp() : running_(0), snd_timer_(this), last_snd_time(0), next_snd_time(0), isPending(0)
{
  bind("pktsize_", &pktsize_);
  bind("interval_", &normal_interval);
}


// OTcl command interpreter
int CosmmusApp::command(int argc, const char*const* argv)
{
  Tcl& tcl = Tcl::instance();

  if (argc == 2) {
	  if (strcmp(argv[1], "start") == 0) {
		  start();
		  return(TCL_OK);
	  }
	  if (strcmp(argv[1], "stop") == 0) {
		  stop();
		  return(TCL_OK);
	  }
  }
  
  if (argc == 3) {
	  if (strcmp(argv[1], "attach-agent") == 0) {
		  agent_ = (Agent*) TclObject::lookup(argv[2]);
		  if (agent_ == 0) {
			  tcl.resultf("no such agent %s", argv[2]);
			  return(TCL_ERROR);
		  }
		  agent_->attachApp(this);
		  return(TCL_OK);
	  }

	  if (strcmp(argv[1], "set_full_rate") == 0) {
		  max_rate = atoi(argv[2]);
		  return(TCL_OK);
	  }
	  
	  if (strcmp(argv[1], "set_packet_size") == 0) {
		  pktsize_ = atoi(argv[2]);
		  return(TCL_OK);
	  }
  }

  if (argc == 4) {
	  if (strcmp(argv[1], "attach-simulator") == 0) {
		  cosmmus_simulator = (CSim*) TclObject::lookup(argv[2]);
		  player_number = atoi(argv[3]);
		  num_players = cosmmus_simulator->get_num_players();
		  cout << "Number of players = " << num_players << endl;
		  isPending = new bool[num_players];
		  last_snd_time = new double[num_players];
		  next_snd_time = new double[num_players];
		  
		  for (int player = 0 ; player < num_players ; player++) {
			  isPending[player] = false;
			  last_snd_time[player] = 0.0f;
		  }
		  
		  return(TCL_OK);
	  }
  }

  return (Application::command(argc, argv));
}


void CosmmusApp::start()
{
  running_ = 1;
  check_send_sched();
}


void CosmmusApp::stop()
{
	running_ = 0;
	
}


// Send application data packet
void CosmmusApp::send_cosmmus_pkt()
{
  if (running_)
    agent_->sendmsg(pktsize_);
}


// Receive message from underlying agent
void CosmmusApp::recv_msg(int nbytes, const char *msg)
{

}


bool CosmmusApp::isIdle(void) {
	return (snd_timer_.status() == TIMER_IDLE);
}
		
		
double CosmmusApp::relevance(void) {
	return cosmmus_simulator->relevanceRelation(0, player_number);
}


double CosmmusApp::relevance(long unsigned int player) {
	return cosmmus_simulator->relevanceRelation(0, player);
}


void CosmmusApp::check_send_sched() {

	for (int player = 1 ; player < num_players ; player++) {
		check_player_update_schedule(player);
	}
	
	if (running_) snd_timer_.resched(0.01f);

}


void CosmmusApp::check_player_update_schedule(long unsigned player) {
	double myRel = relevance(player);
	double now = Scheduler::instance().clock();	

	cout << "Relevance[" << player << "] is " << myRel << endl;	
	
	if (myRel > 0.001) {
		cout << "Now it's: " << now << ". Calculating new send time[" << player << "]: ";
		double new_snd_time = last_snd_time[player] + normal_interval / myRel;
		if(!isPending[player] || new_snd_time < next_snd_time[player])
			next_snd_time[player] = new_snd_time;
		isPending[player] = true;
		cout << next_snd_time[player] << endl;
	}

	if (now > next_snd_time[player] && isPending[player]) {
		cout << "Now sending a packet[" << player << "]" << endl;
		send_cosmmus_pkt();
		last_snd_time[player] = now;
		isPending[player] = false;
	}
	
	cout << endl;
}