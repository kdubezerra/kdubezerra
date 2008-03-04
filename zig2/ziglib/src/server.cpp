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

//debug print macro
#define LOG(P0) { if (con != 0) con->xprintf(1,P0); }
#define LOG1(P0,P1) { if (con != 0) con->xprintf(1,P0,P1); }
#define LOG2(P0,P1,P2) { if (con != 0) con->xprintf(1,P0,P1,P2); }
#define LOG3(P0,P1,P2,P3) { if (con != 0) con->xprintf(1,P0,P1,P2,P3); }
#define LOG4(P0,P1,P2,P3,P4) { if (con != 0) con->xprintf(1,P0,P1,P2,P3,P4); }
#define LOG5(P0,P1,P2,P3,P4,P5) { if (con != 0) con->xprintf(1,P0,P1,P2,P3,P4,P5); }

//include utils.h AFTER debug macros
#include "zig/server.h"
#include "zig/utils.h"
#include "zig/address.h"

using namespace std;

//PROTECTED -- added this function because if this thead can't be
//created, then at least the final disconnect procedure is called...
void server_c::client_disconnection_finalization(remote_client_c *cl) {

	// call back app
	beh->leet_client_disconnected( cl->id , cl->disconnection_reason_code );
	
  //**** delete the client and it's entry from the map ****
	// this is what closes the client socket -- deletes the cl->station which
	// in it's destructor closes the socket.
	//remove from client's map
	map<int, remote_client_c*>::iterator it = clients.find( cl->id );
	if (it == clients.end()) {
		LOG("***********======== (((( BAD LEET SERVER ERROR )))=========******************\n");
		LOG1("*********** LEET SERVER ERROR: can't delete_client( %i ) -- NOT FOUND!\n", cl->id);
		FAILSTOP();
	}
	else {
		clients.erase(it); // erase from map
		SAFEDELETE(cl); //DELETE the client object
	}
}

//shared code like process_requests() : broadcastthread_thread_run() and ... poll_sockets() ? (yes)
void server_c::process_broadcasts(int result, buffer_c pkt) {

	//get address of client who sent the broadcast packet
	NLaddress client_address;
	nlGetRemoteAddr(broadcast_sock, &client_address);

	//prepare for parsing from the beginning
	pkt.seek(0);

	// check data to read
	if (result > 0) {

		try {

			//get command byte
			NLubyte command = pkt.getByte();

			con->xprintf(2,"BROADCASTTHREAD read = %i command = %i\n", pkt.size(), (int)command);

			//command: LAN QUERY -- a client wants to know about servers (us)
			if (command == LEET_BCCOMMAND_SERVERINFO) {

				// servers answers as broadcast to the LAN too, to make implementation simpler. however 
				// the server answers with the same "query ID" sent in the query so at least some sort 
			// of filtering by the clients can be implemented if deemed useful
			
				NLlong query_id = pkt.getLong();

				//custom query data passed by client along with LEET_BCCOMMAND_QUERY
				buffer_c custom;
				custom.putBlock( pkt.data_cur(), pkt.size_left());  //put on custom the data on the current pkt pos
				custom.seek(0);		//keep custom ready to be parsed

				//an empty buffer - to be filled with custom answer to client
				buffer_c answer;

				//ask gameserver what's the answer 
				bool send_response = beh->leet_get_server_info(client_address, custom, answer);

				//if gameserver tells us to send the answer, then send it!
				if (send_response) {

					char blah[200];
					nlAddrToString(&client_address, blah);
					con->xprintf(2,"sending broadcast-serverinfo response (origin was: %s)\n", blah);

					//ignore nlWrite() errors.
					buffer_c anspkt;
					anspkt.putByte( (NLubyte) LEET_BCRESULT_SERVERINFO );
					anspkt.putLongs( query_id );											 // query id to enable possible filtering
					anspkt.putShort( (NLushort)this->port );         // where is the server (port number)
					anspkt.putBlock( answer.data(), answer.size() ); // custom app answer
					anspkt.seek(0);
					anspkt.getDataToSocket(broadcast_sock, anspkt.size());
				}
			}
			//unknown command
			else {
				// what else could we do?
				// just ignore it...
			}
		}
		catch (int ) {
			// exception reading packets. just ignore it...
			//
			// REVIEW: we could also count the times a remote IP address causes this and eventually
			//   auto-ban/auto-suspend it here.
		}
	}
	else {
		LOG1("BROADCASTTHREAD WARNING: read result == %i, ignoring this pkt.\n", result);
	}
}

//ctor, setting behavior. behavior will be deleted by the server's destructor
server_c::server_c(server_behavior_i *beh, console_c *conio) {

	this->beh = beh;
	con = conio;

	msocket.set_console(con);

	//debug_id = "(server_c)";

	//my local compression config flag. it is applied to all the new clients. -999 is a magic value that
	//specifies that the flag is not initialized yet (so let the default from station.h be used)
	server_compression_config = -999;

	sock = NULL;
	started = false;
	port = 6666;
	broadcast_port = ZIG_DEFAULT_BROADCAST_PORT;

	maxplayers = 16;

	set_client_timeout();

	//v2.0: server's default port range configuration
	//Port range is enabled by default, and the default interval to
	//scan avaliable ports is set to [1025, 5000]
	use_port_range = true;
	minport = 1025;
	maxport = 5000;
	set_local_port_range(minport, maxport);

	client_id_generator = -1; // NEW v1.5.0

	//ZIGLITE v1.1
	cfg_max_packet_size = ZIG_DEFAULT_MAX_OUTGOING_PACKET_SIZE;
}

//dtor
server_c::~server_c() {

	//stop server
	stop();

	//delete behavior... NOT! NEVER! 	//delete beh;  	//beh = 0;
}

// configure usage of packet compression. to turn on the sending of compressed packets at any time, 
// pass a value >=0 as argument, which specifies the minimum size that the packet must have, in bytes, 
// to be compressed (pass 0 to compress all outgoing packets). to turn off the sending of compressed 
// packets at any time, call this method with no parameters (or a negative value as parameter). 
// NOTE: this call resets the compression stats.
void server_c::set_compression(int minimum_packet_size) {

	//record the setting (so we can apply to all the new clients
	server_compression_config = minimum_packet_size;

	//reconfigure existing clients now
	map<int, remote_client_c*>::iterator it = clients.begin();
	while (it != clients.end()) {
		remote_client_c *cl = (*it).second; //alias
		cl->station->set_compression( server_compression_config ); //reconfigure packet compression with that client
		it++; //next
	}
}

//start server at port, with a maximum acceptable number of players. returns 0 if fails, 1 on success.
int server_c::start(int port_num, int max_players, int broadcast_port) {

	//already started? cannot re-start -- caller should call stop() himself
	if (started)
		return 0;

	maxplayers = max_players;
	port = port_num;
	this->broadcast_port = broadcast_port; // NEW: listening socket for UDP broadcast!

	// ZIGLITE NEW: create the msocket
	msocket.set_callback_interface(this);
	int result = msocket.open_socket(port, port);
	if (result == 0)
		return 0;

	//v1.5.0 NEW: start socket (and possibly thread, below) that listens for LAN BROADCAST
	//  (all of this is mostly copied from "reqthread/sock" code)
	nlHint(NL_REUSE_ADDRESS, NL_TRUE);
	broadcast_sock = nlOpen(broadcast_port, NL_BROADCAST);
	nlHint(NL_REUSE_ADDRESS, NL_FALSE);
	nlSetSocketOpt(broadcast_sock, NL_BLOCKING_IO, NL_FALSE);  // Disable blocking IO
	if (broadcast_sock == NL_INVALID) {

		// clean up: close the msocket
		msocket.close_socket();
	
		return 0; // FAILED TO OPEN SOCKET (port in use, probably)
	}

	//ok
	started = true;
	return 1;
}

//stops the server immediately.
void server_c::stop() {

	//already stopped?
	if (started == false) return;

	// close LAN broacast socket
	// THIS replaces the slap_broadcast_socket() below (wakes up the nlPollSocket(broadcast_sock); )
	nlClose(broadcast_sock);
	broadcast_sock = NL_INVALID;

	//trigger disconnector thread for all client
	map<int, remote_client_c*>::iterator it = clients.begin();
	while (it != clients.end()) {

		//alias
		remote_client_c *cl = (*it).second;

		//next
		// IMPORTANT: eh preciso incrementar o iterador ANTES de chamar
		// disconnect_client, pois esse metodos chama o metodo
		// client_disconnection_finalization, que remove o cliente do map,
		// portanto invalidando o iterador
		it++;

		//tell to disconnect it
		//con->printf("server::disconnect_client(%i, timeout= %i) called\n", cl->id, timeout);

		//ZIG 2.0: store disconnection reason
		cl->disconnection_reason_code = LEETSERVER_DISCONNECTED_SHUTDOWN;

		disconnect_client(cl->id);
	}

	// clean up
	clients.clear();		//clearing clients

	//ZIGLITE: finally close the socket and delete all station_c objs
	msocket.close_socket();
	
	//ok
	started = false;
}

// ZIG_SERVER_THREADING_0: when this option is passed to start(), then server_c will NOT create 
// background threads for polling client sockets, polling the server request socket or check 
// for client timeouts. in this situation you must call this method periodically to make 
// server_c do all of this. this method does nothing if the server is not started. 
void server_c::poll_sockets() {

	//con->xprintf(0, "server_c::poll_sockets init\n");

	// 1. broadcastthread and broadcast_sock -- nonblocking read pending LAN broadcast packets
	buffer_c pkt;
	pkt.clear();
	NLint result = pkt.putDataFromSocket(broadcast_sock);
	if (result == 0) {
		//NOP -- no pending requests
	}
	else if (result == NL_INVALID) {
		
		//whatever...
		LOG4("BROADCASTTHREAD read, result = %i ERR(%s) SYSERR(%i %s)\n", result, 
		nlGetErrorStr(nlGetError()), 
		nlGetSystemError(), 
		nlGetSystemErrorStr(nlGetSystemError()) );
	}
	else {

		// process the nlRead() result...
		try {
			process_broadcasts(result, pkt);
		} catch (int) {	} // just ignore malformed packets
	}

	// 2. read and process connected clients from "the" server socket
	msocket.process_nonblocking();

	// 3. check for client time-outs AND send disconnection packets / finalize client disconnection
	// duplicating some code so we avoid iterating twice for ALL clients in the same tight loop
	//
	//   FIXME: this will certainly NOT scale if you have thousands of clients; there is no need
	//    to iterate through ALL clients EVERY call to poll_sockets(). then again, waiting X time
	//    and THEN iterating through ALL clients doesn't help either (the "chosen" frames will still 
	//    lag badly)
	// 
	//   TO-DO: iterate through a certain "portion" of the client list each time poll_sockets() is
	//    called. this portion ideally depends on the size of the client list.
	//

	//current time
	double now = get_time();

	//check all clients for timeouts and sending disconnection packets
	map<int, remote_client_c*>::iterator it = clients.begin();
	while (it != clients.end()) {

		// client
		remote_client_c *cl = (*it).second;

		// must increment the iterator before the call to disconnect_client() because that's
		// where the client will be removed from the clients map, thus making the iterator invalid
		it++;

		// client still connected by server and timed out?
		if (cl->server_disconnected == false)
		if ((now - cl->last_received_packet) > client_timeout) {

			con->xprintf(3,"(SERVER): disconnecting client %i due to timeout.\n", cl->id);

			//ZIG 2.0: store disconnection reason
			cl->disconnection_reason_code = LEETSERVER_DISCONNECTED_TIMEOUT;

			disconnect_client(cl->id);
		}

		// ZIG 2.0.0: removing anything that uses disconnect_packets_left_count & friends since this 
		//            logic should be in station_c (HOWEVER take care since SOME of the disconnection
		//            logic needs to stay because server calls station to disconnect, so at least server knows
		//            that there is a disconnection going on for that client!
		/*
		// disconnecting this client?
		if (cl->disconnect_client == true) {

			// if already reached zero or client acknowledged: call disconnection finalization
			if ((cl->disconnect_packets_left_count <= 0) || ((cl->server_disconnected == true) && (cl->told_disconnect == true))) {
			
				// finalize disconnection. since this DELETES the client from the map, must "fix" the iterator here
				// by advancing it before the call
				it++;
				client_disconnection_finalization(cl);
				continue; //skip the "it++" at the end of the loop since we already skipped one
			}
			// still packets left to send and time to send?
			else if ((cl->disconnect_packets_left_count > 0) && (now > cl->disconnect_packets_next_time)) {

				// send packet
				cl->station->send_raw_packet( cl->disconnect_packet_cache );
	     
				// less one
				cl->disconnect_packets_left_count--;
				cl->disconnect_packets_next_time = get_time() + 0.2;  //200ms - next check
			}
		}
		*/
	}
}

//disconnects a specific client.
//return: 0 fail (no such client?), 1 success.
//additional parameter: custom disconnect data to be passed to the client.
int server_c::disconnect_client(int client_id, buffer_c reason) {

	//FIXED A BUG: disconnect_client() can be called multiple times. it is the job
	// of this function to RETURN 0 when the client is already disconnected.
	// if the client is not in the map, there's no such client and returns 0; if the client
	// is found but it's "disconnect_client" flag is TRUE, then return 0 also.

	bool fail = false;

	//find client
	remote_client_c *cl = get_client(client_id);

	//not found?
	if (cl == NULL) {
		con->xprintf(2,"ERROR (server_c::disconnect_client): can't find client==%i\n", client_id);
		fail = true;
	}
	//found but already disconnecting?
	else if (cl->disconnect_client == true) {
		fail = true;
	}
	else {
		//set "disconnect_client() called" flag
		cl->disconnect_client = true;
	}

	if (fail) return 0;

	//register the fact that was the server that nuked the client
	//
	// FIXED: apparently, this is only true if the client HASN'T already
	//   sent us a disconnect packet FIRST! in which case, cl->told_disconnect is true
	//   and then server called this method to obey the client!
	//
	if (cl->told_disconnect == false) cl->server_disconnected = true;

	cl->station->disconnect(reason);

	return 1;
}

//disconnects a specific client.
//return: 0 fail (no such client?), 1 success.
int server_c::disconnect_client(int client_id) {
	
	buffer_c nothingness;
	return disconnect_client(client_id, nothingness);
}



//send a game frame to a given client. must be called regularly (~100ms), but there are no hard
//requirements for the interval.
//returns 0 on failure, 1 on success.
//returns the allocated packet id in the optional packet_id pointer
int server_c::send_frame(int client_id, buffer_c &framebuf, int *packet_id) {

	con->xprintf(0, "server_c::send_frame init\n");

	//find client
	remote_client_c *cl = get_client(client_id);

	//not found?
	if (cl == 0)
		return 0;

	//write to unreliable buffer
	cl->station->write_unreliable(framebuf);

	//dispatch the packet
	//return cl->station.send_packet(packet_id);
	// NEW (v1.3.0): send_packet returns 0 on SUCCESS now, and <0 (>0?) on failure

	int result = cl->station->send_packet(packet_id);
	if (result == SENDPACKET_OK)  // 0 is success
		return 1; // but 1 is expected for success
	
	//NOTE: result IS <0 here (since already tested for ==0 above and there are no +1,+2,+3.. positive 
	//  return values for station.send_packet()
	// the negative result value here means a BAD error ocurrent with the station packet sending:
	//  - nlWrite() failed (probably bad enough)
	//  - some limitation on the reliable resend/ack buffers was reached
	//
	// either way, since it is bad, we will disconnect the client here!
	//

	// here: FAILED since returned different than 0
	//
	//  ZIG 2 NOTE: result is (MUST BE) a negative value. the code will be translated by zigserver to one 
	//  of the ZIGSERVER_DISCONNECTED_xxx values
	//cl->disconnection_reason_code = result;
	//
	// ZIG 2 NEW: stop the bullshit and send a socket error code
	cl->disconnection_reason_code = LEETSERVER_DISCONNECTED_SOCKET_ERROR;

	// MUST _NOW_ cause client disconnection. if this is not called the client will 
	// simply time out due to lack of server sending packets (at least this happens 
	// when on debugmode we set the server-side stations to always return -3 on 
	// send_packet...)
	disconnect_client(client_id);

	return 0; // and 0 for failure
}

//sends the given reliable message to the given client. reliable = heavy, do not use for frequent
//world update data. use for gamestate changes, talk messages and other stuff the client can't miss, or
//stuff he can even miss but it's better if he doesn't and the message is so infrequent and small that
//it's worth it.
//returns 0 on failure, 1 on success
int server_c::send_message(int client_id, buffer_c &message, int stream_id, bool reliable) {

	//find client
	remote_client_c *cl = get_client(client_id);

	//not found?
	if (cl == 0) return 0;

	//do send it
	return send_message(cl, message, stream_id, reliable);
}

//broadcasts the given reliable message to all active clients. return: 0 fail, 1 success.
int server_c::broadcast_message(buffer_c &message) {

	int retval = 1;

	//call send_message for every client
	map<int, remote_client_c*>::iterator it = clients.begin();
	while(it != clients.end()) {

		if (send_message((*it).second, message) == 0)
			retval = 0;		//fails broadcast if a single send failed
		it++;
	}

	//"fails" if a single send fails.
	return retval;
}

//retrieve the next pending message from a client
//if no more messages returns buffer.code == 0, else returns buffer.code == 1
//returns buffer.code == -1 if client_id not found
buffer_c server_c::receive_message(int client_id, int stream_id) {

	buffer_c msgbuf;

	//find client in map
	map<int, remote_client_c*>::iterator it = clients.find(client_id);

	//check if found
	if (it == clients.end()) {
		msgbuf.code = -1; //error: client not found!
	}
	else {
		//client found, get next message from it:
		//0 = fail(==no more msg)  1 = success(==had a message)
		msgbuf = it->second->station->read(stream_id);
	}

	return msgbuf;
}

//retrieve the next pending message from a client. returns 0 if no more messages, 1 if message read ok.
//buffer paramenter: a pre-allocated buffer for putting the message in
//returns -1 if client_id not found
int server_c::receive_message(int client_id, buffer_c &msgbuf, int stream_id) {

	//find client in map
	map<int, remote_client_c*>::iterator it = clients.find(client_id);

	//check if found
	if (it == clients.end()) {
		msgbuf.code = -1; //error: client not found!
	}
	else {
		//client found, get next message from it:
		//0 = fail(==no more msg)  1 = success(==had a message)
		it->second->station->read(msgbuf, stream_id);
	}

	return msgbuf.code;	
}

// NEW (ziglite 1.2): creates a stream and returns its identifier (calls station_c method)
int server_c::create_stream(int client_id, int type, policy_c policy) {

	//find client
	map<int, remote_client_c*>::iterator it = clients.find(client_id);	
	if (it == clients.end()) return -1;

	remote_client_c* cli = it->second;
	// just paranoia ;)
	if ( cli == NULL ) return -1;

	if ( cli->station == NULL ) return -1;

	return cli->station->create_stream(type, policy);
}

// NEW (ziglite 1.2): deletes a stream (calls station_c method)
// returns true if success, false otherwise
bool server_c::delete_stream(int client_id, int stream_id) {

	//find client
	map<int, remote_client_c*>::iterator it = clients.find(client_id);	
	if (it == clients.end()) return false;

	remote_client_c* cli = it->second;
	// just paranoia ;)
	if ( cli == NULL ) return false;

	if ( cli->station == NULL ) return false;

	return cli->station->delete_stream(stream_id);
}

// call get/set default stream id methods from station
void server_c::set_default_stream_id(int client_id, int stream_id) {

	//find client
	map<int, remote_client_c*>::iterator it = clients.find(client_id);	
	if (it == clients.end()) return;

	// just paranoia ;)
	if ( it->second == NULL) return;
	
	if ( it->second->station) it->second->station->set_default_stream_id(stream_id);
}

int server_c::get_default_stream_id(int client_id) {
	//find client
	map<int, remote_client_c*>::iterator it = clients.find(client_id);	
	if (it == clients.end()) return -1;

	// just paranoia ;)
	remote_client_c* cli = it->second;
	if ( cli == NULL) return -1;

	if (cli->station) return it->second->station->get_default_stream_id();
	else return -1;
}

// NEW (ziglite 1.2): retrieves the first stream of the client
// return -1 if none
bool server_c::seek_first_stream(int client_id) {
	//find client
	map<int, remote_client_c*>::iterator it = clients.find(client_id);	
	if (it == clients.end()) return false;

	remote_client_c* cli = it->second;
	// just paranoia
	if (cli == NULL) return false;

	cli->station->seek_first_stream();
	return true;
}

// NEW (ziglite 1.2): retrieves the NEXT stream of the client (after last stream query)
// return -1 if last stream
int server_c::get_next_stream(int client_id) {
	//find client
	map<int, remote_client_c*>::iterator it = clients.find(client_id);	
	if ( it == clients.end() ) return -1;

	remote_client_c* cli = it->second;
	if (cli == NULL) return -1; //just paranoia

	return cli->station->get_next_stream();
}

//THREAD CALLBACK : data just arrived in a client's "station"
void server_c::incoming_client_data(remote_client_c *cl, buffer_c &in, int packet_id) {

	// bump lag control timer forward... (so client doesn't disconnect)
	cl->last_received_packet = get_time();
	
	//check if request packet
//	if (in.code == STATION_INCOMING_PACKET_REQUEST) {

		// =========
		//   NOTE
		// =========
		//
		//this is called if the other side is making "station requests" ("user" kind)
		// in.id = request id
		// in.data = 100% full of custom request data
		//
		// THIS "IF" case is preventing the request from reaching the gameserver! so
		// if we want to forward this request, we must do so explicitly here (through
		// a nice callback, for instance, instead of returning a "game packet" and putting
		// ugly special .code's  and .id's in the buffer_c object!

		// *****************************************************************************
		//      CURRENTLY, THE SERVER DOESN'T HAVE NOTHING TO ANSWER TO THE CLIENT!
		// *****************************************************************************
	//check if special (raw) packet
	//else 
	if (in.code == STATION_INCOMING_PACKET_SPECIAL) {
		// ZIGLITE 1.1: avoid malformed packets with too little data
		if (in.size_left() >= (int) (sizeof(NLshort) + sizeof(NLubyte))) {

			//con->printf("CTS BUG antes do getshort: in.getpos = %i in.size = %i\n", in.getpos(), in.size());
		
			in.getShort();		//skip "0" id of special packet

			//con->printf("CTS BUG depois do getshort: in.getpos = %i in.size = %i\n", in.getpos(), in.size());

			NLubyte code = in.getByte();		//get special code

			//con->printf("CTS BUG depois do getbyte: in.getpos = %i in.size = %i\n", in.getpos(), in.size());
			//con->printf("CTS BUG code = = %i\n", code);

			//client wants to disconnect or is acking a disconnection
			if (code == CTS_DISCONNECT) {

				con->xprintf(2,"server_c received CTS_DISCONNECT from client %i\n", cl->id);

				//client now knows
				cl->told_disconnect = true;

				//if disconnection initiated by server - NOP, the disconnector thread will catch
				//the value of told_disconnect... eventually
				if (cl->server_disconnected) {

					// callback already called, no reply needed
				}
				//client-initiated disconnection
				else { 

					//disconnect the client with the async thread to avoid problems deleting
					//the remote_client_c*, which is used by the reader loop, which, in turn,
					//uses "this" thread.
					//con->printf("server_c::incoming_client_data(): client %i initiated disconnection..\n", cl->id);

					//ZIG 2.0: store disconnection reason
					// NO REASON: client-side disconnection
					cl->disconnection_reason_code = LEETSERVER_DISCONNECTED;

					disconnect_client(cl->id);		//what's the timeout?
				}
			}
			// messages that can be handled by "unreg" code
			else if (code == CTS_SERVERINFO) {

				// this is not quite elegant. the "unreg" code for CTS_CONNECT handles both the 
				// case of a new connect request and handling the replica packets with connection 
				// requests that have been already granted (the client is already connected and 
				// it is why the CTS_CONNECT is received by this incoming_client_data() instead).
				// in the case of CTS_CONNECT it would be more elegant if the unreg code handled 
				// the "new" connect request case and leave the echoing of the accept response 
				// to this callback, but since the "old" code that already existed handles both 
				// cases on the same place and it is spaguetti code, we leave it all to it.
			 
				in.seek(0);  // "rewind" packet since incoming_packet_unreg() starts over

				address_c addr = cl->station->get_remote_address();

				msocket_incoming_unreg(addr, in);
			}		
			else {
			
				// DEAL WITH UNKNOWN SPECIAL MESSAGE FROM CLIENT
				con->printf("LEET SERVER: UNKNOWN 'SPECIAL' message %i from client!\n", code);
			}
		}
		else {
			//malformed packet - disconnect the client
			//con->xprintf(2,"server_c received a MALFORMED CTS_DISCONNECT packet from client %i: disconnected it!\n", cl->id);

			//ZIG 2.0: store disconnection reason
			cl->disconnection_reason_code = LEETSERVER_DISCONNECTED_PROTOCOL_VIOLATION;

			disconnect_client(cl->id);
		}
	}
	//not special, a regular game packet
	else if (in.code == STATION_INCOMING_PACKET_NORMAL) {

		if (cl->server_disconnected == true) {
			
			// do nothing, a thread should be already spitting "disconnect" packets to the client
		}
		else if (cl->told_disconnect == true) {

			// this could occur if 1) the client is nuts 2) a data packet sent before the
			// disconnection request arrived first. in any case just ignore it since the
			// client connection is already doomed.
		}
		else { 

			// send the data to the gameserver
			in.seek(0);
			beh->leet_client_incoming_data( cl->id, in, packet_id );
		}
	}
	else {
		//log unknown code
		con->xprintf(6, "FATAL ERROR (server_c::incoming_client_data): UNKNOWN INCOMING PACKET CODE == %i\n", in.code);
		FAILSTOP();
	}
}

//PROTECTED -- get client for id
remote_client_c *server_c::get_client(int client_id) {

	//find client
	map<int, remote_client_c*>::iterator it = clients.find(client_id);

	//check if found
	if (it == clients.end())
		return NULL; //error: client not found!

	//return ptr to the client
	return (*it).second;
}

//PROTECTED -- effectively sends the message
int server_c::send_message(remote_client_c *cl, buffer_c &message, int stream_id, bool reliable) {
	return cl->station->write(message, stream_id, reliable, NULL);
}

//shared code -- timeout_thread_run() and poll_sockets() -- checks for client timeouts
void server_c::process_timeouts() {

	//current time
	double now = get_time();

	//check all clients for timeouts
	map<int, remote_client_c*>::iterator it = clients.begin();
	while (it != clients.end()) {

		remote_client_c *cl = (remote_client_c *)( (*it).second );

		//client still connected by server and timed out?
		if ( cl->server_disconnected == false )
		if ( (now - cl->last_received_packet) > client_timeout ) {

			con->xprintf(3,"(SERVER): disconnecting client %i due to timeout.\n", cl->id);

			//ZIG 2.0: store disconnection reason
			cl->disconnection_reason_code = LEETSERVER_DISCONNECTED_TIMEOUT;
			
			disconnect_client(cl->id);
		}

		//next
		it++;
	}
}

// msocket has closed: all station_c's have been deleted and current station_c pointers are 
// now invalid.
void server_c::msocket_closed() {

	// ZIGLITE REVIEW: nothing to do (?)
}

// got NL_INVALID while reading from the UDP socket. an use of this callback is to detect
// NL_SOCK_DISCONNECT errors and, if a client, disconnect from the server or, if a server, 
// try to find out which client socket died and remove it from the server.
void server_c::msocket_read_error(NLsocket &sock) {

	if (nlGetError() == NL_SOCK_DISCONNECT) {
		con->xprintf(2, "SERVER_C got NL_SOCK_DISCONNECT, checking if from a known client address.\n");
		
		address_c rad(sock);
		con->xprintf(2, "  socket remote addr = %s\n", rad.get_address().c_str());
		
		map<int, remote_client_c*>::iterator it = clients.begin();
		while (it != clients.end()) {

			remote_client_c *cl = (*it).second;

			address_c cad = cl->station->get_remote_address();

			con->xprintf(2, "  client %i's remote addr = %s\n", cl->id, cad.get_address().c_str());

			if (rad == cad) {
				con->xprintf(2, "    disconnecting client %i because of remote socket death!\n", cl->id);

				// disconnect the client

				//ZIG 2
				cl->disconnection_reason_code = LEETSERVER_DISCONNECTED_SOCKET_ERROR;

        disconnect_client( cl->id );

				break;  // no more to check
			}

			it++;
		}
	}
}

// incoming packet from unregistered IP:PORT source. a typical implementation of this method 
// would answer directly to "serverinfo"-like requests, and would also handle "new connection" 
// requests by registering a new station_c socket for that IP:PORT.
// parameters: remote address and UDP packet payload
void server_c::msocket_incoming_unreg(address_c &addr, const buffer_c &pkt) {

	// ZIGLITE : set up vars expected by the cut n pasted code below

	// avoid warning (warning: unused variable ¿result¿)
	//int result = pkt.size(); //"result" should be the packet length
	NLsocket &sock = msocket.sock;	// alias to the only NLsocket we have...
		
	//prepare for parsing from the beginning
	buffer_c::iterator it = pkt.begin();

	// ZIGLITE: use the address we got...
	NLaddress nladdr = addr.get_nl_address();
	nlSetRemoteAddr(sock, &nladdr);		//"address of the next outgoing msgs = dummy"

	//ZIGLITE 1.1: avoid malformed packets with too little data
	if (pkt.size_left() >= (int) (sizeof(NLushort) + sizeof(NLubyte))) {

		//strip the 0 packet id (mandatory, since now "packet IDs" are used on all ZIG protocols since
		//  the "LEET" packets port is abolished)

		//avoid warning (warning: unused variable ¿dummy¿)
		//NLushort dummy = it.getShort();  // FIXME: assert this?
		it.getShort();  // FIXME: assert this?

		//get command byte
		NLubyte command = it.getByte();

		con->xprintf(2,"REQTHREAD read = %i command = %i\n", pkt.size(), (int)command);

		//command: GET SERVER INFORMATION
		if (command == CTS_SERVERINFO) {

			//custom request data passed by client along with LEET_SVCOMMAND_INFORMATION
			buffer_c custom;
			int ansSize = pkt.size_left() - sizeof(NLushort) - sizeof(NLubyte);
			if (ansSize > 0) {
				char* block = new char[ansSize];
				it.getBlock(block, ansSize);
				custom.putBlock(block, ansSize);  //put on custom the data on the current pkt pos
				custom.seek(0);		//keep custom ready to be parsed
				delete[] block;
			}

			//get address of client  ZIGLITE changed
			//NLaddress client_address;
			NLaddress client_address = addr.get_nl_address();  // REVIEW
			//nlGetRemoteAddr(sock, &client_address);

			//an empty buffer - to be filled with custom answer to client
			buffer_c answer;

			//ask gameserver what's the answer
			bool send_response = beh->leet_get_server_info(client_address, custom, answer);

			//if gameserver tells us to send the answer, then send it!
			if (send_response) {

				char blah[200];
				nlAddrToString(&client_address, blah);
				con->xprintf(2,"sending serverinfo response to : %s\n", blah);

				nlSetRemoteAddr(sock, &client_address);

				//ignore nlWrite() errors.
				buffer_c anspkt;
				anspkt.putShort( 0 ); // ZIGLITE - NEW LEET proto on game packets port
				anspkt.putByte( (NLubyte) STC_SERVERINFO );
				anspkt.putBlock( answer.data(), answer.size() );
				anspkt.seek(0);
				anspkt.getDataToSocket(sock, anspkt.size());
			}
		}
		//unknown command
		else {
			// what else could we do?
			// just ignore it...
		}
	}
	else {
		// malformed packet -- ignore
		// FIXME/REVIEW: this is a possible place for counting malformed packets from
		//               a remote IP address and eventually ban/suspend it.
	}
}

// a remote station has called connect() towards us. return true to accept, false to reject.
bool server_c::msocket_accept_station(address_c &addr, const buffer_c& custom, buffer_c& answer) {

	//used by the cut-and-pasted code below
	NLaddress client_address = addr.get_nl_address();

	//it's a new player. (station_c/msocket_c already deal with repeated "CONNECT" packets from same IP:PORT)
	// must allocate a local server port for him and add him to the client <map>.

	//the client, will be != 0 if found == true;
	// avoid warning (warning: unused variable 'cl')
	//remote_client_c *cl = NULL;

	//it's a new client:
	// avoid warning (warning: unused variable 'allocated_new_client_id')
	//int allocated_new_client_id = -1;  // -1 == none yet

	//paranoid check: more than HARDCODED_CLIENT_LIMIT clients try to connect!
	if (clients.size() >= HARDCODED_CLIENT_LIMIT) {

		con->xprintf(6,"======================================================\n");
		con->xprintf(6," ERROR: ZIGNET INTERNAL SERVER LIMITATION REACHED     \n");
		con->xprintf(6,"        Incoming client connection request dropped.\n");
		con->xprintf(6,"======================================================\n");
		con->xprintf(6,"Sorry, but ZIGNET's server implementation cannot handle\n");
		con->xprintf(6,"more than %i clients connected at the same time!!\n", HARDCODED_CLIENT_LIMIT);
		con->xprintf(6,"======================================================\n");

		//send rejection reply

		// *************************
		// REVIEW: need to add some code/info to "answer" so the client_c knows the 
		//  rejection was due to lack of client IDs
		// *************************

		return false;
	}
	else {

		//client rejected?
		if (beh->leet_accept_client(client_address, custom, answer) == false) {

			// *** FIXME: restore support for silent rejection ***
			//
			//silent rejection
			//if (answer.code == 1) {
			//	LOG("LEET SERVER: silent rejection.\n"); // -- NOP --
			//	return false;
			//}
			//else

			//send rejection reply with answer = from gameserver callback
			return false;
		}
		//client accepted
		else {

			//HACK : save for the callback below
			msocket_accept_custom = custom;

			// will accept the client
			return true;
				
		}//fim gameserver accepted
	
	}//fim tem ID livre pro cara
}

// called after msocket_accept_connection() returns true - a new station is created for 
// the incoming connection request.
// NOTE: the created station knows how to handle duplicate connection packets that can arrive 
// after this call.
void server_c::msocket_station_connected(station_c &st, const buffer_c& oConnectionData) {

	//it's a new client:
	int allocated_new_client_id = -1;  // -1 == none yet

// the code below is the default (new version)
#ifndef ZIG_USE_COMPACT_CLIENT_ID_GENERATOR

	// NEW (v1.5.0): do not always immediately reuse "holes" on the client ID value domain. instead 
	// of searching for the first "hole", increase a "search pointer" that wraps around the player ID 
	// values and skips over any already-used ID values that it finds.

	while (true) {
		
		// next client ID, wraps around
		client_id_generator++;
		if (client_id_generator >= HARDCODED_CLIENT_LIMIT) {
			client_id_generator = 0;
		}

		//try to find client #"client_id_generator" in map
		map<int, remote_client_c*>::iterator cl_it = clients.find( client_id_generator );
		
		// client not found, ID is unused
		if (cl_it == clients.end()) {

			allocated_new_client_id = client_id_generator;  // save

			break; // OK! stop search for the ID
		}
	}

#else // #ifndef ZIG_USE_COMPACT_CLIENT_ID_GENERATOR

	// use the OLD (pre-v1.5.0) way of generating client IDs ("compact" method)

	//find an unused id in the map -- allocate to client
	bool ids[HARDCODED_CLIENT_LIMIT];
	int ivar;	//IMPORTANT-used later!
	for (ivar=0;ivar<HARDCODED_CLIENT_LIMIT;ivar++) //clear all
		ids[ivar]=false;
	map<int, remote_client_c*>::iterator it = clients.begin(); //marking all ids found....
	while (it != clients.end()) {
		ids[(*it).first]=true;
		it++;
	}
	for (ivar=0;ivar<HARDCODED_CLIENT_LIMIT;ivar++) //find it, result in "i"
		if (ids[ivar]==false) 
			break;

	//NEW: following code now expects "allocated_new_client_id" instead of "ivar"
  allocated_new_client_id = ivar;

#endif // #ifndef ZIG_USE_COMPACT_CLIENT_ID_GENERATOR #else

	// create client
	remote_client_c *cl = new remote_client_c(this, &st, con);

	//ZIGLITE 1.1: set the default max outgoing packet sizes
	cl->station->set_max_outgoing_packet_size(cfg_max_packet_size);

	// FIXMEV150 DEBUGGING
	cl->station->set_console(con);

	//v1.5.0: DEBUGGING
	// a small debugging aid to help differentiate client-side and server-side 
	// stations, for when running debug mode with both clients and a servers 
	// instantiated on the same process.
	cl->station->set_debug("server");

	//NEW v1.5.0: new client ID generation
	cl->id = allocated_new_client_id;
	
	//THIS IS VERY IMPORTANT. nowhere else to place...
	// ZIGLITE NEW: callback interface is the remote_client_c, not server_c , so we don't
	//  need to pass an additional ID...
	cl->station->set_callback_interface( cl );   //was: ( this )

	//NEW: setup compression!
	if (server_compression_config != -999)  //magic flag meaning: LEAVE AT DEFAULT
		cl->station->set_compression( server_compression_config );
		
	//NEW: maybe a bug fix
	cl->last_received_packet = get_time() + 3.0;		// initialize the timeout counter (+3s = bonus paranoia)

	//add client to map, with ID == "i" and remote_client_c == "cl"
	clients[cl->id] = cl;

	// call back app
	// FIXME : use ", bool forced_accept, buffer_c& answer) {" additional (NEW!) parameters!
	//         I think this "msocket_accept_custom" breaks if we get simultaneous connection attempts.
	//         But then again, that doesn't happen in a client-server setup. No problems. Nothing to see here, move along...
	beh->leet_client_connected( cl->id, msocket_accept_custom );
}



// -------------------- REMOTE_CLIENT_C -----------------------------------

remote_client_c::~remote_client_c() {
	address_c station_addr = station->get_remote_address();
	(server->msocket).delete_station(station_addr);
}

// incoming data read by a station from the msocket. it can either be "special" packets 
// (packet_id == 0) or "regular"(=="normal") game packets (packet_id > 0 && packet_id <= 32512)
void remote_client_c::station_incoming_data(buffer_c &pkt, int packet_id) {
	server->incoming_client_data(this, pkt, packet_id);
}

// no need to implement this
// server does not connect to clients

void remote_client_c::station_connected(buffer_c& connection_data, const bool forced) {}

void remote_client_c::station_connection_timed_out() {}

void remote_client_c::station_connection_refused(buffer_c& reason) {}

// FIXME/REVIEW : this was added by Diego M to get his disconnect request/response protocol working. 
//   must merge this with the disconnect_client() routine or document better why the need for two 
//   separate disconnection methods ("immediate" vs. "")
void server_c::immediate_disconnect_client(int client_id) {
	remote_client_c* cl = get_client(client_id);
	if (cl != NULL) client_disconnection_finalization(cl);
}

/// Connection aborted
void remote_client_c::station_disconnected(bool remote, buffer_c& reason) {

	// FIXME/REVIEW: care for remote & reason ??

	server->client_disconnection_finalization(this);
}
