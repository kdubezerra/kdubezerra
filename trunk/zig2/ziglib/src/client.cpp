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

#include "zig/client.h"
#include "zig/utils.h"

using namespace std;

//ctor, setting behavior. behavior will **NOT** be deleted by the client's destructor!
//ZIG 1.4.0: removing console_c dependency from thread_c
client_c::client_c(client_behavior_i *beh, console_c *conio): serveraddr(InvalidAddress) {

	this->beh = beh;
	this->con = conio;

	msocket.set_console(con);

	//v1.5.0: default client port range config - NO range (use OS default)
	set_local_port_range(0,0);

	// init msocket stuff
	station = NULL;

	//init default value
	set_connect_timeout();

	//compression config value. if -666, means "do not set, leave at default"
	compression_config_value = -666;

	//ZIGLITE v1.1
	cfg_max_packet_size = ZIG_DEFAULT_MAX_OUTGOING_PACKET_SIZE;

	reset_state();
}

//dtor
client_c::~client_c() {

	con->xprintf(0, "client_c::~client_c()\n");

	disconnect();
}

// =========== SOCKET POLLING SUPPORT ==============
//NEW (ZIG v1.2.1): when socket_polling_mode == true, must call this periodically to make 
// ZIG read the client socket and call the data callbacks (leet_incoming_data(), ...). this method 
// does nothing if the client is not connected. 
void client_c::poll_socket() {

	//if "not connected" (not even trying connection) then return
	if (connect_state == CCS_DISCONNECTED) return;

	// NEW (ziglite): call the msocket to do all the reading. 
	// if something is read, then station will CALL BACK client.cpp
	msocket.process_nonblocking();

	//NEW (ziglite 1.1): connection packets and timeout detection 
	// is now done by the msocket/station
}
// ============= SOCKET POLLING SUPPORT == END =================

//reset object state -- make it ready to connect again
void client_c::reset_state() {

	// NOT EVERYTHING goes in here!
	connect_state = CCS_DISCONNECTED;

	last_send_reliables = 0.0;

	internal_disconnected_code_reason = 0; // NEW (v1.3.0)
}

//set the server's address. call before connect() - address must be a RESOLVED IP!
bool client_c::set_server_address(char *address) {

	//assert
	if (connect_state != CCS_DISCONNECTED) return false; //cannot set address if not connected!

	address_c temp(address);

	//set address - address must be a RESOLVED IP!
	if (temp.valid() == false) {
		con->xprintf(2,"client_c::set_server_address(%s) failed error %s sys %s\n", 
			address,
			nlGetErrorStr(nlGetError()),
			nlGetSystemErrorStr(nlGetSystemError())
		);
	}

	serveraddr = temp;
	return true;
}

//set the custom data sent with every connection packet
void client_c::set_connect_data(buffer_c &data) {

	//nada mais fácil!
	connect_data = data;

}

// connects in the remote server
//returns false if there was some internal error and the request couldn't be
//  completed (this is a BAD error).
bool client_c::connect() {

	con->xprintf(0, "client_c::connect\n");

	if (connect_state != CCS_DISCONNECTED) return true;

	reset_state();
	// start trying to connect

	int result = 0;

	// open msocket
	msocket.set_callback_interface(this);
	result = msocket.open_socket(minport, maxport);
	if (result == 0) return false;	

	// IMPORTANT! since we are the client, we create a station that points 
	// to the server's address (IP:PORT). no other "stations" will be needed!
	if (serveraddr.valid() == false) {
		con->xprintf(2, "serveraddr: INVALID\n");
		return false;
	}

	station = msocket.create_station( serveraddr ); 
	station->set_callback_interface(this);

	// ZIGLITE 1.1: configure max outgoing packet sizes on the new station
	station->set_max_outgoing_packet_size(cfg_max_packet_size );

	station->set_rtt_protocol_interval(1.0);

	// configure compression on the new station. if -666, "leave at station default"
	if (compression_config_value != -666) {
		station->set_compression( compression_config_value ); 
	}

	//v1.5.0: DEBUGGING
	// a small debugging aid to help differentiate client-side and server-side 
	// stations, for when running debug mode with both clients and a servers 
	// instantiated on the same process.
	station->set_debug("client");

	// FIXMEV150 DEBUGGING
	station->set_console(con);
	
	// now AFTER station open()
	connect_state = CCS_CONNECTING;

	//ZIGLITE 1.1: use station's new connect() support
	con->xprintf(0, "connect data.size = %i\n", connect_data.size());	

	station->set_connect_data( connect_data );

	station->connect( static_cast<double>( connect_to_server_timeout ) );

	return true;
}

//send reliable message to server
//returns 0 on failure, 1 on success
int client_c::send_message(buffer_c &message, int stream_id, bool reliable) {
	if (station != NULL) return station->write(message, stream_id, reliable, NULL);
	else return -1;
}

//dispatches the packet with the given unreliable data block and all the
//protocol overload (reliable messages, acks...)
int client_c::send_frame(buffer_c &udata, int *packet_id) {

	con->xprintf(0, "client_c::send_frame\n");

	if (station == NULL) return 0;

	//write to unreliable buffer
	station->write_unreliable(udata, 0);

	//dispatch a packet
	int result = station->send_packet( packet_id );
	if (result == 0) // NEW (v1.3.0): send_packet returns 0 on success...
		return 1; 

	// here: FAILED since returned different than 0
	// LEET error codes == station error codes; save for the disconnect callback
	// NOTE: only negative values reserved in "internal_disconnected_code_reason"'s enum for 
	//  send_packet()'s error codes
	//
	// NOTE(2): -1,-2,-3,-4.. send_packet errors; 0= ok (no positive values defined)
	// so we DON'T have to check the value here. it IS <0 because already tested for ==0 above...
	//if (result < 0) { 
	internal_disconnected_code_reason = result; // this is assigned to "zig_reason" in disconnected() app callback

	//  station.send_packet FAILED and it MIGHT be related to the 
	//   station "delta" etc. limitation bugs. if that is the case,
	//   then MUST force a disconnection here.
	disconnect(); // DO NOT pass value here -- 0 (no-args) means "look into zig_reason parameter!"

	return 0;
}

//gets the next reliable message avaliable from the server.
//should be called by the "client behaviour" class inside it's incoming_data(..) method.
//returns in buffer_c.code: 0 if no more messages, 1 if message valid, -1 if error
buffer_c client_c::receive_message(int stream_id) {
	return station->read(stream_id);
}

// gets the next message avaliable from a stream of the server, puts into the buffer parameter.
// should be called by the "client behaviour" class inside it's incoming_data(..) method.
// returns: 0 if no more messages, 1 if message valid, -1 if error
int client_c::receive_message(buffer_c &msgbuf, int stream_id) {

	//get next message:
	//0 = fail(==no more msg)  1 = success(==had a message). -1 error (not used)
	station->read(msgbuf, stream_id); 
	return msgbuf.code;	
}

// NEW (ziglite 1.2): creates a stream and returns its identifier (calls station_c method)
int client_c::create_stream(int type, policy_c policy) {
	if (station) return station->create_stream(type, policy);
	else return -1;
}

// NEW (ziglite 1.2): deletes a stream (calls station_c method)
// returns true if success, false otherwise 
bool client_c::delete_stream(int stream_id) {
	if (station) return station->delete_stream( stream_id );
	else return false;
}

// call get/set default stream id methods from station
void client_c::set_default_stream_id(int stream_id) {
	if (station) station->set_default_stream_id(stream_id);
}

int client_c::get_default_stream_id() {
	if (station) return station->get_default_stream_id();
	else return -1;
}

// NEW (ziglite 1.2): retrieves the first stream of the server
// return -1 if none
bool client_c::seek_first_stream() {
	if (station) {
		station->seek_first_stream();
		return true;
	}
	else return false;
}

// NEW (ziglite 1.2): retrieves the NEXT stream of the client (after last stream query)
// return -1 if last stream
int client_c::get_next_stream() {
	if (station) return station->get_next_stream();
	else return -1;
}

//PROTECTED - disconnect. sends disconnection packets, etc.
//code_reason is a custom int parameter passed to the callback.
void client_c::disconnect(const buffer_c& reason) {

	//if not connected, do nothing
	if (connect_state == CCS_DISCONNECTED) return;

	con->xprintf(0, "client_c::disconnect\n");

	station->disconnect(reason);
}

// incoming packet from unregistered IP:PORT source. a typical implementation of this method 
// would answer directly to "serverinfo"-like requests, and would also handle "new connection" 
// requests by registering a new station_c socket for that IP:PORT.
// parameters: remote address and UDP packet payload
void client_c::msocket_incoming_unreg(address_c &addr, const buffer_c &pkt) {

	// clients only accept packets from the server they have connected to, so 
	// any "unreg" packets at the client are just ignored. 
}

// a remote station has called connect() towards us. return true to accept, false to reject.
bool client_c::msocket_accept_station(address_c &addr, const buffer_c& connect_data, buffer_c& answer) {
	// client_c NEVER accepts anything. just ignore
	return false;
}

// called after msocket_accept_connection() returns true - a new station is created for 
// the incoming connection request.
// NOTE: the created station knows how to handle duplicate connection packets that can arrive 
// after this call.
void client_c::msocket_station_connected(station_c &st, const buffer_c& oConnectionData) {
	// NEVER called
}

// got NL_INVALID while reading from the UDP socket. an use of this callback is to detect
// NL_SOCK_DISCONNECT errors and, if a client, disconnect from the server or, if a server, 
// try to find out which client socket died and remove it from the server.
void client_c::msocket_read_error(NLsocket &sock) {

	if (nlGetError() == NL_SOCK_DISCONNECT) {
		con->xprintf(2, "CLIENT_C got NL_SOCK_DISCONNECT, checking if from server address.\n");
		
		address_c rad(sock);

		con->xprintf(2, "  socket remote addr = %s\n", rad.get_address().c_str());
		con->xprintf(2, "  server remote addr = %s\n", serveraddr.get_address().c_str());

		// ZIGLITE v1.0.1 BUG FIX: test for CCS_CONNECTED: if get read errors while trying 
		// to connect, just ignore them. if this check is removed, the connection doesn't
		// return with "connection timed out".
		if (rad == serveraddr) {
			internal_disconnected_code_reason = LEETCLIENT_DISCONNECTED_SOCKET_ERROR;
			disconnect();
		}
	}
}

// msocket has closed: all station_c's have been deleted and current station_c pointers are 
// now invalid.
void client_c::msocket_closed() {
}

//---- station_c callback interface impl -----------------------------

// incoming data read by a station from the msocket. it can either be "special" packets 
// (packet_id == 0) or "regular"(=="normal") game packets (packet_id > 0 && packet_id <= 32512)
// NEW ZIGLITE 1.2: stream identifier
void client_c::station_incoming_data(buffer_c &udata, int packet_id) {

	//for answers
	buffer_c  buf;

	if (udata.code == STATION_INCOMING_PACKET_NORMAL) {

		// call back the gameclient forwarding the unreliable block of data from the RUDP packet...
		// ZIGLITE 1.1: FIXED: do not call leet_incoming_data() if leet_connected() hasn't yet been 
		// called. this can happen with out-of-order receipt of packets...
		if (connect_state == CCS_CONNECTED) {
			beh->leet_incoming_data(udata, packet_id);
		}
	}
	else {
		//log unknown code from server
		con->printf("FATAL ERROR (client_c::incoming_server_gamepacket): UNKNOWN INCOMING PACKET CODE = %i\n", udata.code);
		FAILSTOP();
	}
}

//NEW (ziglite 1.1): connection handshake: connection accepted
void client_c::station_connected(buffer_c& connection_pkt, const bool forced) {

	if (connect_state == CCS_CONNECTED) {
		con->printf("FATAL ERROR (client_c::station_connected called twice!!)\n");
		FAILSTOP();
	}
	else {

		//connected!
		connect_state = CCS_CONNECTED;

		con->xprintf(0, "client_c::station_connected - irei chamar leet_connected\n");
		con->xprintf(0, "connection_pkt.size = %i, size_left = %i\n", connection_pkt.size(), connection_pkt.size_left());
		beh->leet_connected( connection_pkt );
	}
}

/// Connection handshake: connection attempts timed out
void client_c::station_connection_timed_out() {

	connect_state = CCS_DISCONNECTED;

	//close station / kill msocket
	msocket.close_socket();
	station = NULL;		// this is sufficient because msocket.close_socket(); deletes all station_c instances on its map!

	con->xprintf(0,"client_c::kill_connection() - closed msocket\n");

	//"no response"
	beh->leet_connection_timed_out();
}

/// Connection handshake: refused
void client_c::station_connection_refused(buffer_c& reject_pkt) {

	if (connect_state == CCS_CONNECTED) {

		con->xprintf(6,"***********************************************************************************\n");
		con->xprintf(6,"ERROR CLIENT_C: RECEIVED 'REJECT CONNECT' BUT ALREADY CONNECTED! SERVER IS BUGGED!!\n");
		con->xprintf(6,"***********************************************************************************\n");
		
		//so we just disconnect and that's it
		disconnect();
	}
	else if (connect_state != CCS_DISCONNECTED) {

		//correct state if in "connecting" or "disconnecting" states

		// **********************************************
		// FIXME : get the buffer for the callback!!
		// **********************************************
		//remaining data is custom answer data
		//if (udata.size_left() > 0)
		//	buf.putBlock( udata.data_cur(), udata.size_left() );
		//notify behavior
		//buf.seek(0);

		connect_state = CCS_DISCONNECTED;

		//close station / kill msocket
		msocket.close_socket();
		station = NULL;		// this is sufficient because msocket.close_socket(); deletes all station_c instances on its map!

		// TODO: fill the .code field with the disconnection reason
		beh->leet_connection_refused( reject_pkt );
	}
}

void client_c::station_disconnected(bool remote, buffer_c& reason) {

	// first argument is: "bool server_initiated". since we are the CLIENT, then if the 
	// connection was initiated REMOTELY, then remote==true and so server_initiated==true.
	// conversely, if the disconnection has local causes, then server_initiated==remote==false.
		//set state
	connect_state = CCS_DISCONNECTED;

	//close station / kill msocket
	msocket.close_socket();
	station = NULL;		// this is sufficient because msocket.close_socket() deletes all station_c instances on its map!

	buffer_c empty;
	beh->leet_disconnected(remote, empty, 0, this->internal_disconnected_code_reason);
}
