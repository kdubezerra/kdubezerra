/*

	gameserver

	the mini-game server

*/

#ifndef _GAMESERVER_H_
#define _GAMESERVER_H_


//derive your gameserver class from "zigserver_c"
class gameserver_c : public zigserver_c {
public:

	//ctor
	gameserver_c(console_c *conio);

	//dtor
	virtual ~gameserver_c();

	//we're not using those methods on this mini-game:
	virtual void client_ping_result(int client_id, int ping_value);

	//abstract methods from parent class that we will implement
	virtual bool get_server_info(address_c &remote_addr, buffer_c &custom, buffer_c &answer);
	virtual bool accept_client(address_c &client_addr, buffer_c &custom, buffer_c &answer);
	virtual void client_connected(int client_id, buffer_c &custom);
	virtual void client_disconnected(int client_id, int zig_reason);
	virtual void client_incoming_data(int client_id, buffer_c &in, int packet_id);
	
	//the methods below are optional to override, but you should do so.
	virtual bool init();
	virtual bool update_frame();
	virtual bool broadcast_frame();
	virtual void finish();

protected:

	//count the number of connected players
	int client_count;

	//the server-side game state, very minimal stuff (not even using the C++ STL)
	player_t  player[ MAX_PLAYER_ID ];
};



#endif

