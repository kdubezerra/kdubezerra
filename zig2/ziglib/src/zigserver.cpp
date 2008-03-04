/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, F�bio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*

	zigserver

	ZIG Game Engine Skeleton 
	server class

*/

#include "zig/zigserver.h"

#include <stdio.h>
using namespace std;

//auxiliary class for a <client-id --- > client> class
class zig_remote_client_c {
	public:

	zig_remote_client_c(int client_id) { id = client_id; reset(); }
	void reset() {

		//P1NG
		int i;for (i=0;i<4;i++)ping_id[i]=0;

		latency = 0;

		// ZIG 2.0: by default, it is server calling zigserver::leet_client_disconnected that informs 
		//  the zig reason for disconnection. but if this is != 0, then zigserver overrides what server says.
		zig_disconnect_reason_override = 0;
	}

	//ping id % 4 diz qual o indice em que ele vai
	NLushort ping_id[4];	// 0 se nenhum, id do ping se existente //P1NG
	double ping_time[4];	// tempo do send packet
	int latency;				// client's estimated latency in milliseconds

	//my ID!
	int id;

	//ZIG 2.0: we need this because, for example:
	// - zigserver calls server::disconnect_client() because of ZIG protocol violation
	// - however server::disconnect_client() doesn't take a "zig code-reason" -- it GIVES one 
	//   when calling back at zigserver::leet_client_disconnected()
	//
	//so if this code is not 0, it is taken into consideration by leet_client_disconnected(); it 
	// should be set before server::disconnect_client() is called
  int zig_disconnect_reason_override;
};

//===== zigserver_c method implementation =====

//common ctor code
void zigserver_c::common_inits() {
	running_this_destructor = false;
	server = 0;
	//set_shutdown_timeout();	//set default value
//	running_async = false;
	running_nonblocking = false;
		
	//my local compression config flag. it is applied to all the new clients. -999 is a magic value that
	//specifies that the flag is not initialized yet (so let the default from station.h be used)
	server_compression_config = -999; // magic value means "LEAVE AT DEFAULT"
	
	// v1.5.0: fixed noninited value usage: must set BOTH vars before calling timing_vars_changed_event(), 
	//  in order to make Valgrind happy
	net_ratio = 1;      //this is the FIX: default that will most likely not change. call below inits server_tick and all is well
	set_server_tick();	//100ms (0.1) should be the default value...
	set_net_ratio();		// still called to set to the REAL, documented default value (default argument value in the 
	                    // method declaration at zig/zigserver.h)
	
	set_client_timeout();	//set default value

	//ZIGLITE: server does not have a port range anymore
	//v1.5.0: default: no local port range for UDP sockets is specified
	//set_local_port_range(0,0);

	//v1.5.0: saves the last/greatest packet_id reported in an client_incoming_data() 
	// callback in order to be able to report late packets. a negative value means 
	// "no packet received/reported yet"
	greatest_incoming_packet_id = -1;

	//ZIGLITE 1.1: default max packet sizes
	cfg_max_packet_size = ZIG_DEFAULT_MAX_OUTGOING_PACKET_SIZE;
}

//ctor
zigserver_c::zigserver_c(console_c *conio) {
	running_this_destructor = false; // paranoia
	con = conio;
	common_inits();
}	

//dtor
zigserver_c::~zigserver_c() {

	running_this_destructor = true; // DO NOT call app

	if (running_nonblocking)
		stop();		//stop asynch thread AND THEN call shutdown(delete leetserver)
	else
		shutdown();	//just call shutdown(delete leetserver)
}

///ctor: no console; if you want to set a console later, call set_console(). messages generated by 
///this zigserver between the object construction and the call to set_console() will be sent to 
///the default global console (console_c::con)
//console_c console_c::defcon;
zigserver_c::zigserver_c() {
	running_this_destructor = false; // paranoia
  con = &(console_c::get_default_console());
	common_inits();
}

///set the console (works with the no-args ctor)
void zigserver_c::set_console(console_c *conio) {
	con = conio;
}

/// sets the server's logic update (and, indirectly, the network's) tick rate. the parameter 
/// is a real number for the interval, in seconds, between calls to the update_frame() method. 
/// NOTE: this method can only be called before start(). if it is called after the server 
/// is started, it does nothing.
void zigserver_c::set_server_tick(double tick) { 

	// do nothing if server is already started
	if (server != 0)
		return;
	
	server_tick = tick; 
	timing_vars_changed_event(); 
}

/// sets the server's update ticks to net ticks ratio. 
/// example: with set_server_tick(0.05), the server calls update_frame() at 20Hz. if 
/// set_net_ratio(1), then it calls broadcast_frame() at 20Hz (every 50ms) too, but with 
/// set_net_ratio(2), it calls broadcast at 10Hz (every 100ms), and with set_net_ratio(3), it calls 
/// at 6.66666666667Hz (every 150ms). 
/// NOTE: this method can only be called before start(). if it is called after the server 
/// is started, it does nothing.
void zigserver_c::set_net_ratio(int ratio) { 

	// do nothing if server is already started
	if (server != 0)
		return;

	net_ratio = ratio; 
	timing_vars_changed_event(); 
}

///retrieve the most-recently measured latency (the "ping time") in milliseconds between the
///server and a given client
int zigserver_c::get_client_latency(int client_id) {

	zig_remote_client_c *client = get_client(client_id);
	
	if (client == 0)
		return 0;

	return client->latency;
}

// used by set_server_tick & set_net_ratio
void zigserver_c::timing_vars_changed_event() {

	//recalculate the magic modulus number for implicit ping requests (this must be done on
	//the client also)
	// FIXED BUG (v1.2.4): the client was using the NET TICK of the server to calculate the
	// modulus, but the server was using it's SIMULATION TICK!
	//
	//   implicit_ping_modulus = (int)( ZIGNET_IMPLICIT_PING_MS / (tick * 1000.0) );
	//
	//the "tick" in the formula must be multiplied by the net ratio; the client was 
	//doing that!
	double ACTUAL_NET_TICK_INTERVAL_IN_SECONDS = server_tick * this->net_ratio;
	implicit_ping_modulus = (int)( ZIGNET_IMPLICIT_PING_MS / (ACTUAL_NET_TICK_INTERVAL_IN_SECONDS * 1000.0) );

//	con->printf("server's tick = %f ratio = %i\n", server_tick, net_ratio);
//	con->printf("server's implicit ping modulus = %i\n", implicit_ping_modulus);

	//tell all zigclients to change their net rates to match ours by creating a thread
	//for every request (hmmm.. ouch? nobody told you to change the net rate while the
	//server was running!)

	// *************************************************
  // NEW ZIGLITE:
	//  changing net-tick while clients connected will make the game blow up!
	//  IXME: make it throw an error OR forbid changing timing vars while 
	//         server running
	// *************************************************
	
	/*

	map<int, zig_remote_client_c*>::iterator it = clients.begin();
	while (it != clients.end()) {

		//create the thread (non joinable, detached)
		thread_c<zigserver_c> new_thread;
		if (new_thread.create(this, &zigserver_c::change_nettick_thread_run, false, (void *)it->second) == false) {
			//CAN'T create another thread. bad error.
			
			// IXME : resort to making the request right here, blocking the whole server but
			// it's better than nuke the process

			// IXME : OR at least just drop the client, don't nuke the whole server!

			FAILSTOP();
		}
		else {
			//inc misc count for this client -another thread to wait for before nuking this
			// zig_remote_client_c  instance!
			it->second->misc_threads_counter.inc();
		}
		

		it++;
	}

	*/
}

///starts the server in non-blocking mode. the caller thread returns immediatelly. the caller 
///thread may call stop() at a later time to terminate the server. returns false if there was an 
///error. 
///WHAT YOU MUST DO AFTER CALLING THIS METHOD: you must periodically call the process_nonblocking()
///method, or the server won't communicate. typically, you will do this inside your own "main loop".
///IMPORTANT: both the render_frame() and poll_input() methods WILL NOT BE CALLED! when running 
///non-blocking, it is YOUR responsibility to deal with the timing of polling input and rendering 
///to the screen (controlling frames-per-second).
bool zigserver_c::start(int port, int maxplayers, int broadcast_port) {

	//try to start
	// NEW (ZIG v1.2.1): hard-coded ZIG_SERVER_THREADING_0 -- no threads used at the server with run_nonblocking().
	if (!start_impl(port, maxplayers, broadcast_port)) {
		con->xprintf(2,"ZIGSERVER: run_non_blocking failed - can't start!\n");
		return false;
	}

	// set this so process_nonblocking() will "tick" when get_time() == ("now" + server_tick)
	process_nonblocking_last_tick_time = get_time();

  // init netskip for process_nonblocking()
  net_ratio_skipcount = 1;  //so when --, == 0 immediately

	//running nonblocking: returns true
	return (running_nonblocking = true);
}

//stops the server. call this function if you called run_async() or run_non_blocking(). 
//if running async, stops and joins the thread running the server. if running non blocking, 
//just makes sure that the server shuts down. takes some time, as it involves sending messages 
//and waiting responses. 'timeout' specifies how much time to wait before nuking the server,
//disregarding the shutdown network protocol for clients.
//NOTE: this is a FRONT-END to shutdown()...
bool zigserver_c::stop() {

	// check if has a successful run_async() or run_non_blocking() call pending to be stopped... if not, return false;
	if (!running_nonblocking) {
		con->xprintf(2,"zigserver_c::stop() : returning FALSE because of !running_async && !running_nonblocking\n");
		return false;
	}

	if (stop_called)		//stop allready called!
		return true;

	//set flag - server will exit ASAP
	stop_called = true;

	bool return_code;

	if (running_nonblocking) {
		// IF RUNNING NONBLOCKING: call shutdown which MIGHT have already been called by process_nonblocking(). we
		// return FALSE here if shutdown returns FALSE on us also (so the app may know if it's call to stop() was
		// useful or process_nonblocking() had already shutdown() things.
		return_code = shutdown();
	}
	else
		FAILSTOP();  // paranoia

	//not running async nor nonblocking anymore
	running_nonblocking = false;

	//ok
	return return_code;  // new: if running nonblocking, might return false here
}

/// runs timing logic to decide whether it's time to call back the application's update_frame() or 
/// broadcast_frame() callbacks.
/// IMPORTANT: you MUST call this method periodically (on your own code's "main loop" probably) or 
/// the server won't communicate, that is, your app's "update_frame()" and "broadcast_frame()" methods 
/// will not get called at all. 
/// RETURN: 'false' if server was or is stopped or if an error occurred on reading, 'true' otherwise. 
bool zigserver_c::process_nonblocking(){

	//con->xprintf(0, "zigserver_c::process_nonblocking init\n");

	//return if not running in nonblocking mode yet
	if (running_nonblocking == false)
		return false;

  // NEW: this method is to be ONLY EVER CALLED if running with run_non_blocking()
	// so we dumped:
	//  - sleep/yield timer code
	//  - checks for nonblocking
	//  - NEW: NO CALLS TO poll_input()
	//  - NEW: NO CALLS TO render_frame()

  // -----------------------------------------------------------
	// NEW in ZIG v1.2.1: process_nonblocking() polls the client's 
	// sockets and does ALL the reading, so we don't have an extra
	// thread allocated just for this (which causes threading bugs
	// since ZIG isn't synchronizing "incoming packet" callbacks 
	// with the rest (game main loops, ...)
	// -----------------------------------------------------------
	if (server) {

		//this does:
		//1- read and process server requests (connect, ...)
		//2- read and process data from connected clients
		//3- check for client timeouts
		server->poll_sockets();
	}

	// aux var for app callback results
  bool result;

	//check if it's a tick time
	double now = get_time();
	if ((now - process_nonblocking_last_tick_time) >= server_tick) {

		// FIXED: save so next tick time occurs only according to the server tick
		//process_nonblocking_last_tick_time = now;
		// NEW: forces server to try hard to keep up with the configured frame rate, if lags in one
		//   frame, will try to compensate in the next
		process_nonblocking_last_tick_time += server_tick;
		// BUT: special case: if the lagging tick time delta is TOO great, then cap it. the goal of 
		//  the "frame lagging" thing is to adjust for small fluctuations in the update intervals (1/10th of
		//  server_tick or so) in order to keep the actual update frequency equal to the configured update 
		//  frequency -- not to deal with large frame lags.
    if ((now - process_nonblocking_last_tick_time) > server_tick * 20.0) //twenty frames behind - that's a lot of lag
			process_nonblocking_last_tick_time = now;		// just reset the timer and start ticking normally again

		// tick time!
		// update_frame() and, depending on the "net ratio", broadcast_frame()

		//simulate frame
		result = update_frame();
		if (result == false) {
			// shutdown server (REVIEW: what about the shutdown() return code?)
			shutdown();
			// not running nonblocking anymore -- it's over
			// if you call stop() there is nothing to do anymore anyways
			running_nonblocking = false;
			// return false, stuff stopped
			return false;
		}

		//dec net ratio skip count
		net_ratio_skipcount--;

		//if reached 0, broadcast and reset to ratio value
		if (net_ratio_skipcount == 0) {

			result = broadcast_frame();
			if (result == false) {
				// shutdown server (REVIEW: what about the shutdown() return code?)
				shutdown();
				// not running nonblocking anymore -- it's over
				// if you call stop() there is nothing to do anymore anyways
				running_nonblocking = false;
				// return false, stuff stopped
				return false;
			}
	
			//reset skipcount value
			net_ratio_skipcount = net_ratio;

		}//net skip==0
  }//tick time==true

	// return true because everything went fine (still running)
	return true;
}

//PROTECTED - start up the server. port = game port
bool zigserver_c::start_impl(int port, int maxplayers, int broadcast_port) {

	con->xprintf(2,"zigserver::start_impl(port %i maxp %i broadcast_port %i)\n", port, maxplayers, broadcast_port);

	//has server...
	if (server != 0) {
		con->xprintf(2,"zigserver::start : SERVER ALREADY STARTED\n");
		return false;
	}

	//PARANOID check... would not be true while server==0...
	assert( running_nonblocking == false );
	if (running_nonblocking) {
		con->xprintf(2,"zigserver::start : running_nonblocking == TRUE!!!!\n");
		return false;
	}

	//ZIGLITE: moved up here so gameserver::init() may call set_net_ratio() / set_server_tick() 
	//         (the "server" server_c pointer is now used to allow or disallow tick/ratio change)
	//
	// try to init app
	bool result = init();
	if (result == false) {
		con->xprintf(2,"zigserver::start : app failed to init()ialize!\n");
		// failed on initialisation...
		return false;
	}

	//create the leetnet server
	//the "NEW behavior" object is deleted when the server object is deleted
	server = new server_c( this , con );

	//ZIGLITE: server does not have a port range anymore
	// v1.5.0: use server's local port range for UDP sockets
	//server->set_local_port_range(minport, maxport);

	if (server_compression_config != -999) //magic value means "LEAVE AT DEFAULT"
		server->set_compression( server_compression_config );

	//pass configuration parameters
	server->set_client_timeout( client_timeout );

	//start net server. if error, quit
	if (!server->start(port, maxplayers, broadcast_port)) {

		//delete leetserver
		SAFEDELETE(server);		
		con->xprintf(2,"zigserver::start : can't start leetserver!\n");

		return false;
	}

	//not called yet!
	stop_called = false;
	
	return true;
}

//PROTECTED - shut down the server. this function takes some time, as it involves sending messages and
//waiting responses.
bool zigserver_c::shutdown() {

	if (server == 0)
		return false;

	//stop the leetnet server
	//server->stop(shutdown_timeout);
	server->stop();

	//app: finish
	finish();

	//delete/finish
	SAFEDELETE(server);

	return true;
}

//PROTECTED -- find client
zig_remote_client_c *zigserver_c::get_client(int client_id) {

	zig_remote_client_c *client = 0;

	map<int, zig_remote_client_c *>::iterator it = clients.find(client_id);
	if (it != clients.end())
		client = it->second;

	return client;
}

//"PRIVATE" -- leetserver calls to know if it accepts or denies a connection.
bool zigserver_c::leet_accept_client(NLaddress &client_addr, const buffer_c &custom, buffer_c &answer) {

	con->xprintf(0, "custom.size = %i, answer.size = %i\n", custom.size(), answer.size());
	try {
		buffer_c::iterator it = custom.begin();

		//get ZIG VERSION
		NLubyte zigversion = it.getByte();

		con->xprintf(0, "byte = %i, zignet = %i\n", (int) zigversion, ZIG_NET_VERSION);

		//if wrong version, deny at ZIG level (don't even show it to the gameserver)
		if (zigversion != ZIG_NET_VERSION) {

			// this is not needed : the "return false" below puts a LEET_REJECT header on 
			// the outgoing answer packet...
			//answer.putByte( (NLubyte) ZIGNET_CONNECTION_REJECTED );		//rejected

			answer.putByte( (NLubyte) ZIG_NET_VERSION );							//our version is this one
			answer.putByte( (NLubyte) ZIGNET_REJECT_VERSION );				//by version
			return false;
		}

		// if things reach here, then the client is O.K. to this zigserver. BUT we still
		// have to check with the GAME-SERVER if this client is O.K. or not.
		//
		//   WHAT WE DO NOW: we want to know if the gameserver is really going to
		//   accept this client so we give him the custom buffer with the read cursor where
		//   it is now, and we give him a BLANK answer buffer to write into.

		buffer_c game_answer;

		//what the hell, let's do this right - passing a new custom buffer just with the
		//data that matters for the accept_client callback.
		buffer_c game_custom;
		int ansSize = custom.size_left() - sizeof(NLubyte);

		if (ansSize > 0) {
			char* block = new char[ansSize];
			it.getBlock(block, ansSize);
			game_custom.putBlock(block, ansSize);  //put on custom the data on the current pkt pos
			game_custom.seek(0);			//keep custom ready to be parsed
			delete[] block;
		}

		con->xprintf(0, "custom.size_left = %i, game_custom.size = %i\n", custom.size_left(), game_custom.size());
		address_c addr_wrapper(client_addr);

		bool game_accepts = accept_client(addr_wrapper, game_custom, game_answer);

		//THEN: if gameserver accepted, then we stuff in the answer the zignet accept
		// stuff, plus gameserver's answer. if not, we stuf the zignet's reject stuff,
		// plus the gameserver's answer;

		//this is a standard part of the response (either if accepted or if rejected)
		//  - our version is this one (just for completeness' sake):
		answer.putByte( (NLubyte) ZIG_NET_VERSION );

		if (game_accepts == true) {

			con->xprintf(0, "gameserver accepted client!\n");

			//add server nettick in short milliseconds
			//answer.putShort( ((NLushort)( this->server_tick * 1000.0 )) );	//net tick in millis

			//v1.3.3 fix rounding bug
			int rounded;
			rounded = droundi( this->server_tick * net_ratio * 1000.0 );

			//NEW: first add the TIMER TICK * THE NET SKIP RATIO = NEW formula for server's net rate
			answer.putShort( ((NLushort)( rounded )) );	//net tick in millis

			rounded = droundi( this->server_tick * 1000.0 );

			//NEW: now add the TIMER TICK, which is the SIM RATIO of the server
			answer.putShort( ((NLushort)( rounded )) );	//sim tick in millis
		}
		else {

			con->xprintf(0, "gameserver rejected client!\n");
			//zignet reason: game server didn't like you.. check the rest of the packet for
			//a custom reason block from the gameserver.
			answer.putByte( (NLubyte) ZIGNET_REJECT_GAMESERVER );   
		}

		con->xprintf(0, "game_answer.size = %i\n", game_answer.size());

		//put additional accept/reject gameserver stuff
		if (game_answer.size() > 0)
			answer.putBlock( game_answer.data(), game_answer.size() );

		//return wether the gameserver accepted you or not (well, if it depended just on ye olde
		//zig I would accept you, but you know those pesky gameservers...)
		return game_accepts;

	} catch (int ex) {
		
		con->xprintf(2,"EXCEPTION zigserver_c::leet_accept_client = %i\n", ex);
		return false;	//can't accept wrong packets
	}
}

//leetserver callback: a client has just been accepted by (connected to) the leetserver
//NEW (v1.3.4): "custom" is the same custom data that was passed through the accept_client() callback
void zigserver_c::leet_client_connected(int client_id, buffer_c &custom) { 

	//create client
	zig_remote_client_c *client = new zig_remote_client_c(client_id);
	
	//add to map.
	clients[ client_id ] = client;

	//  in reality, doesn't; the net tick is sent on leet_accept_connection(), so it MAY
	//  have already changed.
	//  we have no choice but to send AGAIN the net tick to the newly connected client
	//  F I X M E: may optimize later: if no calls to zigserver->set_net_tick() between this
	//   connected call and the LAST leet_accept call (whichever one), then we update
	//   the net tick here (won't cover all cases but will avoid stupid messages if the
	//   net rate stays the same thru all of the server's life
	//
	//clients[ client_id ]->clients_net_tick = this->server_tick;	//client KNOWS the right tick

	//tell gameserver
	//NEW (v1.3.4): the "custom" data buffer is (hopefully) exactly the SAME buffer object in
	// the same state of when it was passed to the gameserver in the accept_client() callback for
	// this same client
	client_connected(client_id, custom);
}


//leetserver callback: a client has just disconnected or been disconnected by the leetserver
void zigserver_c::leet_client_disconnected(int client_id, int code_reason) { 

	//v1.3.0: translate code_reason from leet/station into ZIGNET codes
	int zignet_code_reason = ZIGSERVER_DISCONNECTED;  //the default, catch-all value

	//ZIG 2.0: WAS NOT USING THE code_reason PARAMETER! 
	// if it is zero from server_c, will return ZIGSERVER_DISCONNECTED. OK, we were doing that.
	// if it is a negative value, then it is a SENDPACKET_xxx value (currently only -1 exists = socket error)
	// if it is a positive value, it was generated by server_c (must define at enum: LEETSERVER_DISCONNECTED_xxx ...)
  //
	if (code_reason == 0) { // same as "LEETSERVER_DISCONNECTED"
		// nop, already set to ZIGSERVER_DISCONNECTED above.
	}
	else if (code_reason < 0) {
		// SENDPACKET code -- ZIG 2 (NEW): probably never used since sendpacket codes are already translated 
		//  to LEETSERVER_DISCONNECTED_SOCKET_ERROR by the server_c
		if (code_reason == SENDPACKET_SOCKET_ERROR)
			zignet_code_reason = ZIGSERVER_DISCONNECTED_SOCKET_ERROR;
		else {
			// else NOP, leave at the general ZIGSERVER_DISCONNECTED code value.
		}
	}
	else {
		// positive value (>0), it is a LEETSERVER_DISCONNECTED_xxx value then

		// this chain of IFs is just lame... someone has a better idea?

		if (code_reason == LEETSERVER_DISCONNECTED_TIMEOUT) {
			zignet_code_reason = ZIGSERVER_DISCONNECTED_TIMEOUT;
		}
		else if (code_reason == LEETSERVER_DISCONNECTED_PROTOCOL_VIOLATION) {
			zignet_code_reason = ZIGSERVER_DISCONNECTED_PROTOCOL_VIOLATION;
		}
		else if (code_reason == LEETSERVER_DISCONNECTED_SOCKET_ERROR) {
			zignet_code_reason = ZIGSERVER_DISCONNECTED_SOCKET_ERROR;				
		}
		else if (code_reason == LEETSERVER_DISCONNECTED_SHUTDOWN) {
			zignet_code_reason = ZIGSERVER_DISCONNECTED_SHUTDOWN;
		}
		else {
			// else NOP, leave at the general ZIGSERVER_DISCONNECTED code value.
		}
	}

	// ==============================================
	// ZIG 2.0 NEW -- Check for overriding value:
	//   cancels any assignments made above (cancels what server_c reports, 
	//   if zigserver_c knows better)
	// THIS is not the best way to implement this, but it is quick
	// =============================================
	//get client
	zig_remote_client_c *cl = get_client(client_id);
	assert(cl != 0);
	if (cl->zig_disconnect_reason_override != 0) {
		// override!
		zignet_code_reason = cl->zig_disconnect_reason_override;
	}
			
	// BUG FIX v1.3.1: DO NOT call back the application if the derived class ("game server" class) has 
	// already been destroyed!
	if (running_this_destructor == false) {

		//tell gameserver (v1.3.0 NEW: pass on the ZIGSERVER_DISCONNECTED_* reason to app)
		client_disconnected(client_id, zignet_code_reason);  
	}

	//find client
	zig_remote_client_c *client = get_client(client_id);

	//remove from map
	map<int, zig_remote_client_c*>::iterator it = clients.find( client_id );
	if (it == clients.end())	// MUST be there
		FAILSTOP();
	clients.erase(it);

	//DELETE IT
	SAFEDELETE(client);
}

//leetserver callback: incoming game packet from client
void zigserver_c::leet_client_incoming_data(int client_id, buffer_c &in, int packet_id) { 

	//we must strip out any information from the unreliable block that the zigclient
	//set to the zigserver(this), so it doesn't go to the gameserver.

	buffer_c gamein;		//what the gameserver will be getting
		
	//if there is data on in... //P1NG BEGIN
	if (in.size() > 0) {

		// ZIG 2.0 (and earlier, to some point): The FIRST byte of the packet's unreliable block, from zigclient to 
		//   zigserver, is a bitfield. currently, the first bit is present to signal (following, on the unreliable 
		//   block) an answer to an "implicit ping" from the zigserver. the other 7 bits are empty and can be used 
		//   by zigclient/zigserver to implement other stuff.
		//
		//first byte is a special byte
		NLubyte zigbyte = in.getByte();

		//if bit0 == 1, then get implicit ping id and client delta
		if (zigbyte & 1) {

			//con->printf("ZIG-SERVER receives (zigbyte & 1) ping answer\n");

			// ZIGLITE 1.1: avoid malformed packets with too little data
			if (in.size_left() >= static_cast<int>(2 * sizeof(NLushort)) ) {

				//con->printf("..... AND IT HAS sv_ping_id / client_delta_ms\n");	

				NLushort sv_ping_id = in.getShort();
				
				// avoid warning (warning: unused variable �client_delta_ms�)
				//NLushort client_delta_ms = in.getShort();
				in.getShort();

				//get client
				zig_remote_client_c *client = get_client(client_id);
				assert(client != 0);

				//ZIG v1.4.0: id % 4 doesn't work!! new ping id index
				int piid = (sv_ping_id / implicit_ping_modulus) % 4;   // was:  (sv_ping_id % 4)

				//debug
				//con->printf("CHECKING PING:\n");
				//con->printf("id = %i, implicit =%i, piid = %i, value = %i\n", sv_ping_id, implicit_ping_modulus, piid, client->ping_id[ piid ]);

				//update the client's ping data. test if the server is still waiting for it
				if (client->ping_id[ piid ] == sv_ping_id) {

					//update!
					client->ping_id[ piid ] = 0;

					double pingz =
					(
						(
							(
								get_time() - client->ping_time[ piid ]
							) 
							* 
							1000.0
						) 
					);

					client->latency = (int)pingz;
				
					//con->printf("...PING(ms) of CLIENT %i = %f\n", client->id, pingz);
					//make the darn callback to gameserver!
					client_ping_result(client_id, client->latency);
				}
			}
			else {
				// malformed packet found (with too little data)
				// disconnect client

				//get client
				zig_remote_client_c *client = get_client(client_id);
				assert(client != 0);

				// ZIG 2.0 FIXED: sending reason to zigclient when server disconnects and calls zigserver back
				//  --> probably would work best if it was a parameter for server_c::disconnect_client().. but.. meh!
				client->zig_disconnect_reason_override = ZIGSERVER_DISCONNECTED_PROTOCOL_VIOLATION;

				disconnect_client(client_id);
			}
		}
//P1NG END
		//stuff the rest in the answer
		gamein.putBlock( in.data_cur(), in.size_left() );
	}//P1NG END
	
	//tell gameserver
	gamein.seek(0);

	//tell app of incoming packet
	client_incoming_data(client_id, gamein, packet_id);
}

// leetserver callback: somebody wants to get some server information... (see server.h for details)
bool zigserver_c::leet_get_server_info(NLaddress &remote_addr, buffer_c &custom, buffer_c &answer) { 

	address_c addr(remote_addr);
	bool result = get_server_info(addr, custom, answer); 
	return result;
}


///flush a game packet to a client. returns true on success;
// THIS WAS called send_frame() before ZIG 2.0!!!
bool zigserver_c::send_packet(int client_id, buffer_c &framebuf, int *packet_id) { 

	con->xprintf(0, "zigserver_c::send_frame init\n");

	//v1.5.0: bugfix - test if client exists first
	//get client
	zig_remote_client_c *client = get_client(client_id);
	if (!client)
		return false;

	bool r = false; 
	
	if (server) {

		//we're interested on the packet id - "implicit ping"
		int zig_packet_id;

		//send frame;
		r = (server->send_frame(client_id, framebuf, &zig_packet_id) != 0); 

		// P1NG BEGIN /*
		//if packet_id conforms to some magic rule agreed by client and server, then it's 
		// an implicit ping!
		if ( (zig_packet_id % implicit_ping_modulus) == 0) {

			//ZIG v1.4.0: id % 4 doesn't work!! new ping id index
			int piid = (zig_packet_id / implicit_ping_modulus) % 4;   // was:  (zig_packet_id % 4)

			//save time of send of the implicit ping
			client->ping_id[ piid ] = zig_packet_id;
			client->ping_time[ piid ] = get_time();
			
			//debug
			//con->printf("ZIG-SERVER SAVING PING:\n");
			//con->printf("...id = %i, implicit = %i, piid = %i\n", zig_packet_id, implicit_ping_modulus, piid);
		}
		// P1NG END */

		//return packet id
		if (packet_id != 0)
			(*packet_id) = zig_packet_id;
	}

	return r; 
}

