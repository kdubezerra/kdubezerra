/*

	gameclient

	the mini-game client

*/

#ifndef _GAMECLIENT_H_
#define _GAMECLIENT_H_

#include "zig/station.h"
#include "zig/zigclient.h"

//derive your gameclient class from "zigclient_c"
class gameclient_c : public zigclient_c {
public:

	//ctor
	gameclient_c(console_c *conio);

	//dtor
	virtual ~gameclient_c();

	//abstract methods from parent class that we will implement
	virtual void connected(buffer_c &hello);
	virtual void connection_timed_out();
	virtual void connection_refused(buffer_c &reason, int code_reason);
	virtual void disconnected(bool server_initiated, buffer_c &reason, int code_reason, int zig_reason);
	virtual void incoming_data(buffer_c &in, int packet_id);
	virtual void incoming_server_info(address_c &addr, buffer_c &info, int time_ms, bool timedout);

	//the methods below are optional to override, but you should do so.
	virtual void before_packet_send();
	virtual bool init();
	virtual void finish();

	//gameclient methods not directly related to the inheriting from zigclient_c
	bool game_draw_frame();			//update game screen
	bool game_read_input();			//read new player input
	void chat_string_from_console(char *str);		//chat string entered from the GUI (console)

protected:

	//a fancy helper method
	void set_status(char *new_status);			

	//showing the full-screen console?
	bool conshow;

	//used to force the console's screen refresh
	bool refresh_console;

	//are we connected?
	bool game_connected;

	//the latest game status screen (to draw on screen)
	char last_status[256];

	//the local IP addresses
	int naddr;
	string laddr[16];	

	//the client-side game state, very minimal stuff. I'm not even using the C++ STL.
	player_t  player[ MAX_PLAYER_ID ];

	//stream IDs
	int st_event, st_chat, st_positions;
};



#endif

