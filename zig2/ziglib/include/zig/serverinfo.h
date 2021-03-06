/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, F�bio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*

	serverinfo

	auxiliary class for:
	- finding servers (IP addresses) in local area network (LAN) (uses LAN 
	  UDP broadcast)
	- querying known servers (by IP address or hostname) for custom status 
	  information (current number of players, current type of game, 
		settings, etc)

*/

/*! \file serverinfo.h
    \brief serverinfo.h provides a mechanism for getting information from game servers 
		on known addresses and for discovering servers in the local network.
*/

#ifndef _ZIG_SERVERINFO_H_
#define _ZIG_SERVERINFO_H_

#include "zig.h"
//#include "thread.h"
#include "address.h"
#include "buffer.h"
#include "console.h"
#include "utils.h"
#include <list>

/*! \brief Interface that must be implemented by classes (objects) that want to receive callbacks from 
		serverinfo_c.

		Please see the documentation of serverinfo_c for an overview of this server query feature, which offers 
		support for building game-server browsers.
*/
class serverinfo_listener_i {
public:

	/*! \brief Called when a server answers to a serverinfo_c::get_server_info() query.

			\param addr Address of the remote server (IP or hostname, and port number);
			\param info Information replied by the server. This is the actual answer, which is generated by 
			the implementation of zigserver_c::get_server_info();
			\param time_ms Delay (in milliseconds) between sending the request and receiving this response.
	*/
	virtual void serverinfo_result(address_c &addr, buffer_c &info, int time_ms) = 0;

	/*! \brief Called when an answer to a serverinfo_c::get_server_info() query times out.

			\param addr Address of the remote server (IP or hostname, and port number) that didn't answer;
			\param time_ms Delay (in milliseconds) between sending the request and receiving this time-out 
			notification.
	*/
	virtual void serverinfo_timeout(address_c &addr, int time_ms) = 0;

	/*! \brief Called when a server answers to a serverinfo_c::get_local_server_info() query.

			\param addr Address of the remote server (IP or hostname, and port number) that is answering the query;
			\param info Information replied by the server. This is the actual answer, which is generated by 
			the implementation of zigserver_c::get_server_info();
			\param time_ms Delay (in milliseconds) between sending the request and receiving this response.
*/
	virtual void serverinfo_local_result(address_c &addr, buffer_c &info, int time_ms) = 0;

	/*! \brief Called when a serverinfo_c::get_local_server_info() query runs out of time (finishes).
			
			This method is always called after a serverinfo_c::get_local_server_info() query is made, but only 
			after the query runs out of time (which is the \c timeout_ms parameter of the query). 
		
			This method is always called after the timeout specified for the query, whether any servers were 
			found on the local network or not. It basically signals that a LAN scan is completed. 

			\param time_ms Total time elapsed during the LAN scan, in milliseconds.
			
			\see serverinfo_c::get_local_server_info() for more details on scanning the LAN for servers.
	*/
	virtual void serverinfo_local_timeout(int time_ms) = 0;

	virtual ~serverinfo_listener_i() { }
};

// internal helper classes defined in serverinfo.cpp
class serverinfo_result_c;
class serverinfo_query_c;

// serverinfo_c class implements the serverinfo functionality that was previously available as 
// part of zigclient_c.
// to use this new functionality, you must extend a class "X" from the serverinfo_listener_i class, 
// create a new serverinfo_c instance "S" and call "S.init(X)". 
// NOTE: EACH instance of serverinfo_c class can handle MANY serverinfo requests and responses 
// concurrently, and you can also run MANY serverinfo_c instances concurrently (they won't affect 
// each other)

/*! \brief Provides support for implementing game-server browsers, by asking ZIG servers for information 
		and gathering the answers.
		
    The "server info" feature was developed to solve the following problem: 
		
		A game player, running a ZIG client (zigclient_c) instance on his/her machine, has the address 
		(IP:PORT or HOSTNAME:PORT) of many remote hosts that are running ZIG servers (zigserver_c) of 
		his/her favorite 3D shooter game. The player wants to decide in which server he/she will connect, 
		but doesn't know what is currently "going on" in each of these servers. So, the game client should 
		query each known server to obtain some server information, such as, current "game map" being 
		played, the current number of players on the server, the current communication latency from the 
		player's machine to the server, etc. So, the game client asks each server for this information, 
		and gather the answers, displaying them for the user, for instance, in a "server browser" interface.

		How to use this class in your application (quick overview):

		- Create a class that extends from class serverinfo_listener_i, implementing all of the virtual methods 
		specified by serverinfo_listener_i;
			- HINT: since zigclient_c already extends from serverinfo_listener_i, and extending from zigclient_c is 
			already mandatory, then just use your "game client" class for serverinfo result handling.
		- Create a serverinfo_c object;
		- To initialize the serverinfo_c object, call init(), passing a pointer to an instance of the class that 
		extends serverinfo_listener_i;
		- To make the serverinfo_c object work its magic, put a call to process_nonblocking() somewhere on your 
		game's main loop (anywhere will do);
		- Call any one of the get_server_info() or get_local_server_info() to ask a remote ZIG server for information.
		The answers will come as calls to the serverinfo_listener_i object you passed to the init() call.

		Of course, you also have to write the server-side code that answers to your game client's serverinfo queries!
    For that, you just have to implement zigserver_c::get_server_info(), which is called automatically by ZIG 
		whenever a serverinfo query is received by your ZIG game server!

		Example (pseudo-code):

		<pre>
		<a>// the class that implements the client process of our cool client-server game
		<a>// NOTE: zigclient_c already extends from serverinfo_listener_i !!!
		class gameclient : public zigclient_c {

			virtual void serverinfo_result(address_c &addr, buffer_c &info, int time_ms) {

				<a>//server "addr" answered "info" in "time_ms" milliseconds! ...
			}
			<a>//...
		};

		<a>//...

		gameclient client;		<a>// the object that receives serverinfo answers
		serverinfo_c browser;		<a>// the object that manages serverinfo queries

		browser.init( & client );
		browser.get_server_info( address_c("game.somehost.com:37500") );

		<a>// gameclient may receive one call to serverinfo_result(), if the server 
		<a>// at "game.somehost.com" and at port "37500" answers the serverinfo request.
		</pre>

		Notes:

		- You don't have to actually use the serverinfo_c class. The zigclient_c class has serverinfo functionality 
		built-in, thanks to backwards compatability with previous versions of the ZIG library. You only need this 
		class if you want two different server query managers (say, for two independent server browsers).

		- Each instance of serverinfo_c class can handle MANY serverinfo requests and responses concurrently, and you 
		can also run MANY serverinfo_c instances concurrently and they won't affect each other.
*/
class serverinfo_c {
public:

	/*! \brief Default constructor: please call init() after this, or else the object won't work!
	*/
	serverinfo_c() { con = 0; listener = 0; };

	// ----------------------------------------------------------------------------------------
	//    OBJECT LIFETIME
	// ----------------------------------------------------------------------------------------

	/*! \brief Configures the object: must be called once before calling any other methods of this object.

			This method is used to give a pointer to a serverinfo_listener_i object to the serverinfo object. 
			This listener object is called by the serverinfo object when an answer to a previous serverinfo 
			query arrives, or when a serverinfo query times out. So, it is important that init() is called, 
			with a pointer to a valid serverinfo_listener_i object, before issuing any serverinfo queries 
			through calls to any of the get_server_info() or get_local_server_info() objects. Otherwise, 
			there will be no way to receive the serverinfo answers.

			\param listener A pointer to an object whose class implements the serverinfo_listener_i interface, 
			which will receive callbacks from this serverinfo_c object whenever there is an answer (or time-out) 
			to a serverinfo query.

			\param con Optional parameter: pointer to a console_c object which will receive messages issued 
			by the serverinfo object.
	*/
	void init(serverinfo_listener_i *listener, console_c *con = 0) { 
		this->listener = listener; 
		this->con = con;
	}

	/*! \brief Allows the serverinfo object to work with pending tasks. <b>MUST BE CALLED PERIODICALLY AND FREQUENTLY!</b>

			You must place a call to process_nonblocking() somewhere on your game's main loop. If it is called anywhere 
			from 1 to 1000 times per second, it is fine. Between 10 and 20 is very nice.

			Once called, this method carries out the following tasks:

			- If it is time, then send outgoing serverinfo query packets (this is for any number of current active queries);
			- If any results have arrived, then call back the serverinfo_listener_i object (set by the init() call) to deal 
			with the results;
			- Detect any queries that have timed out and call back the serverinfo_listener_i object to deal with the 
			timed-out query.
	*/
	void process_nonblocking();

	// ----------------------------------------------------------------------------------------
	//    SERVICE METHODS
	// ----------------------------------------------------------------------------------------

	/*! \brief Asks a specific game server for general information. 

			This method sends a general ("empty") query to a game server running on the given remote host IP address 
			and port. By "empty" query is meant a query without parameters. 

			The result of this query will be informed by way of one serverinfo_listener_i callback:
			- If the query is answered on time by the server, then it will be a serverinfo_listener_i::serverinfo_result();
			- If the query times out, then it will be serverinfo_listener_i::serverinfo_timeout() instead.

			One of those two is guaranteed to be called for each query.

			Remember that the ZIG library's server-side code, when receiving a serverinfo query from the network, will 
			just call the method zigserver_c::get_server_info(). You have to implement this method in your "game server" 
			class which extends from zigserver_c, and this implementation is the code that actually answers all 
			get_server_info() queries (answers which end up as serverinfo_listener_i::serverinfo_result() callbacks on 
			the client-side). 

			\param server_addr Full address of the server: IP address (or hostname) and port number;
			\param timeout_ms Query timeout, in milliseconds. Default is 3000 milliseconds.
	*/
	void get_server_info(address_c &server_addr, int timeout_ms = 3000);

	/*! \brief Asks a specific game server for specific information.

			This method sends a specific query to a game server running on the given remote host IP address and port. 
			By "specific query" is meant a query with arguments. All arguments to the query are encoded in the 
			\c custom parameter (which is just a buffer of bytes and can contain anything).
      
			For instance, in the \c custom buffer, you could encode one int argument to the query, which is the kind 
			of information you want to receive: \c 1 for the list of players that are currently playing the game, \c 2 
			for the list of "game maps" in the map rotation of the server, \c 3 for the highscores, etc. You can use 
			the \c custom parameter in whatever way you like. The server will receive the buffer as-is and give it to 
			your server-side code at zigserver_c::get_server_info() to handle it.
			
			The result of this query will be informed by way of one serverinfo_listener_i callback:
			- If the query is answered on time by the server, then it will be a serverinfo_listener_i::serverinfo_result();
			- If the query times out, then it will be serverinfo_listener_i::serverinfo_timeout() instead.

			One of those two is guaranteed to be called for each query.

			Remember that the ZIG library's server-side code, when receiving a serverinfo query from the network, will 
			just call the method zigserver_c::get_server_info(). You have to implement this method in your "game server" 
			class which extends from zigserver_c, and this implementation is the code that actually answers all 
			get_server_info() queries (answers which end up as serverinfo_listener_i::serverinfo_result() callbacks on 
			the client-side). 
						
			\param server_addr Full address of the server: IP address (or hostname) and port number;
			\param custom A block of bytes which is sent as argument to the query;
			\param timeout_ms Query timeout, in milliseconds. Default is 3000 milliseconds.
	*/
	void get_server_info(address_c &server_addr, buffer_c &custom, int timeout_ms = 3000);

	/*! \brief Asks all game servers on the local network (LAN) for general information. 

			This method sends a serverinfo query to all servers that are on the same local network (LAN) and 
			that are listening at a given UDP broadcast port.

			This method sends a general ("empty") query. By "empty" query is meant a query without parameters. 

			If one or more servers on the LAN answer to the the query, the result will come as one or more 
			serverinfo_listener_i::serverinfo_local_result() callbacks. One callback will be generated for each 
			server that was found.

			After \c timeout_ms milliseconds have passed since the query was made, one call to 
			serverinfo_listener_i::serverinfo_local_timeout() is made. This signals to the game code that the 
			LAN scan is completed. In other words: since the query is not for a specific server but for the 
			whole LAN, the query is not completed when one answer is received. The query remains active after 
			a response being received. So the only way that a get_local_server_info() call finishes is with a 
			timeout.

			Remember that the ZIG library's server-side code, when receiving a serverinfo query from the network, will 
			just call the method zigserver_c::get_server_info(). You have to implement this method in your "game server" 
			class which extends from zigserver_c, and this implementation is the code that actually answers all 
			get_local_server_info() queries (answers which end up as serverinfo_listener_i::serverinfo_local_result() 
			callbacks on the client-side). 
			
			\param timeout_ms Query timeout, in milliseconds. Default is 1500 milliseconds.
			\param broadcast_port The UDP broadcast port that the server(s) are listening at. The default is 54876 
			(ZIG_DEFAULT_BROADCAST_PORT). You don't have to change this if you haven't changed it at zigserver_c.
	*/
	void get_local_server_info(int timeout_ms = 1500, int broadcast_port = ZIG_DEFAULT_BROADCAST_PORT);

	/*! \brief Asks all game servers on the local network (LAN) for specific information. 

			This method sends a serverinfo query to all servers that are on the same local network (LAN) and 
			that are listening at a given UDP broadcast port.

			This method sends a specific query. By "specific query" is meant a query with arguments. All arguments 
			to the query are encoded in the \c custom parameter (which is just a buffer of bytes and can contain anything).
      
			For instance, in the \c custom buffer, you could encode one int argument to the query, which is the kind 
			of information you want to receive: \c 1 for the list of players that are currently playing the game, \c 2 
			for the list of "game maps" in the map rotation of the server, \c 3 for the highscores, etc. You can use 
			the \c custom parameter in whatever way you like. The server will receive the buffer as-is and give it to 
			your server-side code at zigserver_c::get_server_info() to handle it.

			If one or more servers on the LAN answer to the the query, the result will come as one or more 
			serverinfo_listener_i::serverinfo_local_result() callbacks. One callback will be generated for each 
			server that was found.

			After \c timeout_ms milliseconds have passed since the query was made, one call to 
			serverinfo_listener_i::serverinfo_local_timeout() is made. This signals to the game code that the 
			LAN scan is completed. In other words: since the query is not for a specific server but for the 
			whole LAN, the query is not completed when one answer is received. The query remains active after 
			a response being received. So the only way that a get_local_server_info() call finishes is with a 
			timeout.

			Remember that the ZIG library's server-side code, when receiving a serverinfo query from the network, will 
			just call the method zigserver_c::get_server_info(). You have to implement this method in your "game server" 
			class which extends from zigserver_c, and this implementation is the code that actually answers all 
			get_local_server_info() queries (answers which end up as serverinfo_listener_i::serverinfo_local_result() 
			callbacks on the client-side). 

			\param custom A block of bytes which is sent as argument to the query;
			\param timeout_ms Query timeout, in milliseconds. Default is 1500 milliseconds.
			\param broadcast_port The UDP broadcast port that the server(s) are listening at. The default is 54876 
			(ZIG_DEFAULT_BROADCAST_PORT). You don't have to change this if you haven't changed it at zigserver_c.
	*/
	void get_local_server_info(buffer_c &custom, int timeout_ms = 1500, int broadcast_port = ZIG_DEFAULT_BROADCAST_PORT);

	// ----------------------------------------------------------------------------------------
	//    IMPLEMENTATION DETAILS
	// ----------------------------------------------------------------------------------------

private:

	//aux method - insert result/timeout in queue
	void queue_result(bool local, address_c &addr, buffer_c &info, int time_ms, bool timed_out);

	//NEW (ZIG v1.2.1): when socket polling mode is enabled, save result of serverinfo requests instead 
	// of calling from the serverinfo thread. handle multiple stacks by encoding the results in a map.
	std::list<serverinfo_result_c*> serverinfo_callbacks;

	//NEW (ZIG v1.5.0): when "nonblocking" (non-threaded) is enabled, must store current state of all 
	// ongoing queries.
	std::list<serverinfo_query_c*> serverinfo_queries;

	//debug console
	console_c *con;

	//listener interface
	serverinfo_listener_i *listener;
};

#endif

