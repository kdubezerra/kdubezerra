/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*

	LEETNET 2

	net server

*/

#ifndef _ZIG_HEADER_SERVER_H_
#define _ZIG_HEADER_SERVER_H_

#ifndef NO_DOXYGEN		// internal class -- do not document

#include "leetnet.h"
#include <nl.h>
#include "buffer.h"
#include "msocket.h"
#include <map>
#include <string>

// hardcoded client limit - client id's will always be assigned between ( 0 .. LIMIT-1 )
#define	HARDCODED_CLIENT_LIMIT 65536

//NEW v2.0: values for internal disconnected_code (NOT sent to disconnect() to avoid mixing up
// with app client's codes)
enum {

	// IMPORTANT: negative values are reserved for station_c::send_packet() error codes !!! (-1, -2, -3, -4, ...)

	// default reason code, meaning nothing. DON'T CHANGE THIS VALUE.
	LEETSERVER_DISCONNECTED = 0,      

	//disconnected by a socket error in the reader thread
	LEETSERVER_DISCONNECTED_TIMEOUT,

	//disc. because leet client couldn't parse a message from leet server
	LEETSERVER_DISCONNECTED_PROTOCOL_VIOLATION,

	//disconnected by a socket error in the reader thread
	LEETSERVER_DISCONNECTED_SOCKET_ERROR,

	// server is stopping
	LEETSERVER_DISCONNECTED_SHUTDOWN
};

// server behavior abstract class (interface)
// you must subclass this, and pass an instance to the server_c instance (replacement for stinky callbacks)
class server_behavior_i {
public:

	// implement this so the leetserver knows if it should accept or deny a connection
	// this can be used for a first quick authentication over the IP address of the client or, more commonly, 
	// to deny connections if the server is full or if the server is "locked" or something like that...
	// parameter "custom": custom connect data passed by the client (contains credentials maybe..?)
	// parameter "answer": an empty buffer, to be filled by the behavior with data to be answered to
	// the client. if the client is rejected and custom.code == 1, no answer packet will be sent (silent rejection)
	virtual bool leet_accept_client(NLaddress &client_addr, const buffer_c &custom, buffer_c &answer) = 0;

	// leetserver callback: a client has just been accepted by (connected to) the leetserver. 
	// client_id: the unique network identity of this connected client
	// custom: the same "custom connect data" that was passed to leet_accept_client() for this 
	//  same client. (see above) 
	virtual void leet_client_connected(int client_id, buffer_c &custom) = 0;

	// leetserver callback: a client has just disconnected or been disconnected by the leetserver
	// "code_reason": 0 if no special reason, nonzero otherwise (currenly only the negative-value 
	// error codes from station_c::send_packet() are propagated (as-is) through code_reason, but 
	// other values might be used in the future too)
	virtual void leet_client_disconnected(int client_id, int code_reason) = 0;

	// leetserver callback: incoming game packet from client
	virtual void leet_client_incoming_data(int client_id, buffer_c &in, int packet_id) = 0;

	// leetserver callback: somebody wants to get some server information (LEET_SERVERINFO message to
	// the server's public UDP requests port). remote_addr is the address of where the request came from.
	// "custom" contains extra request information that was on the request packet (may be empty).
	// "answer" is the buffer where the callback must write the answer to give to the client (it's already
	// empty). return false if should not answer anything (ignore it), return true to return the "answer".
	virtual bool leet_get_server_info(NLaddress &remote_addr, buffer_c &custom, buffer_c &answer) = 0;
	
	virtual ~server_behavior_i() {}
};

// remoteclient class: represents a view from the server side of one remote client
// ZIGLITE - NEW: remote_client_c receives all callback from station (incoming packets!)
class server_c;

class remote_client_c : public station_callback_i {
public:

	//reference to owner server
	server_c *server;

	//client's id (map key in server_c::clients)
	int id;
	
	// ZIGLITE new: ptr to station instance at the msocket
	station_c	*station;
	
	// the variables below encode the client's connection state
	//
	bool		told_disconnect;		// client already told that he wants to disconnect or, a "disconnect"
																			// packet was already received from the client
	bool		server_disconnected; // set to true when the server kicks the client. told_disconnect may
																			 // be false at that point.

	//a server_c::disconnect_client() method is running on a thread or has already been called
	//for this client.
	bool		disconnect_client;

	// ZIG 2.0.0 : removed -- all disconnection code moved to "station_c"! forgot to remove this logic
	//                        from server_c ...
	//
	// for the new "asynchronous disconnection" routine (when using ZIG_SERVER_THREADING_0)
	// time to send next disconnect packet
	//double disconnect_packets_next_time;
	// count of disconnection packets left to send
	//int disconnect_packets_left_count;
	// the disconnect packet
	//buffer_c disconnect_packet_cache;
	
	// time of last packet received by server from this client. to check client timeouts.
	double last_received_packet;

	// NEW (v1.3.0): store reason/error code to pass by leet_client_disconnected(). 0 = generic/unspecified reason
	int disconnection_reason_code;

	//ctor
	remote_client_c(server_c *svr, station_c *station_ptr, console_c *conio = 0) {
		server = svr;

		//ZIGLITE new
		station = station_ptr;

		station->set_console(conio);

		told_disconnect = false;
		server_disconnected = false;
		last_received_packet = get_time();
		disconnect_client = false;
		disconnection_reason_code = 0;
	}

	//dtor
	virtual ~remote_client_c();

	// incoming data read by a station from the msocket. it can either be "special" packets 
	// (packet_id == 0) or "regular"(=="normal") game packets (packet_id > 0 && packet_id <= 14000) 
	virtual void station_incoming_data(buffer_c &pkt, int packet_id);

	/// Connection handshake: remote station accepted connection
	/// @param connection_pkt: Accept packet sent by remote station (it may be also the request packet,
	/// when two stations connecting simultanealy
	virtual void station_connected(buffer_c& connection_pkt, const bool forced);

	/// Connection handshake: timed out connection attempts
	virtual void station_connection_timed_out();

	/// Connection handshake: remote station refused connection
	/// @param reject_pkt: Reject packet with (potential) rejection reason
	virtual void station_connection_refused(buffer_c& reject_pkt);

	/// Station has disconnected; "remote" is true if remote station has disconnected us. if 
	/// "remote" is false, then something locally has triggered the disconnection (e.g. local 
	/// call to station_c::disconnected() is the most probable cause).
	virtual void station_disconnected(bool remote, buffer_c& reason);
};

// leetserver class
class server_c : public msocket_callback_i {
public:

	friend class remote_client_c;

	//ctor, setting behavior. behavior will **NOT** be deleted by the server's destructor!
	server_c(server_behavior_i *beh, console_c *conio = 0);

	//dtor
	virtual ~server_c();

	/// configure (optional) port range for this server's UDP sockets. by default, no server port range is 
	/// specified and any opened UDP server sockets use a local port value automatically chosen by the OS. if 
	/// specified, UDP server sockets will always be opened in a port in range [minport, maxport] (inclusive) or 
	/// not opened at all (returning "connection failure" or similar error). 
	/// if either argument is <= 0 or > 65535 or if minport > maxport, the local port range is disabled. 
	void set_local_port_range(int minport, int maxport) { 
		if ((minport <= 0) || (minport > 65535) || (maxport <= 0) || (maxport >= 65535) || (minport > maxport)) {
			use_port_range = false;  // server port range configuration: disabled
			this->minport = 0;
			this->maxport = 0;
		} else {
			use_port_range = true; // server port range configuration: enabled
			this->minport = minport; 
			this->maxport = maxport;
		}
	}

	// configure usage of packet compression. to turn on the sending of compressed packets at any time, 
	// pass a value >=0 as argument, which specifies the minimum size that the packet must have, in bytes, 
	// to be compressed (pass 0 to compress all outgoing packets). to turn off the sending of compressed 
	// packets at any time, call this method with no parameters (or a negative value as parameter). 
	// NOTE: this call resets the compression stats.
	void set_compression(int minimum_packet_size = -1);

	// set the default max outgoing packet sizes for all new clients and also change the max outgoing 
	// packet sizes for all clients that are already connected
	void set_max_outgoing_packet_size(int max_packet_size) {
		cfg_max_packet_size = max_packet_size;
		// for all clients:
		std::map<int, remote_client_c*>::iterator it = clients.begin();
		while (it != clients.end()) {
			remote_client_c *cl = (*it).second;
			if (cl->station) {
				cl->station->set_max_outgoing_packet_size(max_packet_size);
			}
			it++;
		}
	}

	// customize the max outgoing packet sizes for a single client. if client_id is invalid, 
	// this method does nothing
	void set_max_outgoing_packet_size_for_client(int client_id, int max_packet_size) {
		remote_client_c *cl = get_client(client_id);
		if (cl)	
			if (cl->station) {
				cl->station->set_max_outgoing_packet_size(max_packet_size);
			}
	}

	// start server at port, with a maximum acceptable number of players. returns 0 if fails, 1 on success.
	int start(int port_num, int max_players, int broadcast_port = ZIG_DEFAULT_BROADCAST_PORT);

	// stops the server. parameter is the number of seconds to wait for finishing all stuff nicely.
	//void stop(int timeout = 3);
	void stop();

	// must call this method periodically to make server_c read all client sockets, check for 
	// client timeouts, etc. this method does nothing if the server is not started. 	
	// NOTE: this call processes ALL pending packets on the server socket.
	void poll_sockets();
  
	//set client timeout. client will be disconnected automatically if it does not send any packets
	//in the specified timeout in seconds.
	void set_client_timeout(int timeout = 30) { client_timeout = timeout; }

	//disconnects a specific client.
	//return: 0 fail (no such client?), 1 success.
	int disconnect_client(int client_id);

	//disconnects a specific client.
	//return: 0 fail (no such client?), 1 success.
	//additional parameter: custom disconnect data to be passed to the client.
	int disconnect_client(int client_id, buffer_c reason);

	//disconnects a specific client NOW! (only call client_disconnection_finalization()...)
	void immediate_disconnect_client(int client_id);

	//send a game frame to a given client. must be called regularly (~100ms), but there are no hard
	//requirements for the interval.
	//returns 0 on failure, 1 on success.
	//returns the allocated packet id in the optional packet_id pointer
	int send_frame(int client_id, buffer_c &framebuf, int *packet_id = 0);

	//sends the given reliable message to the given client. reliable = heavy, do not use for frequent
	//world update data. use for gamestate changes, talk messages and other stuff the client can't miss, or
	//stuff he can even miss but it's better if he doesn't and the message is so infrequent and small that
	//it's worth it.
	//returns 0 on failure, 1 on success
	int send_message(int client_id, buffer_c &message, int stream_id = -1, bool reliable = true);

	// broadcasts the given reliable message to all active clients. return: 0 fail, 1 success.
	// IMPORTANT!! VERY IMPORTANT!!
	// Uses the default stream of every client!!
	// if the client hasn't yet created a stream, it sends the broadcast by unreliable stream 0.
	int broadcast_message(buffer_c &message);

	//retrieve the next pending message from a client
	//if no more messages returns buffer.code == 0, else returns buffer.code == 1
	//returns buffer.code == -1 if client_id not found
	buffer_c receive_message(int client_id, int stream_id);

	//retrieve the next pending message from a client. returns 0 if no more messages, 1 if message read ok.
	//buffer paramenter: a pre-allocated buffer for putting the message in
	//returns -1 if client_id not found
	int receive_message(int client_id, buffer_c &msgbuf, int stream_id = -1);

	// NEW (ziglite 1.2): creates a stream for a client and returns its identifier (calls station_c method)
	int  create_stream(int client_id, int type = STREAM_CUMULATIVE_ACK, policy_c policy = DefaultPolicy);

	// NEW (ziglite 1.2): deletes a stream (calls station_c method)
	// returns true if success, false otherwise 
	bool delete_stream(int client_id, int stream_id = -1);

	// call get/set default stream id methods from station
	void set_default_stream_id(int client_id, int stream_id);
	int  get_default_stream_id(int client_id);

	// NEW (ziglite 1.2): seek the first stream of the client
	// return false if none
	bool seek_first_stream(int client_id);

	// NEW (ziglite 1.2): retrieves the NEXT stream of the client (after last stream query)
	// return -1 if last stream
	int get_next_stream(int client_id);

	//---- CALLBACKS (?) -------------------------------------------------

	//data just arrived in a client's "station"
	void incoming_client_data(remote_client_c *cl, buffer_c &in, int packet_id);

	//---- msocket_c callback interface impl -----------------------------

	// incoming packet from unregistered IP:PORT source. a typical implementation of this method 
	// would answer directly to "serverinfo"-like requests, and would also handle "new connection" 
	// requests by registering a new station_c socket for that IP:PORT.
	// parameters: remote address and UDP packet payload
	virtual void msocket_incoming_unreg(address_c &addr, const buffer_c &pkt);

	// got NL_INVALID while reading from the UDP socket. an use of this callback is to detect
	// NL_SOCK_DISCONNECT errors and, if a client, disconnect from the server or, if a server, 
	// try to find out which client socket died and remove it from the server.
	virtual void msocket_read_error(NLsocket &sock);

	// a remote station has called connect() towards us. return true to accept, false to reject.
	virtual bool msocket_accept_station(address_c& addr, const buffer_c& connect_data, buffer_c& reason);

	// called after msocket_accept_connection() returns true - a new station is created for 
	// the incoming connection request.
	// NOTE: the created station knows how to handle duplicate connection packets that can arrive 
	// after this call.
	virtual void msocket_station_connected(station_c& st, const buffer_c& oConnectionData);

	// msocket has closed: all station_c's have been deleted and current station_c pointers are 
	// now invalid.
	virtual void msocket_closed();

protected:

	//shared code like process_requests() : broadcastthread_thread_run() and ... poll_sockets() ? (yes)
	void process_broadcasts(int result, buffer_c pkt);

	//shared code -- timeout_thread_run() and poll_sockets() -- checks for client timeouts
	void process_timeouts();

	//shared code -- poll_thread_run() and poll_sockets() -- polls client sockets and processes any data arrived from clients
	NLint process_clients(int poll_timeout_ms);

	//just another obscure function
	void client_disconnection_finalization(remote_client_c *cl);

	//get client for id
	remote_client_c *get_client(int client_id);

	//effectively sends the message
	int send_message(remote_client_c *cl, buffer_c &message, int stream_id = -1, bool reliable = true);

	//delete a client
	void delete_client(remote_client_c *cl);

	//boolean state: server already start()'ed?
	bool started;

	//ports (NEW: UDP broadcast port for LAN server discovery)
	int port, broadcast_port;

	//maxplayers
	int maxplayers;

	//server behavior
	server_behavior_i *beh;

	// ZIGLITE: the socket holder
	msocket_c msocket;

	// ZIGLITE: direct pointer to the NLsocket inside msocket (only for nlWrite()!)
	NLsocket *sock; 

	//NEW (v1.5.0): thread/socket for reading LAN UDP BROADCAST requests
	NLsocket broadcast_sock;

	//clients map <client_id --> (client data record)>
	std::map<int, remote_client_c*>		clients;
	
	//client timeout, in seconds
	int client_timeout;

	//my local compression config flag. it is applied to all the new clients. -999 is a magic value that
	//specifies that the flag is not initialized yet (so let the default from station.h be used)
	int server_compression_config; // = -999; this value set at the constructor

	//server's local port range configuration
	bool use_port_range;
	int minport;
	int maxport;

	//NEW v1.5.0: client ID generator
	int client_id_generator;

	//debug console (may be 0 - no debug output)
	console_c		*con;

	//ZIGLITE 1.1
	int cfg_max_packet_size;

	//ZIGLITE 1.1: communication between msocket_accept_station() and msocket_station_connected()
	buffer_c msocket_accept_custom;
};

#endif // #ifndef NO_DOXYGEN		// internal class -- do not document

#endif

