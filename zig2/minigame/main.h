/*

	main

	main header file for the mini-game project

*/

#ifndef _MAIN_H_
#define _MAIN_H_


#pragma warning(disable: 4786)

//if not defined, will NOT include Allegro, this means the program will have
//no I/O. I'm using this to test ZIG compilation/linkage because I don't 
//know how to compile Allegro myself (shame..)
#define GAME_ALLEGRO

#ifdef GAME_ALLEGRO

//include Allegro, the "multimedia" game library of choice for this game. you can use whatever
//you want (SDL, OpenGL, DirectX, etc.) for YOUR game project.
#include <allegro.h>
#ifdef ALLEGRO_WINDOWS		//patching main / _main / WinMain link errors...
#include <winalleg.h>
#include <windows.h>
#endif

#endif // GAME_ALLEGRO

//zig includes
//
// NOTE: as of ZIG v1.2.3, the standard is to use: 
//   #include <zig/some-header.h> 
// instead of 
//   #include <some-header.h>
//
#include <zig/zigclient.h>
#include <zig/zigserver.h>

//allegro console (conalleg_c class). if you are going to use another "multimedia" game
//library and you want the console to actually render to the screen, you will have to code
//your own derived class of console_c (use conalleg.h and conalleg.cpp as a basis for this,
//it's not really complicated)
#include "conalleg.h"


		//the stuff below is used by both the gameclient_c and the gameserver_c:

		// the PORT NUMBER (UDP) used for connection
		#define GAME_PORT 25235

		//"256" is the maximum value of "client ID". for testing purposes this is enough. this is NOT
		//used as a "maximum connected clients" cap - I'm using this because there's no STL usage in
		//this example and I don't feel like implementing a "map" from scratch :-)
		#define MAX_PLAYER_ID 256

		//now, THIS is the maximum number of connected clients cap on the server. may be as high as 256.
		#define MAX_CLIENTS 4

		//a VERY SIMPLE game state record for a game player
		struct player_t {

			//does this player exists? if "false", denotes an unused entry
			bool exists;

			int mx;  //current mouse-x of this player, as transmitted by stream messages
			int my;  //current mouse-y of this player, as transmitted by stream messages
			int mcol;  //color of this player's character

			int mx_u;	//current mouse-x of this player, as transmitted by the classic unreliable data block (out of streams)
			int my_u; //current mouse-y of this player, as transmitted by the classic unreliable data block (out of streams)

			// stream IDs
			int st_event, st_chat, st_positions;
		};

#endif

