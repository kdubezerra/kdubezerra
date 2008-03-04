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

	net client

*/

#ifndef _ZIG_HEADER_CLIENT_H_
#define _ZIG_HEADER_CLIENT_H_

#ifndef NO_DOXYGEN		// internal class -- do not document

#include "leetnet.h"
#include "station.h"
#include "console.h"
#include "address.h"
#include "msocket.h"

//NEW v1.3.0: values for internal_disconnected_code_reason (NOT sent to disconnect() to avoid mixing up
// with app client's codes)
enum {

	// IMPORTANT: negative values are reserved for station_c::send_packet() error codes !!! (-1, -2, -3, -4, ...)

	// default reason code, meaning nothing. DON'T CHANGE THIS VALUE.
	LEETCLIENT_DISCONNECTED = 0,      

	//disconnected by a socket error in the reader thread
	LEETCLIENT_DISCONNECTED_SOCKET_ERROR,

	//disc. because leet client couldn't parse a message from leet server
	LEETCLIENT_DISCONNECTED_PROTOCOL_VIOLATION
};

//client behavior (replacement for callbacks)
class client_behavior_i {
public:

	//client has connected sucessfully, custom accept packet data passed
	virtual void leet_connected(buffer_c &hello) = 0;

	//connection attempt timed out
	virtual void leet_connection_timed_out() = 0;

	//connection refused, custom reason data passed from the server.
	virtual void leet_connection_refused(buffer_c &reason) = 0;

	//client has been disconnected. if disconnection was initiated by the server, then server_initiated
	//will be == true, and "reason" may contain custom disconnect data passed by the server (or not).
	//if disconnection was client-initiated, then "reason" is alwats empty.
	//code_reason is a custom integer passed to connect(false, ...);
	//internal_reason is one of the LEET_DISCONNECTED_xxx values, where 0 means "no special reason"
	virtual void leet_disconnected(bool server_initiated, buffer_c &reason, int code_reason, int internal_reason) = 0;

	//incoming data from server! passes the data and the packet id (may be of interest)
	virtual void leet_incoming_data(buffer_c &in, int packet_id) = 0;

	virtual ~client_behavior_i() {}
};

//possible values for client_c's int connect_state var.
enum {
	CCS_DISCONNECTED,	//client is not connected
	CCS_CONNECTING,  	//client is disconnected, trying connection
	CCS_CONNECTED    	//client is connected
};

//leetnet2 client class
class client_c : public msocket_callback_i, public station_callback_i {
public:

	//ctor, setting behavior. behavior will **NOT** be deleted by the client's destructor!
	client_c(client_behavior_i *beh, console_c *conio = 0);

	//dtor
	virtual ~client_c();

	/// configure (optional) port range for this client's UDP sockets. by default, no client port range is 
	/// specified and any opened client sockets use a local port value automatically chosen by the OS. if 
	/// specified, client sockets will always be opened in a port in range [minport, maxport] (inclusive) or 
	/// not opened at all (returning "connection failure" or similar error). 
	/// if either argument is <= 0 or > 65535 or if minport > maxport, the local port range is disabled. 
	void set_local_port_range(int minport, int maxport) { 
		if ((minport <= 0) || (minport > 65535) || (maxport <= 0) || (maxport >= 65535) || (minport > maxport)) {
			use_port_range = false;  // client port range configuration: disabled
			this->minport = 0;
			this->maxport = 0;
		} else {
			use_port_range = true; // client port range configuration: enabled
			this->minport = minport; 
			this->maxport = maxport;
		}
	}

	// configure the amount of time, in seconds, that the client will keep trying to
	// connect to the server, before giving up. the default parameter is the default 
	// timeout value.
	void set_connect_timeout(int timeout = 4) { connect_to_server_timeout = timeout; }

	// configure the max recommended & max absolute outgoing packet sizes
	void set_max_outgoing_packet_size(int max_packet_size) {
		cfg_max_packet_size = max_packet_size;
		if (station) {
			station->set_max_outgoing_packet_size(max_packet_size);
		}
	}


	// when socket polling mode is enabled, must call this periodically to make ZIG read the client 
	// socket and call the data callbacks (leet_incoming_data(), ...). this method does nothing if 
	// the client is not connected. 
	void poll_socket();

	//set minimum send reliables interval. set this to the server's net tick time, or
	// some other value, or just leave on default (0.0)
	void set_min_send_reliables_interval(double interval = 0.0) {
		min_send_reliables_interval = interval;
	}

	// configure usage of packet compression. to turn on the sending of compressed packets at any time, 
	// pass a value >=0 as argument, which specifies the minimum size that the packet must have, in bytes, 
	// to be compressed (pass 0 to compress all outgoing packets). to turn off the sending of compressed 
	// packets at any time, call this method with no parameters (or a negative value as parameter). 
	// NOTE: this call resets the compression stats.
	void set_compression(int minimum_packet_size = -1) { 
		compression_config_value = minimum_packet_size;
		if (station) {
			station->set_compression(compression_config_value);
		}
	}

	double get_loss_estimative() const { if (station) return station->get_loss_estimative(); else return -1.0; }

	double get_delay_estimative() const { if (station) return station->get_delay_estimative(); else return -1.0; }

	double get_rtt_estimative() const { if (station) return station->get_rtt_estimative(); else return -1.0; }

	//reset object state -- make it ready to connect again
	void reset_state();

	//set the server's address. call before connect() - address must be a RESOLVED IP!
	//returns true of ok, false if there is already a connection.
	bool set_server_address(char *address);

	//set the custom data sent with every connection packet
	void set_connect_data(buffer_c &data);

	// connects with remote server.
	//returns false if there was some internal error and the request couldn't be
	//  completed (this is a BAD error).
	bool connect();

	//disconnect from server. sends disconnection packets, etc. with disconnection reason as an optional parameter
	void disconnect(const buffer_c& reason = EmptyBuffer);

	//is connected?
	bool is_connected() { return (connect_state == CCS_CONNECTED); }

	//send reliable message to server
	//returns 0 on failure, 1 on success
	int send_message(buffer_c &message, int stream_id = -1, bool reliable = true);

	//dispatches the packet with the given unreliable data block and all the
	//protocol overload (reliable messages, acks...)
	//returns 0 on failure, 1 on success
	//returns the allocated packet id in the optional packet_id pointer
	int send_frame(buffer_c &udata, int *packet_id = 0);

	//gets the next reliable message avaliable from the server.
	//should be called by the "client behaviour" class inside it's incoming_data(..) method.
	//returns in buffer_c.code: 0 if no more messages, 1 if message valid, -1 if error
	buffer_c receive_message(int stream_id);

	//gets the next reliable message avaliable from the server, puts into the buffer parameter.
	//should be called by the "client behaviour" class inside it's incoming_data(..) method.
	//returns: 0 if no more messages, 1 if message valid, -1 if error
	int receive_message(buffer_c &msgbuf, int stream_id);

	// NEW (ziglite 1.2): creates a stream and returns its identifier (calls station_c method)
	int  create_stream(int type = STREAM_CUMULATIVE_ACK, policy_c policy = DefaultPolicy);

	// NEW (ziglite 1.2): deletes a stream (calls station_c method)
	// returns true if success, false otherwise 
	bool delete_stream(int stream_id = -1);

	// call get/set default stream id methods from station
	void set_default_stream_id(int stream_id);
	int  get_default_stream_id();

	// NEW (ziglite 1.2): retrieves the first stream of the server
	// return -1 if none
	bool seek_first_stream();

	// NEW (ziglite 1.2): retrieves the NEXT stream of the server (after last stream query)
	// return -1 if last stream
	int get_next_stream();

	// NEW (ziglite 1.2): verify if a specific stream exist
	bool test_stream(int stream_id);

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
	virtual bool msocket_accept_station(address_c &addr, const buffer_c& connect_data, buffer_c& reason);

	// called after msocket_accept_connection() returns true - a new station is created for 
	// the incoming connection request.
	// NOTE: the created station knows how to handle duplicate connection packets that can arrive 
	// after this call.
	virtual void msocket_station_connected(station_c &st, const buffer_c& oConnectionData);

	// msocket has closed: all station_c's have been deleted and current station_c pointers are 
	// now invalid.
	virtual void msocket_closed();

	//---- station_c callback interface impl -----------------------------
	
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

protected:

	//the behavior (callback set)
	client_behavior_i *beh;

	//the server address
	address_c serveraddr;

	// client station ptr and owner msocket
	station_c *station;
	msocket_c msocket;

	//client connection's current state
	int connect_state;		// ENUM 

	//data sent in every connect packet
	buffer_c connect_data;

	//time of last send_packet with reliables, minimum interval for sending reliables in packet
	double	last_send_reliables;
	double	min_send_reliables_interval;

	//NEW (ZIG v1.3.0): "disconnect code reason", for INTERNAL use (don't mix up with app's code_reason)
	// 0 = catch-all, default, generic, non-specific disconnection reason (no internal reason)
	int internal_disconnected_code_reason;

	//NEW (v1.3.4): custom "connect-to" timeout (in seconds/tries)
	int connect_to_server_timeout;

	//v1.5.0: client's local port range configuration
	bool use_port_range;
	int minport;
	int maxport;

	//debug console (may be 0 - no debug output)
	console_c *con;

	//compression config value. if -666, means "do not set, leave at default"
	int compression_config_value;

	//ZIGLITE 1.1: max outgoing packet size config
	int cfg_max_packet_size;
};

#endif //#ifndef NO_DOXYGEN		// internal class -- do not document

#endif

