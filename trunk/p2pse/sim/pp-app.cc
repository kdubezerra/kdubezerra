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


#include <iostream>
#include "pp-app.h"
#include "pp-simulator.h"

using namespace std;

// MmApp OTcl linkage class
static class PPAppClass : public TclClass {
  public:
    PPAppClass() : TclClass("Application/P2pseApp") {}
    TclObject* create(int, const char*const*) {
      return (new PPApp);
    }
} class_pp_app;


// When snd_timer_ expires call MmApp:send_mm_pkt()
void PPSendTimer::expire(Event*)
{
  t_->check_send_sched();
}


// Constructor (also initialize instances of timers)
PPApp::PPApp() : running_(0), snd_timer_(this), last_snd_time(0), next_snd_time(0), isPending(0), packetGrouping(0), acc(0), last(0.0f), server(true), last_client_packet_time(0), additional_player_overhead_(0)
{
  bind("pktsize_", &pktsize_);  
  bind("interval_", &normal_interval);
  bind("packetGrouping_", &packetGrouping);
  bind("pack_aggr_time_", &pack_aggr_time);
}


// OTcl command interpreter
int PPApp::command(int argc, const char*const* argv)
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
    
    if (strcmp(argv[1], "set_additional_player_overhead") == 0) {
      additional_player_overhead_ = atoi(argv[2]);
      return(TCL_OK);
    }
  }

  if (argc == 5) {
    if (strcmp(argv[1], "attach-simulator") == 0) {
		  
      if (strcmp(argv[4], "serverapp") == 0) {		  
        pp_simulator = (PPSim*) TclObject::lookup(argv[2]);
        player_number = atoi(argv[3]);
        num_players = pp_simulator->get_num_players();
        pp_simulator->setServerApp(this);
        server = true;
        cout << "Set to server." << endl;
      }
		  
      if (strcmp(argv[4], "clientapp") == 0) {		  
        pp_simulator = (PPSim*) TclObject::lookup(argv[2]);
        player_number = atoi(argv[3]);
        num_players = pp_simulator->get_num_players();
        pp_simulator->setClientApp(this);
        server = false;
        cout << "Set to client." << endl;
      }
      cout << "Number of players = " << num_players << endl;
      isPending = new bool*[num_players];
      last_snd_time = new double*[num_players];
      next_snd_time = new double*[num_players];
      mv_pstate = new pp_state[num_players];
		  
      for (int player_i = 0 ; player_i < num_players ; player_i++) {
			  
        mv_pstate[player_i] = pp_simulator->getStatus(player_i);
			  
        isPending[player_i] = new bool[num_players];
        last_snd_time[player_i] = new double[num_players];
        next_snd_time[player_i] = new double[num_players];			  
			  					  
        for (int player_j = 0 ; player_j < num_players ; player_j++) {
          isPending[player_i][player_j] = false;				  
          last_snd_time[player_i][player_j] = ((double)(rand() % (int)(normal_interval*1000.0f))) / 1000.0f;
        }
			  
      }
		  
      return(TCL_OK);
    }
  }

  return (Application::command(argc, argv));
}


void PPApp::start()
{
  running_ = 1;
  check_send_sched();
}


void PPApp::stop()
{
  running_ = 0;
	
}


// Send application data packet
void PPApp::send_pp_pkt(int n_players_updates)
{
  
  int new_packet_size = pktsize_ + n_players_updates * additional_player_overhead_ ;

  double now = Scheduler::instance().clock();
	
  if (running_) { 
	  
    if(!packetGrouping) {			
      agent_->sendmsg(new_packet_size);
    }
    
    if (!n_players_updates) 
      cout << new_packet_size  << " bytes transferidos no instante " << now << endl;
		
//  else if (now - last >= pack_aggr_time || (acc + 2) * pktsize_ > agent_->size()) {
    else if (now - last >= pack_aggr_time || (acc + new_packet_size) > agent_->size()) {
      agent_->sendmsg(acc + new_packet_size);
      acc = 0;
      last = now;
    }
		
    else {
      acc += new_packet_size ;
    }

  }		
	
}


// Receive message from underlying agent
void PPApp::recv_msg(int nbytes, const char *msg)
{

}


bool PPApp::isIdle(void) {
  return (snd_timer_.status() == TIMER_IDLE);
}
		
		
double PPApp::relevance(long unsigned int dest_player, long unsigned int src_player) {
  return pp_simulator->relevanceRelation(dest_player, src_player);
}


void PPApp::check_send_sched() {

  if (server) {
    check_server_send_schedule();
  }
  else {
    check_player_send_schedule();
  }
	
  if (running_) snd_timer_.resched(0.001f);

}


void PPApp::check_player_send_schedule() {	
  double now = Scheduler::instance().clock();
	
  if (!last_client_packet_time) {
    last_client_packet_time = (double*)malloc(num_players * sizeof(double));
    for ( int i = 0 ; i < num_players ; i++) {
      last_client_packet_time[i] = ((double)(rand() % (int)(normal_interval*1000.0f))) / 1000.0f;
      cout << "last_client_packet_time[" << i << "] = " << last_client_packet_time[i] << endl;
    }
  }
	
  for( int player = 0 ; player < num_players ; player++ ){
    if (pp_simulator->getStatus(player) == action) continue;
    if (now - last_client_packet_time[player]  >= normal_interval) {
      send_pp_pkt();
		last_client_packet_time[player] = now;
//    last_client_packet_time[player] += normal_interval; //to avoid a propagation of the interval error due to the discrete event simulator
    }		
  }
	
}


void PPApp::check_server_send_schedule() {
  int n_updates;
  
  for (int src_player = 0 ; src_player < num_players ; src_player++) {
    
    if (pp_simulator->getStatus(src_player) == action) continue;
        
    n_updates = 0;
    
    for (int dest_player = 0 ; dest_player < num_players ; dest_player++)    {
//    if (src_player == dest_player) continue; //Theoretically, the player receives his own updated information      
      if (pp_simulator->getStatus(dest_player) == action) continue;
      if (check_player_update_schedule(dest_player, src_player))
        n_updates++;    
    }
    
    if (n_updates) send_pp_pkt(n_updates);
    
  }
}


bool PPApp::check_player_update_schedule(long unsigned dest_player, long unsigned src_player) {
  double rel_ = relevance(dest_player, src_player);
  double now = Scheduler::instance().clock();
  bool ret_value = false;
	
  if (rel_ > 0.001) {
    double new_snd_time = last_snd_time[src_player][dest_player] + normal_interval / rel_;
    if(!isPending[src_player][dest_player] || new_snd_time < next_snd_time[src_player][dest_player])
      next_snd_time[src_player][dest_player] = new_snd_time;
    isPending[src_player][dest_player] = true;
  }

  if (now > next_snd_time[src_player][dest_player] && isPending[src_player][dest_player]) {
    ret_value = true;
//    send_pp_pkt();
//    last_snd_time[src_player][dest_player] = now;
//    to avoid a propagation of the interval error due to the discrete event simulator:
    last_snd_time[src_player][dest_player] = now;
    isPending[src_player][dest_player] = false;
  }

  return ret_value;
  
}


void PPApp::appChangeCB (pp_state from_state, pp_state to_state, long unsigned pl_id) {
//   if (last_client_packet_time)
//     last_client_packet_time[pl_id] = Scheduler::instance().clock() - normal_interval;
//   for (int other = 0 ; other < num_players ; other++) {
//     isPending[pl_id][other] = false;
//     isPending[other][pl_id] = false;    
//   }
      
  if (from_state == action && to_state == action) {
    mv_pstate[pl_id] = action;
    fireATAEvent(pl_id);
  }
	
  if (from_state == action && to_state == social) {
    mv_pstate[pl_id] = social;
    fireATSEvent(pl_id);
  }
	
  if (from_state == social && to_state == action){
    mv_pstate[pl_id] = action;
    fireSTAEvent(pl_id);
  }
}

void PPApp::fireATAEvent(long unsigned pl_id) {
  if (server) {
// 		cout << "Server app received an ATA event" << endl;
		//... generate and send s->c messages related to this event
  }
  else {
// 		cout << "Client app received an ATA event" << endl;
		//... generate and send c->s messages related to this event
  }
}

void PPApp::fireATSEvent(long unsigned pl_id) {
  if (server) {
    if (pl_id == 0) cout << "Server app received an ATS event from player 0 in time " << Scheduler::instance().clock() << endl;
		//... generate and send s->c messages related to this event
  }
  else {
    if (pl_id == 0) cout << "Client app received an ATS event from player 0 in time " << Scheduler::instance().clock() << endl;
		//... generate and send c->s messages related to this event
  }
}

void PPApp::fireSTAEvent(long unsigned pl_id) {
  if (server) {
    if (pl_id == 0) cout << "Server app received an STA event from player 0 in time " << Scheduler::instance().clock() << endl;
		//... generate and send s->c messages related to this event
  }
  else {
    if (pl_id == 0) cout << "Client app received an STA event from player 0 in time " << Scheduler::instance().clock() << endl;
		//... generate and send c->s messages related to this event
  }
}