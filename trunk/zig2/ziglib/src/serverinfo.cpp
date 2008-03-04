/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, Fábio Reis Cecin.
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

#include "zig/serverinfo.h"
#include "zig/leetnet.h"



//helper class: serverinfo query result
class serverinfo_result_c {
public:
	address_c addr;
	buffer_c info;
	int time_ms;
	bool local;     // new - cleaner
	bool timed_out; // new - cleaner
};

//helper class: serverinfo pending query
class serverinfo_query_c {
public:
	bool local;								// is it a broadcast query?
	NLlong query_id;          // ID of query (for "local" queries only)
	address_c addr;						// if not local, destination addr (copied as-is to serverinfo_result_c)
	NLsocket sock;            // socket used for sending queries and receiving the responses
	double start_time;				// start time of query, to detect timeout
	double timeout;           // clock time for the timeout, calculated after and relative to start_time
};




//get server info
void serverinfo_c::get_server_info(address_c &server_addr, int timeout_ms) {

	buffer_c empty;
	get_server_info(server_addr, empty, timeout_ms);
}

//broadcast serverinfo to LAN port
void serverinfo_c::get_local_server_info(int timeout_ms, int broadcast_port) {

	buffer_c empty;
	get_local_server_info(empty, timeout_ms, broadcast_port);
}

// this method MUST BE CALLED PERIODICALLY if the object is init()'ed for non-threaded operation.
void serverinfo_c::process_nonblocking() {

	// ignore if running in threaded mode
//	if (threaded)
//		return;

	// 1) checks for incoming QUERY RESPONSE PACKETS (for ANY of MULTIPLE active queries that can be 
	//    managed with this instance) and while at it check for timeouts.
	//
//	serverinfo_queries_mutex.lock();
	if (serverinfo_queries.size() > 0) {

		// for receiving responses
		buffer_c resp;
		
		// for all queries
		list<serverinfo_query_c*>::iterator it = serverinfo_queries.begin(); // for each pending callback...
		while (it != serverinfo_queries.end()) {

			// next query
			serverinfo_query_c *query = (*it);

			// remove current element?
			bool remove_it = false;

			// receive broadcast query response / check broadcast timeout
			if (query->local) {

				// check for timeout
				if (get_time() > query->timeout) {

					// dummy result buffer
					buffer_c info; 

					// put result in queue (local=false, addr, dummy buffer, ping_time=0, timeout=true)
					queue_result(false, query->addr, info, 0, true);

					// delete query
					remove_it = true;
				}
				else {

					//check if answer arrived
					resp.clear();
					int res = resp.putDataFromSocket(query->sock);
					if (res > 0) {

						// BROADCAST -- IMPORTANT -- get the remote address of whoever responded!
						address_c addr(query->sock);

						//calculate ping time for answer
						int ping_time = (int) ((get_time() - query->start_time) * 1000.0);

						//answer arrived?
						resp.seek(0);
						if (resp.size() > 0) {

							NLubyte code = resp.getByte();
							if (code == LEET_BCRESULT_SERVERINFO) {

								// ZIGLITE 1.1: avoid malformed packets
								if (resp.size_left() >= (int) (sizeof(NLlong) + sizeof(NLshort)) ) {

									//using filtering (useful for, e.g.: avoiding getting duplicate 
									// answers if a net client is asking too)
									NLlong incoming_query_id = resp.getLongs();

									// IGNORE if query id is different
									if (incoming_query_id == query->query_id) {

										//get request port number
										int server_port = (int)resp.getShort();

										// IMPORTANT - set port at addr 
										// broadcast answer comes from broadcast port not request port (where connects are sent to)
										addr.set_port( server_port );
									
										//response - put result data!
										buffer_c info;
										info.clear();
										info.putBlock( resp.data_cur(), resp.size_left() );
										info.seek(0);			//ready for parsing whatever is in it

										// put result in queue (local==true, sender addr, custom response, ping time, timeout==false)
										queue_result(true, addr, info, ping_time, false);

										// DO NOT delete query here, since all broadcast queries eventually time out!
									}
								}
								else {
									// malformed packet -- just ignore it.
								}
							}
						}
					}

				}// NOT TIMEOUT

			}
			// receive single-host query response / check single-host timeout
			else {

				// check for timeout
				if (get_time() > query->timeout) {

					// dummy result buffer
					buffer_c info; 

					// put result in queue (local=false, addr, dummy buffer, ping_time=0, timeout=true)
					queue_result(false, query->addr, info, 0, true);

					// delete query
					remove_it = true;
				}
				// not timeout
				else {

					resp.clear();
					
					//check if answer arrived
					int res = resp.putDataFromSocket(query->sock);
					if (res > 0) {

						//calculate ping time for answer
						int ping_time = (int) ((get_time() - query->start_time) * 1000.0);

						//answer arrived?
						resp.seek(0);
						// ZIGLITE 1.1: avoid malformed packets
						if (resp.size() > (int) (sizeof(NLushort) + sizeof(NLubyte)) ) {

							// ZIGLITE - remove packet_id
							resp.getShort(); 
							
							NLubyte code = resp.getByte();
							if (code == STC_SERVERINFO) {

								//response - put result data!
								buffer_c info;
								info.putBlock( resp.data_cur(), resp.size_left() );
								info.seek(0);

								// put result in queue (local=false, timeout=false)
								queue_result(false, query->addr, info, ping_time, false);

								// delete query
								remove_it = true;
							
							}//answer is serverinfo
						}//answer has at least 1 byte
					}//received answer
				}//not timeout
			}// not local
      
			// query removal (and next query)
			if (remove_it) {
				SAFEDELETE(query);
				it = serverinfo_queries.erase(it);
			}
			// next query only
			else
				it++;
					
		}// while it!=end
	}
//	serverinfo_queries_mutex.unlock();

	// 2) calls back for results; -- check for pending serverinfo callbacks
//	serverinfo_callbacks_mutex.lock();
	if (serverinfo_callbacks.size() > 0) {
		list<serverinfo_result_c*>::iterator it = serverinfo_callbacks.begin(); // for each pending callback...
		while (it != serverinfo_callbacks.end()) {
			serverinfo_result_c *si = (*it);
			
			// call the appropriate callbacks
			if (si->local) {
				if (si->timed_out)
					listener->serverinfo_local_timeout(si->time_ms);
				else
					listener->serverinfo_local_result(si->addr, si->info, si->time_ms);
			}
			else {
				if (si->timed_out)
					listener->serverinfo_timeout(si->addr, si->time_ms);
				else
					listener->serverinfo_result(si->addr, si->info, si->time_ms);
			}

			SAFEDELETE(si); //just delete the element and leave it in the list for now...
			it++; //NEXT!
		}
		serverinfo_callbacks.clear();  //...now clear the entire list
	}
//	serverinfo_callbacks_mutex.unlock();
}

//get information from a server. result comes in the leet_incoming_server_info callback.
//"custom": optional data to be passed to your game server with the request.
void serverinfo_c::get_server_info(address_c &addr, buffer_c &custom, int timeout_ms) {

	// won't work if listener is not set yet
	if (listener == 0)
		return;

	bool failed = true;		//failed by default

	//  OLD COMMENTS:
	//this function spawns a thread and a socket just for sending the serverinfo request
	//and waiting for the serverinfo response, for some amount of time.
	//
	//the thread answers directly to "beh" (the gameserver), so when shutting down client_c,
	//there's no need to sync with ongoing serverinfo requests.
	// --> WELL, unless you call get_server_info and then "beh" deletes itself before the answer!
	//     however, the app is responsible for counting the responses it gets from every call
	//     to get_server_info(), if it really wants to be sure everything is going to shutdown OK.

	//if address is valid go on, if not please fail now
	if (addr.valid() == true) {

		NLsocket sock = nlOpen(0, NL_UNRELIABLE);
		if (sock != NL_INVALID) {
      
			nlSetSocketOpt(sock, NL_BLOCKING_IO, NL_FALSE); // nonblocking socket

			// set target address
			NLaddress sockaddr = addr.get_nl_address();
			nlSetRemoteAddr(sock, &sockaddr);

			// build query packet
			buffer_c req;
			req.putShort( 0 );  //ZIGLITE - add a packet_id==0 to the query to comply to the "game" protocol 
			req.putByte( (NLubyte) CTS_SERVERINFO );
			req.putBlock( custom.data(), custom.size() );
			req.seek(0);

			// send query packet. if nlWrite() returns error, then don't create the query entry (gives up on response)
			double start_time = get_time();
			int res = req.getDataToSocket( sock, req.size() );
			if (res == req.size()) {

				// create query
				serverinfo_query_c* query = new serverinfo_query_c();

				query->local = false;
				query->addr = addr;			// probed host address
				query->sock = sock;
				query->start_time = start_time;
				query->timeout = start_time + ((double)timeout_ms) / 1000.0;    // v1.5.0: configurable timeout

				// put query on the queue
				serverinfo_queries.push_back( query );			

				// OK!
				failed = false;
			}
			else {
				nlClose(sock);  // query nlWrite failed: close the socket
			}
		}
	}

	//if failed, call back with "timeout" - call the serverinfo response as if it had "timed out"
	if (failed) {
		listener->serverinfo_timeout(addr, timeout_ms);
	}
}

//ask information for all servers on the LAN that are listening at the given port. 
//"custom": optional data to be passed to the game servers.
//"timeout": timeout in milliseconds
void serverinfo_c::get_local_server_info(buffer_c &custom, int timeout_ms, int port) {

	// won't work if listener is not set yet
	if (listener == 0)
		return;

	bool failed = true;		//failed by default

	//open socket
	nlHint(NL_REUSE_ADDRESS, NL_TRUE); // BROADCAST : enable -- needed
	NLsocket sock = nlOpen( port , NL_BROADCAST );  // BROADCAST : open at "port"
	nlHint(NL_REUSE_ADDRESS, NL_FALSE); // BROADCAST : disable after use -- may work!
	nlSetSocketOpt(sock, NL_BLOCKING_IO, NL_FALSE);

	if (sock != NL_INVALID) {

		// generate a random ID for filterng
		NLlong query_id = (NLlong)( get_time() * 1000.0 );    // (don't ask...)
		
		//send packet. if nlWrite() returns error, then don't create query entry
		buffer_c req;
		req.putByte( (NLubyte) LEET_BCCOMMAND_SERVERINFO );
		req.putLongs( query_id );
		req.putBlock( custom.data(), custom.size() );
		req.seek(0);

		double start_time = get_time();

		int res = req.getDataToSocket( sock, req.size() );
		if (res == req.size()) {

			// create query
			serverinfo_query_c* query = new serverinfo_query_c();

			query->local = true;
			query->query_id = query_id;   // local query id
			query->sock = sock;
			query->start_time = start_time;
			query->timeout = start_time + ((double)timeout_ms) / 1000.0;    // v1.5.0: configurable timeout

			// put query on the queue
			serverinfo_queries.push_back( query );			
      
			// OK!
			failed = false;
		}
		else {
			nlClose(sock);  // query nlWrite failed: close the socket
		}
	}

	//if failed, call back with "timeout" - call the serverinfo response as if it had "timed out"
	if (failed) {
		listener->serverinfo_local_timeout(timeout_ms);
	}
}

//aux method - insert result/timeout in queue
void serverinfo_c::queue_result(bool local, address_c &addr, buffer_c &info, int time_ms, bool timed_out) {

	// create result
	serverinfo_result_c *si = new serverinfo_result_c();
	si->addr = addr;
	si->info = info;
	si->time_ms = time_ms;
	si->local = local;  // new - cleaner
	si->timed_out = timed_out;  // new - cleaner

	// add result to pending list -- will be read and removed from the queue in serverinfo_c::process_nonblocking()
	serverinfo_callbacks.push_back(si);		
}


