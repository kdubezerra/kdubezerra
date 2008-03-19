//
// Author:    Jae Chung
// File:      mm-app.h
// Written:   07/17/99 (for ns-2.1b4a)
// Modifed:   10/14/01 (for ns-2.1b8a)
// 

#pragma once
		
#include "timer-handler.h"
#include "packet.h"
#include "app.h"
#include "scheduler.h"
#include "random.h"
#include "cosmmus-simulator.h"

class CosmmusApp;


// The server uses this timer to
// check every 10ms wether it's time to
// send a new update packet of this player
// to the receiver.
class CSendTimer : public TimerHandler {
public:
	CSendTimer(CosmmusApp* t) : TimerHandler(), t_(t) {}
	inline virtual void expire(Event*);
protected:
	CosmmusApp* t_;
};


// Cosmmus Application Class Definition
class CosmmusApp : public Application {
public:
	CosmmusApp();
	void send_cosmmus_pkt();  // called by SendTimer:expire (Sender)
	bool isIdle(); // unused... (yet)
	double relevance();
	double relevance(long unsigned int player);
	void check_send_sched();
	void check_player_update_schedule(long unsigned player);
protected:	
	int command(int argc, const char*const* argv);
	void start();       // Start sending data packets (Sender)
	void stop();        // Stop sending data packets (Sender)
private:
	virtual void recv_msg(int nbytes, const char *msg = 0); // (Sender/Receiver)

	int pktsize_;          // Application data packet size
	int running_;          // If 1 application is running
	
	CSim* cosmmus_simulator;
	int player_number;
	double normal_interval;
	int max_rate;
	
	long unsigned num_players;
	bool* isPending;	
	double* last_snd_time;
	double* next_snd_time;
				
	CSendTimer snd_timer_;  // SendTimer	
};
