//
// Author:    Jae Chung
// File:      mm-app.h
// Written:   07/17/99 (for ns-2.1b4a)
// Modifed:   10/14/01 (for ns-2.1b8a)
// 
		
#pragma once

#include "random.h"
#include "scheduler.h"
#include "timer-handler.h"
#include "packet.h"
#include "app.h"
#include "state_enum.h"
		
class PPSim;
class PPApp;


// The server uses this timer to
// check every 10ms wether it's time to
// send a new update packet of this player
// to the receiver.
class PPSendTimer : public TimerHandler {
public:
	PPSendTimer(PPApp* t) : TimerHandler(), t_(t) {}
	inline virtual void expire(Event*);
protected:
	PPApp* t_;
};


// P2pse Application Class Definition
class PPApp : public Application {
public:
	PPApp();
	void send_pp_pkt();  // called by SendTimer:expire (Sender)
	bool isIdle(); // unused... (yet)	
	double relevance(long unsigned int player, long unsigned int client);
	void check_send_sched();
	void check_player_update_schedule(long unsigned player, long unsigned client);
	void appChangeCB (pp_state from_state, pp_state to_state, long unsigned pl_id);
protected:	
	int command(int argc, const char*const* argv);
	void start();       // Start sending data packets (Sender)
	void stop();        // Stop sending data packets (Sender)
	void fireATAEvent(long unsigned pl_id);
	void fireATSEvent(long unsigned pl_id);
	void fireSTAEvent(long unsigned pl_id);
	void check_player_send_schedule();
	void check_server_send_schedule();
private:
	virtual void recv_msg(int nbytes, const char *msg = 0); // (Sender/Receiver)

	int pktsize_;          // Application data packet size
	int running_;          // If 1 application is running
	
	PPSim* pp_simulator;
	int player_number;
	double normal_interval;
	double pack_aggr_time;
	int max_rate;
	
	int packetGrouping;
	int acc;
	double last;
	
	long unsigned num_players;
	bool **isPending;	
	double **last_snd_time;
	double **next_snd_time;
	
	bool server;
	pp_state *mv_pstate;
				
	PPSendTimer snd_timer_;  // SendTimer	
};
