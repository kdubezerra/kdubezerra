/*

	gameserver

	the mini-game server

*/

#include "main.h"
#include "gameserver.h"
#include <stdio.h>

// debugging aid
int broadcast_count = 0;
double broadcast_start_time = 0.0;

//ctor
//the catch: almost all of ZIG's classes expect a "console" pointer in the constructor - it's where
//the classes send their debug messages.
gameserver_c::gameserver_c(console_c *conio) : zigserver_c(conio) {
	//set_compression(0);   // for testing compression.

	// since we're sending 20 packets per second, this limits throughput to 5120 bytes/s
	set_max_outgoing_packet_size(256);
}

//dtor
gameserver_c::~gameserver_c() {
}

// ------------------------------------------------------------------------------------------------
//   we're not using those methods on this mini-game:
// ------------------------------------------------------------------------------------------------

void gameserver_c::client_ping_result(int client_id, int ping_value) { 
	//just testing...
	//con->printf("%i = %i (%i)\n", client_id, ping_value, get_client_latency(client_id));
}

// ------------------------------------------------------------------------------------------------
//    abstract methods from parent class that we will implement:
// ------------------------------------------------------------------------------------------------

// somebody on the internet is asking the server for custom information.
// "remote_addr" is the NLaddress of the computer that is asking for the information
// "custom" is custom data the remote computer sent us to help on providing the answer or
// something like that (may be empty)
// "answer" is a buffer to be filled by this method implementation with the answer to give to
// the remote computer.
// return "false" to ignore the request, return "true" to send the "answer" buffer to the remote
// computer.
// in this demo: we will return the number of players that are playing and some more info on a
// descriptive string.
bool gameserver_c::get_server_info(address_c &remote_addr, buffer_c &custom, buffer_c &answer) {

	char text[1024];
	sprintf(text, "ZIG Server running the ZIG Library Minimalist Game Demo.\n  Number of players playing right now = %i\n", client_count);
	answer.putString(text);
	return true;
}

// this method is called when a client is trying to connect to us.
// return "true" from this method to accept the remote game connection.
// return "false" from this method to reject the remote game connection.
// "client_addr" is the HawkNL address of the client (you could check this agains a "banned IPs" list
// or something like that.
// "custom" is the data buffer the client sent to us together with it's "want to connect" request. this
// custom data could include some credentials (passoword) or whatever
// "answer" is a custom answer buffer. anything you write in this buffer will be sent back to the
// client.
bool gameserver_c::accept_client(address_c &client_addr, buffer_c &custom, buffer_c &answer) {

	//we accept all clients - no questions asked, no custom info needed
	return true;
}

// a client has connected. this is called after you return "true" in the accept_client() method.
// "client_id" is the unique ID of the client that has connected. save it somewhere.
// "custom" is the same parameter that was passed to the zigserver_c::accept_client() callback for
//   this same client.
void gameserver_c::client_connected(int client_id, buffer_c &custom) {

	con->printf("server: client %i connected.\n", client_id);

	//if too many clients, disconnect now.
	//if disconnecting because of too many clients, then stuff a dummy byte in the answer (that's our protocol)
	if (client_count >= MAX_CLIENTS) {
		buffer_c answer;
		answer.putByte( 0 );
		disconnect_client(client_id, answer);
		return;
	}

	//another one!
	client_count ++;

	//a temp buffer
	buffer_c msg;

	// STREAM ENDPOINT CREATION: This is introduced by ZIG 2.0 and is important. Please look at the documentation 
	// (Doxygen of zigclient_c) for an explanation of streams.
	//
	// Basically, every connection can have as many streams you want. Each endpoint of a ZIG client-server connection 
	// should create matching endpoints (same amount of create_stream() calls, in the same order with the same properties).
	//
	// See gameclient_c::connected() and check that stream creation matches the calls below.
	//
	// Notice that the Stream IDs are saved individually for each client. It is expected (no, guaranteed) that the 
	// st_event of all player[] entries will be 1 and that the st_chat of all player[] entries will be 2. See the 
	// documentation for zigserver_c::create_stream(). However, we save individual values because, in principle, each 
	// connection endpoint that the server has (one to each connected client) could have its own, distinct set of 
	// streams. Why one would want that is beyond me, but the ZIG 2.0 API allows this flexibility.

	// Stream #1 (st_event): game events (player entered game, player left game, ...)
	policy_c policy = DefaultPolicy;
	policy.set_send_interval(0.0);		// HINT: set to 0.1 or higher to see the player's shadows (ublock goes faster than streams then)
	player[client_id].st_event = create_stream(client_id, STREAM_CUMULATIVE_ACK, policy);

	// Stream #2 (st_chat): chat messages
	player[client_id].st_chat = create_stream(client_id, STREAM_INDIVIDUAL_ACK, policy);

	// Stream #3 (st_positions) - update players positions
	player[client_id].st_positions = create_stream(client_id, STREAM_NO_ACK, policy);

	//a little gotcha: since this client is joining the game NOW, he doesn't know who
	//is already playing, so we must send a "player entered game" message to the newcomer
	//for every player that was already playing
	for (int i=0;i<MAX_PLAYER_ID;i++)
		if (player[i].exists == true) {
			msg.clear();		//clear buffer

			msg.putByte( (NLubyte) 1 );						//message code, 1 == "player entered game"
			msg.putByte( (NLubyte) i );		        //what player ID

			send_message(client_id, msg, player[client_id].st_event, true);					//send the message just to the new player
		}

	//insert into the list. "player id" is the same as "client id" for our purposes.
	player[client_id].exists = true;

	//set player's mouse coordinates to some sensitive defaults.
	player[client_id].mx = 0;
	player[client_id].my = 0;

	// Broadcast a message for everybody telling that a given client has just connected
	//
	// NOTE: the new zigserver_c class of ZIG 2.0 doesn't support broadcast_message(). You have to manually iterate
	// through all clients and call send_message() to each client. This is because the current way streams work: the 
	// server must create stream endpoints for each client connection independently, so there is no guarantee (although 
	// it should be highly expected to be so) that, for instance, a game's "chat stream" is created on all client 
	// connections, and that it will have the same Stream ID on all client connections. This could be improved in ZIG 2.1.
	//
	for (int i=0;i<MAX_PLAYER_ID;i++)
		if (player[i].exists == true) {
			msg.clear();		//clear buffer

			msg.putByte( (NLubyte) 1 );						//message code, 1 == "player entered game"
			msg.putByte( (NLubyte) client_id );		//what player ID

			send_message(i, msg, player[client_id].st_event);
		}
}

// a client has disconnected, for whatever reason. "client_id" is the unique ID of the client.
// you can use this method to de-allocate player information associated with a given client ID.
void gameserver_c::client_disconnected(int client_id, int zig_reason) {

	con->printf("server: client %i disconnected. zig reason = %i\n", client_id, zig_reason);

	//less one...
	client_count --;

	//remove from the list
	player[client_id].exists = false;

	// NOTE: the new zigserver_c class of ZIG 2.0 doesn't support broadcast_message(). You have to manually iterate
	// through all clients and call send_message() to each client. This is because the current way streams work: the 
	// server must create stream endpoints for each client connection independently, so there is no guarantee (although 
	// it should be highly expected to be so) that, for instance, a game's "chat stream" is created on all client 
	// connections, and that it will have the same Stream ID on all client connections. This could be improved in ZIG 2.1.
	//
	buffer_c msg;
	for (int i=0;i<MAX_PLAYER_ID;i++)
		if (player[i].exists == true) {
			msg.clear();		//clear buffer

			msg.putByte( (NLubyte) 2 );						//message code, 1 == "player entered game"
			msg.putByte( (NLubyte)client_id );		//what player ID

			send_message(i, msg, player[client_id].st_event);
		}
}

// incoming data from the client!
// this method is called when an UDP game packet from a given client arrives.
// "in" is the "unreliable data" sent by the client in this packet.
// "packet_id" is the ID of this game packet (may be of some use...)
// "late" is true when this packet was sent before at least one other packet that was already received 
void gameserver_c::client_incoming_data(int client_id, buffer_c &in, int packet_id) {

	// Update the client's latest mouse-x and mouse-y according to the unreliable data block (client 
	//  "inputs" block).
	// This is redundant with the "st_positions" stream information. This is for testing performance 
	//  differences between the unreliable data block and streams of type STREAM_NO_ACK, which is the 
	//  case for the "st_positions" stream.
	player[client_id].mx_u = in.getShorts();
	player[client_id].my_u = in.getShorts();

	// This is a loop that pumps all received messages on all streams of the client "client_id", which is the 
	// client that just received an UDP packet which triggered this client_incoming_data() callback we're in.
	//
	buffer_c msg;
	int stream_id;

	// let's iterate through all server-side stream endpoints of this remote client
	seek_first_stream( client_id ); 

	// while there are streams, stream_id will point to it
	while ((stream_id = get_next_stream( client_id )) != -1) { 

		// while there are messages to be received on stream_id of client_id ...
		while (receive_message(client_id, msg, stream_id) == 1) {

			// event stream
			if (stream_id == player[client_id].st_event) {

					// this game's server expects no events from clients.
			}
			// chat stream
			else if (stream_id == player[client_id].st_chat) {

				// it is a single string. 
				string chat_str = msg.getString();

        // append the client's ID to the message string
				chat_str = itoa(client_id) + ": " + chat_str;

				// build a buffer for send_message()
        buffer_c ansbuf;
				ansbuf.putString(chat_str);

				// send the chat message to all clients through their st_chat stream endpoints
				for (int i=0;i<MAX_PLAYER_ID;i++) {
					if (player[i].exists == true) {
						send_message(i, ansbuf, player[i].st_chat);
					}
				}
			}
			// update player position
			else if (stream_id == player[client_id].st_positions) {

				//the client will send us two 2-byte signed integers (NLshort) telling the most
				//up-to-date position if it's mouse pointer.

				if (msg.size_left() >= sizeof(NLshort) * 2) {
					player[client_id].mx = msg.getShorts();
					player[client_id].my = msg.getShorts();
				}
			}
		}
	}
}

// ------------------------------------------------------------------------------------------------
//   the methods below are optional to override, but you should do so.
// ------------------------------------------------------------------------------------------------

//this is called once, use this to initialize your game server's stuff
//return "false" to prevent the server from starting and exit with an error code.
bool gameserver_c::init() { 

	con->printf("gameserver_c::init() call.\n");

	// in this mini-demo-game, we will run the game "logic" at 20Hz and the net update at 20Hz too:

	// 0.05 seconds == 50 miliseconds = 20 times per second (20Hz). 
	// server tick is the "logic" rate (update_frame() calls rate)
	set_server_tick(0.05);
										
	// one net update per game state update (or: one update_frame() call for	every broadcast_frame() call)
	set_net_ratio(1);
	
	//initialize the "world state":
	for (int i=0;i<MAX_PLAYER_ID;i++)
		player[i].exists = false;			// no connected players, nobody playing...

	//initialize the "players connected" counter to zero.
	client_count = 0;

	return true;
}

//this is called periodically (see init()) so you can update your game's physics (move objects around,
//test for collisions, etc.).
bool gameserver_c::update_frame() { 

	//we don't have anything to do in here because we do all "world updating" we need in client_incoming_data()
	
	return true; 
}

//this is called periodically (see init()) so you can send updates (UDP packets mainly) to all connected
//clients.
bool gameserver_c::broadcast_frame() { 

	static NLushort turn = 1;
	int i;
	
	// debugging: update the counter of broadcasts so we can figure out the amount of broadcast_frame() calls
	// per second that we are actually getting (compare with the preconfigured rate)
	if (broadcast_count == 0)
		broadcast_start_time = get_time();
	broadcast_count++;
	if (broadcast_count > 200) {  // forget current "average" from time to time
		broadcast_count = 0;
		broadcast_start_time = get_time();
	}

	double now = get_time();

	// All connected clients will receive the same update, since everybody is seeing everybody else.
	//
	// The updates are distributed through two means: 
	//   (1) "positions" buffer which is filled below is sent through the "st_positions" stream;
	//   (2) "udata" buffer which is also filled below is sent through the "unreliable data block".
	// This is for comparing the performance of STREAM_NO_ACK streams and the unreliable data block.
	
	// (1) "positions" buffer stream message: the format of the update is just a list of a 7-byte record:
	//
	//  UNSIGNED BYTE:   ID of a player
	//  SIGNED SHORT:    mouse-x of that player (as received by the last client-->server message through a stream of type STREAM_NO_ACK)
	//  SIGNED SHORT:    mouse-y of that player (as received by the last client-->server message through a stream of type STREAM_NO_ACK)
	//  UNSIGNED SHORT:  stream test (for debugging only)
	//
	buffer_c positions;

	// (2) "udata" buffer will be the unreliable data block for every outgoing packet, one packet for each 
	//   connected client. The format of the update is the same as the above, but without the last unsigned short:
	//
	//  UNSIGNED BYTE:   ID of a player
	//  SIGNED SHORT:    mouse-x of that player (as received by the last client-->server message through the unreliable data block)
	//  SIGNED SHORT:    mouse-y of that player (as received by the last client-->server message through the unreliable data block)
	//
	buffer_c udata;

	// Fill both (1) and (2) at once:
	for (i=0;i<MAX_PLAYER_ID;i++) {

		// For each connected player, add a 5-byte record to the update buffer which describes its current state.
		if (player[i].exists == true) {

			// this uses "mx" and "my", which are collected from clients through their "st_positions" streams as well.
			positions.putByte( (NLubyte) i );									//append player ID to the update (stream)
			positions.putShorts( (NLshort) player[i].mx );		//append player's mouse-x to the update (stream)
			positions.putShorts( (NLshort) player[i].my );		//append player's mouse-y to the update (stream)
			positions.putShorts( turn );

			// this uses "mx_u" and "my_u", which are collected from clients through their ublocks as well.
			udata.putByte( (NLubyte) i );										//append player ID to the update (ublock)
			udata.putShorts( (NLshort) player[i].mx_u );		//append player's mouse-x to the update (ublock)
			udata.putShorts( (NLshort) player[i].my_u );		//append player's mouse-y to the update (ublock)

			//con->xprintf(3, "Server: send packet number %d\n", turn);
		}
	}

	// OK, now we're ready to send UDP packets to all connected clients:
	for (i=0;i<MAX_PLAYER_ID;i++) {
		if (player[i].exists == true) {

			// Send a message to player "i" whose contents are "positions", through the "st_positions" stream of 
			//  every client. Contains all position updates for all clients, as received from those same clients, 
			//  by the server, through each client-server connection's "st_positions" stream (STREAM_NO_ACK type, 
			//  which emulates the unreliable data block). -- that is, the same stream from which the updates are 
			//  gathered.
			send_message(i, positions, player[i].st_positions, false);

			// Send a packet to player "i" with "udata" as the unreliable data block of the packet (contains 
			//  all position updates for all clients, as received from those same clients, by the server, through 
			//  each client-server connection's unreliable data blocks -- which are set by the "input_*" methods 
			//  of zigclient_c).
			send_packet(i, udata);
		}
	}

	turn++;
	return true; 
}

//this is called once, when the server is shutting down.
void gameserver_c::finish() { 

	con->printf("gameserver_c::finish() call.\n");
}

