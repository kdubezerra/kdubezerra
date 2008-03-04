/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*

	msocket - a component that wraps I/O around a single opened UDP socket, that can multiplex 
	 (route) packets arriving on that socket to multiple instances of a high-level component 
	 (station.cpp) depending on the sender's (IP:PORT) UDP socket.

	rationale
	  ZIG was initially designed in a way such that one UDP socket would only talk to 
	  another, single UDP socket. so, a game server would open one UDP socket to service 
	  each client (plus one general "request" UDP socket that was used to negotiate new 
	  connections and answer general "outside" requests such as serverinfo queries).
	  
	  well, this would work O.K. if we lived in a NAT and firewall-free world...

	  this is why upto ZIG 1.5.0, the "station_c" class would model the "reliable UDP" 
	  protocol handler coupled with an UDP socket reader/writer.

	  starting from ZIGLITE v1.0 and ZIG v1.6, this is decoupled. "msocket_c" holds 
	  the UDP socket, and according to source IP:PORT of incoming packets, it routes 
	  them to zero or more "registed" station_c instances which have their own 
	  stateful handling of the RUDP protocol.
*/

#ifndef _ZIG_HEADER_MSOCKET_H_
#define _ZIG_HEADER_MSOCKET_H_

#include <nl.h>
#include "zig.h"
#include "address.h"
#include "station.h"

// needed for msocket's registred stations map
#include <map>

// needed for msocket's list of delayed packets
#include <list>

// auxiliar class used in delayed packets STL list
class delayed_packet_c {
	// internal class, so all class members are public!
	public:
		buffer_c  packet; // the packet
		address_c dest;   // the destination
		double    time;   // the minimum value of get_time() to send this packet
};

// msocket callback interface
class msocket_callback_i {
public:

	// incoming packet from unregistered IP:PORT source. a typical implementation of this method 
	// would answer directly to "serverinfo"-like requests, and would also handle "new connection" 
	// requests by registering a new station_c socket for that IP:PORT.
	// parameters: remote address and UDP packet payload
	virtual void msocket_incoming_unreg(address_c &addr, const buffer_c &pkt) = 0;

	// got NL_INVALID while reading from the UDP socket. an use of this callback is to detect
	// NL_SOCK_DISCONNECT errors and, if a client, disconnect from the server or, if a server, 
	// try to find out which client socket died and remove it from the server.
	virtual void msocket_read_error(NLsocket &sock) = 0;

	// a remote station has called connect() towards us. return true to accept, false to reject.
	virtual bool msocket_accept_station(address_c &addr, const buffer_c& connection_data, buffer_c& answer) = 0;

	// called after msocket_accept_connection() returns true - a new station is created for 
	// the incoming connection request.  
	virtual void msocket_station_connected(station_c &st, const buffer_c& oConnectionData) = 0;

	// msocket has closed: all station_c's have been deleted and current station_c pointers are 
	// now invalid.
	virtual void msocket_closed() = 0;

	virtual ~msocket_callback_i() {}
};

//the multiplexing UDP socket
class msocket_c {
public:

	// ctor
	msocket_c();

	// dtor - close socket and delete all stations
	// all (station_c *) pointers returned by create_station() become INVALID!
	virtual ~msocket_c();

	// HACKISH - returns the protected NLsocket
	// This is necessary to polling multiple stations support
	NLsocket get_sock() { return sock; }

	// set callback interface. this MUST be called, preferrably before opening the socket, 
	// or else the station cannot start receiving packets from "unknown" (new) sources. 
	void set_callback_interface(msocket_callback_i *beh) { this->beh = beh; }
	
	// set debug console
	void set_console(console_c *conio) { con = conio; }

	// try to open the socket in the given range. 
	// returns 0 on failure, returns 1..65535 (port number) on success
	int open_socket(int minport, int maxport);

	// try to close the socket if it is opened. if it is not opened, this 
	// method does nothing.
	// this method also deletes ALL station_c objects (rendering the pointers 
	// returned by create_station() INVALID)
	void close_socket();

	bool is_open() const { return sock != NL_INVALID; }

	// instantiate a new subordinate station_c object and register it with a given 
	// source address (IP:PORT). any packets arriving on the UDP socket that are 
	// from the given IP:PORT will be routed to this station_c object. 
	// returns a pointer to the allocated station_c instance, or 0 if an error 
	// ocurred.
	//
	// NOTE: the src_addr parameter's "vport" value matters:
	// it specified the virtual port this station is attached to. vports are 
	// to be used when more than one station is created with the same remote 
	// IP:PORT address (so the vport ID can be used to route traffic).
	// using any other vport other than 0 will increase packet size by 1 byte.
	station_c *create_station(address_c& src_addr);

	// deletes a station, given a reference to it. returns TRUE if station was 
	// found on the map of stations for removal, FALSE otherwise.
	bool delete_station(address_c& station_addr);

	// finds a station in stations map.
	// If doesn't exist station associate with remote address in the map, return NULL.
	station_c* find_station(address_c& remote_addr);

	/// Sends a packet to remote address IP:PORT by the NLsocket
	/// this method is only a "interface" to nlWrite, and just send the
	/// buffer_c data, then it may be used to send RUDP and raw packets
	/// returns the number of bytes sent by NLsocket, -1 on error
	/// @param delay: seconds before send packet
	int send_packet(buffer_c packet, address_c remote_addr, double delay = 0.0);

	/// Sets a emulation of packet loss for all stations created by this msocket
	/// @param loss: seconds before send packet
	void set_packet_loss_emulation(double loss);

	/// Sets a emulation of aleatory delay before send a packet for all stations created by this msocket
	/// @param base_delay: minimum time before send packet
	/// @param delta_delay: difference entreminimum time before send packet
	void set_packet_delay_emulation(double base_delay, double delta_delay);

	// non-blocking operation: read and process any and all packets that are ready 
	// to read from the UDP socket. packets will be forwarded to the station_c 
	// components and station callbacks for incoming packets will be called.
	void process_nonblocking();

public:

	// the socket
	NLsocket	sock;

	// temp buffers that are read by station_c instances when they need to work 
	// since we work one station at a time, we only need one buffer
	buffer_c	udp_read_buf;
	buffer_c	udp_buf_zipped_b;
	buffer_c	udp_buf_unzipped_b;
	buffer_c	sendbuf;
	buffer_c  sendbuf_zipped_b;

protected:

	//helper method: uncompress packet and read the vport. input & output at udp_read_buf
	// return value: target vport for the incoming packet
	int preprocess_packet();

	//debug console
	console_c	*con;

	//callback interface
	msocket_callback_i	*beh;

	/// registration map of source address (IP:PORT) to station_c's
	std::map<address_c, station_c*>	stations;

	/// list of delayed packets (useful for disconnection response packets and
	/// latency emulation)
	std::list<delayed_packet_c> delaylist;

	double m_dDefaultLossEmulation;
	double m_dDefaultBaseLatencyEmulation;
	double m_dDefaultDeltaLatencyEmulation;
};

#endif
