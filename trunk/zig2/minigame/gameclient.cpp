/*

	gameclient

	the mini-game client

*/

#include "main.h"
#include "gameserver.h"
#include "gameclient.h"

extern int zig_stat_zipped_bytes;
extern int zig_stat_normal_bytes;

// debugging aid (declared in gameserver.cpp)
extern int broadcast_count;
extern double broadcast_start_time;

//ctor
//the catch: almost all of ZIG's classes expect a "console" pointer in the constructor - it's where
//the classes send their debug messages.
gameclient_c::gameclient_c(console_c *conio) : zigclient_c(conio) {
	//set_compression(0);   // for testing compression

	// since we're sending 20 packets per second, this limits throughput to 5120 bytes/s
	set_max_outgoing_packet_sizes(256);
}

//dtor
gameclient_c::~gameclient_c() {
}

// ------------------------------------------------------------------------------------------------
//   we're not using those methods on this mini-game:
// ------------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
//    abstract methods from parent class that we will implement:
// ------------------------------------------------------------------------------------------------

// we have connected with the server sucessfully! "hello" is the custom data that the server
// sent together with the "connection-accepted" notification.
void gameclient_c::connected(buffer_c &hello) {

	//initialize the game state: no known players
	for (int i=0;i<MAX_PLAYER_ID;i++)
		player[i].exists = false;

	//update connected flag
	game_connected = true;

	set_status("Connected");

	//unshow console, show game!
	conshow = false;

	// STREAM ENDPOINT CREATION: This is introduced by ZIG 2.0 and is important. Please look at the documentation 
	// (Doxygen of zigclient_c) for an explanation of streams.
	//
	// Basically, every connection can have as many streams you want. Each endpoint of a ZIG client-server connection 
	// should create matching endpoints (same amount of create_stream() calls, in the same order with the same properties).
	//
	// See gameserver_c::client_connected() and check that stream creation matches the calls below.

	// Stream #1 (st_event) - game events
	policy_c policy = DefaultPolicy;
	policy.set_send_interval(0.0);		// HINT: set to 0.1 or higher to see the player's shadows (ublock goes faster than streams then)
	st_event = create_stream(STREAM_CUMULATIVE_ACK, policy); // só afeta o send

	// Stream #2 (st_chat) - chat messages
	st_chat = create_stream(STREAM_INDIVIDUAL_ACK, policy); // afeta o send e o resend

	// Stream #3 (st_positions) - update players positions
	st_positions = create_stream(STREAM_NO_ACK, policy);
}

// failed to connect.
// our connection attempt with the server has timed out (the server hasn't responded).
void gameclient_c::connection_timed_out() {

	set_status("Connection Timed Out");
}

// failed to connect.
// the server refused our connection. "reason" is the custom data that the server sent
// together with the "connection-refused" notification. "code_reason" is a custom code
// for rejection - sometimes it is set to indicated why the connection was refused.
void gameclient_c::connection_refused(buffer_c &reason, int code_reason) {

	set_status("Connection Refused");
}

// disconnected.
// this is called when our previously-estabilished game connection terminated, for whatever
// reason (normal disconnection, error, whatever)
// "server_initiated": if "true", then the server disconnected us, if "false", we have asked the
// server to disconnect.
// "reason": if server_initiated==true, then this has custom data sent by the server together with
// the "disconnected" notification.
// "code_reason": is a custom code - sometimes it is set to indicated why the disconnection occured.
void gameclient_c::disconnected(bool server_initiated, buffer_c &reason, int code_reason, int zig_reason) {

	game_connected = false;

	con->printf("gameclient::disconnected()\n");

	//if the "reason" buffer has more than 0 bytes, we know that the reason for
	//disconnection is "server full" (because that's how we defined the reason packet
	//for this demo game, see gameserver.cpp::client_connected()...)
	if (reason.size() > 0)
		set_status("Disconnected - Server is Full!");
	else {
		char sbuf[1000];
		sprintf(sbuf, "Disconnected: code: %i, zig: %i", code_reason, zig_reason);
		set_status(sbuf);
	}
}

// incoming data from server!
// this is called lots of times after the connection is accepted. this is called to tell us that a
// new UDP game packet from the server has arrived.
// "in" is the "unreliable" data inside the incoming packet
// "packed_id" is the ID of this packet (may be of some use) 
// "late" is set to true when this packet was sent before at least one other packet that was already received 
void gameclient_c::incoming_data(buffer_c &in, int packet_id) {

	//in our minimalist game protocol, the information we expect from the server is:

	// ============================
	//  UNRELIABLE DATA BLOCK
	// ============================

	// A sequence (any size) of this record of 5 bytes (one entry for each client connected to the server):
	//
	//   (NLubyte)  player-id
	//   (NLshort)  updated mouse_x of this player (as the player updated the server through their inputs/ublock channel)
	//   (NLshort)  updated mouse_y of this player (as the player updated the server through their inputs/ublock channel)

	// ============================
	//  EVENT STREAM (st_event)
	// ============================

	//   message: "player entered game"
	//   SYNTAX:
	//     NLubyte 1           // message code
	//     NLubyte <player id> // the player id of the new player
	//
	//   message: "player left game"
	//   SYNTAX:
	//     NLubyte 2           // message code
	//     NLubyte <player id> // the player id of the player that is leaving
	//

	// ============================
	//  CHAT STREAM (st_chat)
	// ============================

	//   message: a simple string to be printed in the game console
	//

	// ============================
	//  POSITIONS STREAM (st_positions)
	// ============================

	// A sequence (any size) of this record of 7 bytes (one entry for each client connected to the server):
	//
	//   (NLubyte)  player-id
	//   (NLshort)  updated mouse_x of this player (as the player updated the server through its "st_positions" stream)
	//   (NLshort)  updated mouse_y of this player (as the player updated the server through its "st_positions" stream)
	//   (NLushort) update Id (debug purposes)

	// This is a loop that parses the unreliable data block received from the server (see above for the format).
	//
	while (in.size_left() >= 5) {

		NLubyte pid = in.getByte();
		player[pid].mx_u = in.getShorts();	// Save position on "mx_u" and "my_u" to differentiate from "mx" and 
		player[pid].my_u = in.getShorts(); //  "my", which are sent exclusively through the "st_positions" streams.
	}

	// This is a loop that pumps all received messages on all streams of this client.
	//
	buffer_c msg;
	int stream_id;

	// let's iterate through all server-side stream endpoints of this remote client
	seek_first_stream();

	// while there are streams, stream_id will point to it
	while ((stream_id = get_next_stream()) != -1) {

		// while there are messages to be received on stream_id ...
		while (receive_message(msg, stream_id) == 1) {

			NLubyte pid, code;

			// event stream messages
			if (stream_id == st_event) {

				code = msg.getByte();

				//"player entered game"
				if (code == 1) {

					pid = msg.getByte();
					player[pid].exists = true;					//insert into player list
					
					//come up with a random color for this player
#ifdef GAME_ALLEGRO			
					player[pid].mcol = makecol( 64 + rand() % 192,
																			64 + rand() % 192,
																			64 + rand() % 192 );  // --- Allegro code --- makecol
#endif			
				}
				//"player left game"
				else if (code == 2) {

					pid = msg.getByte();
					player[pid].exists = false;					//remove from player list
				}
			} 
			// chat stream messages
			else if (stream_id == st_chat) {

				// it is a single string. 
				string chat_str = msg.getString();

				// print it on the console!
				con->printf("%s\n", chat_str.c_str());
			}
			else if (stream_id == st_positions) {

				while (msg.size_left() >= 7) {

					NLubyte pid = msg.getByte();
					player[pid].mx = msg.getShorts();
					player[pid].my = msg.getShorts();
					NLushort updateId = msg.getShorts();
//					con->xprintf(0, "position number %d\n", updateId);
//					con->xprintf(3, "position update %d for player - %d, %d\n", pid, player[pid].mx, player[pid].my);
				}
			}
		}
	}
}

// a previous "get server info" request sent by us is returned by the server OR timed out.
// if didn't time out, then "info" is a buffer with the response from the server.
// in this demo: the gameserver_c generates a single string in the answer.
void gameclient_c::incoming_server_info(address_c &addr, buffer_c &info, int time_ms, bool timedout) {

	if (timedout) {

		if (addr.valid() == false)  // invalid source addr indicates a BROADCAST (LAN) server info request
			con->printf("incoming_server_info(): request to LAN servers timed out.\n");
		else
			con->printf("incoming_server_info(): request to '%s' timed out.\n", addr.get_address().c_str() );
	}
	else {

		string text = info.getString();
		con->printf(
			"incoming_server_info():\nfrom address = '%s'\ntime to answer = %i milliseconds\nanswer:\n%s\n"
			, addr.get_address().c_str()
			, time_ms
			, text.c_str()
		);
	}
}

// ------------------------------------------------------------------------------------------------
//   the methods below are optional to override, but you should do so.
// ------------------------------------------------------------------------------------------------

//this is called once, when the client initializes. return "false" to make the initialization
//fail and to shutdown stuff immediately.
bool gameclient_c::init() {

	con->printf("gameclient::init() call\n");

	//console showing by default
	conshow = false;

	//not connected by default
	game_connected = false;

	//force console visual refresh
	refresh_console = true;

	//our latest status string
	set_status("Not Connected");

	//for fancy-ness: get the list of local addresses from HawkNL.
	//yes, when using ZIG you can use the HawkNL API directly. for instance, if you need TCP sockets.
	NLaddress *paddr = nlGetAllLocalAddr(&naddr);
	for (int i=0;i<naddr;i++) {
		char stradr[256];
		nlAddrToString(&paddr[i], stradr);
		laddr[i] = stradr;
	}

#ifdef GAME_ALLEGRO
	// --- Allegro drawing code ---  : initialize allegro graphics mode

	//color depth
	set_color_depth( desktop_color_depth() );

	//gfx mode: windowed
	if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) < 0) {
		con->printf("ERROR: can't set GFX mode!\n"); 
		return false; //FAILED! can't proceed
	}

	//set window switch mode
	if (set_display_switch_mode(SWITCH_BACKGROUND) == -1) {
		con->printf("WARNING: set_display_switch(BACKGROUND) failed, trying BACKAMNESIA...\n");
		if (set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
			con->printf("WARNING: set_display_switch(BACKAMNESIA) failed. leaving it at current default setting.");
	}

	//set console's rendering surface to the Allegro screen
	((conalleg_c *)con)->set_bitmap(screen, 0, 0, SCREEN_W, SCREEN_H, 10);

#endif //GAME_ALLEGRO

	return true;
}

// this method is called just before the client is about to send a packet to the server. this
// is an opportunity to determine what the client will send to the server.
// in most client-server games like Quake, the client will be sending to the server the current
// status of it's keyboard, like, if the "attack" key is pressed, etc.
void gameclient_c::before_packet_send() {

	//in this minimal game, we just send mouse information to the server - the players play
	//with just the mouse.
	
	//encode current mouse-x and mouse-y in a buffer called "data"
	buffer_c data;

#ifdef GAME_ALLEGRO	
	data.putShorts( (NLshort)mouse_x );		//2-byte signed integer  //--- Allegro input code --- (mouse_x)
	data.putShorts( (NLshort)mouse_y );		//2-byte signed integer  //--- Allegro input code --- (mouse_y)
#else
    data.putShorts( (NLshort) 0 );
    data.putShorts( (NLshort) 0 );
#endif

	// The current minigame implementation sends the player positions through two different transport 
	// mechanisms: 
	//	(1) Through a message stream of type STREAM_NO_ACK, as unreliable messages, and 
	//	(2) Through the "unreliable block" of the packet (the data channel that is out of any streams --
	//				also called "Stream 0" through the ZIG documentation).
	// This is redundant, and is done for testing purposes.
	
	// Send this client's encoded mouse positions ("data") through (1) message stream.
	send_message( data, st_positions, false );

	// Send this client's encoded mouse positions ("data") through (2) unreliable data block (on 
	//  zigclient_c, this is the "input" block which is altered by the input_* functions -- see 
	//  zigclient_c's documentation for a better explanation of the input_* functions).
	input_changed_noflush( data );
}

//this is called once, when the client shuts down.
void gameclient_c::finish() {

	con->printf("gameclient::finish() call\n");
}


// ------------------------------------------------------------------------------------------------
//   other gameclient methods (not directly related to zigclient_c)
// ------------------------------------------------------------------------------------------------

//a fancy helper method
void gameclient_c::set_status(char *new_status) {

	strcpy(last_status, new_status);
	con->printf("CLIENT STATUS: %s\n", last_status);
}

// update game screen
bool gameclient_c::game_draw_frame() {

#ifdef GAME_ALLEGRO

	// the current implementation just clears the screen and redraws everything again. we don't even
	// use double-buffering, so the display will flicker a lot, this is intentional to keep code at
	// a minimum size.

	//--- Allegro rendering code ---

	acquire_bitmap(screen);

	//if full-screen console showing, then draw it
	if (conshow) {

		con->draw_page(refresh_console);
		refresh_console = false;
	}
	//console not showing: draw the game screen
	else {
	
		//first clear the screen
		clear(screen);

		//draw status: connected, etc.
		textprintf(screen, font, 20, 10, makecol(255,255,255), "GAME CLIENT STATUS = '%s'", last_status);
		
		//statistics
		textprintf(screen, font, 20, 26, makecol(255,255,255), "in:   %5i bytes/s", zig_avg_in());
		textprintf(screen, font, 20, 38, makecol(255,255,255), "out:  %5i bytes/s", zig_avg_out());
		textprintf(screen, font, 20, 50, makecol(255,255,255), "total:%5i bytes/s", zig_avg_total());

		textprintf(screen, font, 470, 26, makecol(255,255,255), "loss: %3lf", get_loss_estimative());
		textprintf(screen, font, 470, 38, makecol(255,255,255), "delay: %3lf", get_delay_estimative());
		textprintf(screen, font, 470, 50, makecol(255,255,255), "rtt: %3lf sec", get_rtt_estimative());

		//compression gain (compression not enabled in this demo app by default so don't print)
		// textprintf(screen, font, 20, 50, makecol(255,255,255), "compression gain: %.2f %%  %i > %i", zig_avg_compression_gain(), zig_stat_normal_bytes, zig_stat_zipped_bytes);

		//fancy: draw local IP addresses
		textprintf(screen, font, 20, 80, makecol(255,255,255), "Local IP addresses:");
		for (int k=0;k<naddr;k++)
			textprintf(screen, font, 20, 100+k*15, makecol(255,255,255), "%s", laddr[k].c_str() );
		
		//draw some help text
		textprintf(screen, font, 20, 350, makecol(255,255,255), "Press <HOME> to toggle CONSOLE ON/OFF");
		textprintf(screen, font, 20, 365, makecol(255,255,255), "Press <ESC> to quit");
		textprintf(screen, font, 20, 380, makecol(255,255,255), "--- Console commands: ---");
		textprintf(screen, font, 20, 400, makecol(255,255,255), "  /c              : connect to local server");
		textprintf(screen, font, 20, 415, makecol(255,255,255), "  /c <address>    : connect to a server");
		textprintf(screen, font, 20, 430, makecol(255,255,255), "  /d              : disconnect from a server");
		textprintf(screen, font, 20, 445, makecol(255,255,255), "  /os             : start the local server");
		textprintf(screen, font, 20, 460, makecol(255,255,255), "  /cs             : stop the local server");

		//debugging broadcast timing - let's see how many updates per second the local server is sending
		// to it's connected clients:
		double brdt = get_time() - broadcast_start_time;
		if (brdt <= 0.0) brdt = 1.0;
		double brps = (double)broadcast_count / brdt;
		textprintf(screen, font, 470, 388, makecol(255,255,255), "gameserver");
		textprintf(screen, font, 470, 400, makecol(255,255,255), "broadcasts/s: %.1f", brps);

		//if connected, draw all "players" on their current positions
		//
		//  NEW: this will draw spheres for both x,y send and received through the "unreliable data block"
		//   (as a shadow) and the "st_positions" stream of type STREAM_NO_ACK.
		//
		if (game_connected == true)
			for (int i=0;i<MAX_PLAYER_ID;i++)
				if (player[i].exists == true) {

					// "shadow" (ublock) -- this should not be visible if the "st_positions" stream is sending 
					//   as fast as the unreliable data block of the ZIG packets.
					circlefill(screen, player[i].mx_u, player[i].my_u, 20, makecol(80,80,80));

					// "official" player sphere (st_positions). should be drawn on top of the shadow, making it 
					//   invisible: shows that STREAM_NO_ACK can have the lowest possible latency, the same as 
					//   the "unreliable data block" of ZIG packets.
					circlefill(screen, player[i].mx, player[i].my, 20, player[i].mcol);
					
					textprintf_centre(screen, font, player[i].mx, player[i].my - 30, player[i].mcol, "Player #%i", i);
				}
	}

	release_bitmap(screen);

#endif // GAME_ALLEGRO

	return true;
}

// read new player input
bool gameclient_c::game_read_input() {

#ifdef GAME_ALLEGRO

	//the current implementation of this method does 3 things only:
	// - read keyboard to test if user pressed ESC (quit app)
	// - read keyboard to test if user pressed HOME (toggle fullscreen console on/off)
	// - if the fullscreen console is showing, read keyboard and translate some keypresses in calls
	//   to the console object so it can scroll up/down, receive command-line input, etc.

	//--- Allegro input code ---

	//read input
	while (keypressed()) {

		int key = readkey();
		int ascii = key & 0xff;							//ascii code
		int scan = key >> 8;								//scancode

		//first check for keypresses that don't care if the console is showing or if the game screen is showing
		if (scan == KEY_HOME) {
			conshow = !conshow;			//toggle it
			refresh_console = true;	//force console visual refresh
			break;
		}
		else if (scan == KEY_ESC) {

			//neat trick just for fun: print a quitting message to the console and to the screen, and call a 
			//"screen refresh" so it gets to be visible.
			set_status("Quitting...");
			game_draw_frame();

			return false;	//returning "false" here makes the whole game quit
		}
		//then check: full-screen console showing? then redirect input to the console
		else if (conshow) {

			if (scan == KEY_PGDN) 
				con->scroll_page_down();
			else if (scan == KEY_PGUP) 
				con->scroll_page_up();
			else if (scan == KEY_END) 
				con->scroll_all_down();
			else if (scan == KEY_UP)
				con->history_back();
			else if (scan == KEY_DOWN)
				con->history_forward();
			else if (ascii > 0)
				con->read_char(ascii);
		}
		//if not: full-screen console not showing. input goes to the game screen.
		else {

			//currently we play with only the mouse so don't care for the keys here
		}
	}

	return true;

#else  // GAME_ALLEGRO

    //if we can't use Allegro, then the game just quits after some time
    static int blah = 10000;
    blah--;
    return (blah < 0);
    
#endif // GAME_ALLEGRO

}

//NEW: called from <main.cpp>: non-command strings typed in console prompt are supposed to be 
// chat messages from the player, which should be sent to the server and from there to all
// clients that are connected to the game.
void gameclient_c::chat_string_from_console(char *str) {	

	// if we are connected, then send it to the server
	if (is_connected()) {

		// all messages on the st_chat stream are chat strings, so we don't need to put a message code.
		buffer_c msg;
		msg.putString(str);		// just the string

		send_message(msg, st_chat);
	}	
}


