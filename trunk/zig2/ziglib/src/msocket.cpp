/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*

	msocket - see overview in msocket.h

*/

#include "zig/msocket.h"
#include "zig/bzip2.h"
extern "C" {
	#include "zig/network-api.h"
}

//extern vars from <utils.cpp>
extern int zig_stat_zipped_bytes;  // stats
extern int zig_stat_normal_bytes; 
extern int zig_bzip2_compression_level; // bzip2 global compression level

//extern functions from <network.c>
//extern "C" int zig_open_udp_socket(/*unsigned short port*/);
//extern "C" int zig_close_socket(int sock);
//extern "C" int zig_set_blocking_io(int sock, int value);
//extern "C" int zig_send_packet(int sock, const void* data, unsigned int length, const char* target);
//extern "C" int zig_receive_packet(int sock, void* data, unsigned int max_data_length,
//                       char* source, unsigned int max_source_length);


// ctor
msocket_c::msocket_c() {
	beh = NULL;
	con = NULL;
	sock = NL_INVALID;
	stations.clear();

	// grow all the shared buffers to 64K
	udp_read_buf.ensure_capacity( ZIG_UDP_BUFFER_SIZE );
	udp_buf_zipped_b.ensure_capacity( ZIG_UDP_BUFFER_SIZE );
	udp_buf_unzipped_b.ensure_capacity( ZIG_UDP_BUFFER_SIZE );
	sendbuf.ensure_capacity( ZIG_UDP_BUFFER_SIZE );
	sendbuf_zipped_b.ensure_capacity( ZIG_UDP_BUFFER_SIZE );

	m_dDefaultLossEmulation = ((double) zig_global_packet_loss_percent) / 100.0;
	m_dDefaultBaseLatencyEmulation = ((double) zig_global_packet_delay_base)/ 1000.0;
	m_dDefaultDeltaLatencyEmulation = ((double) zig_global_packet_delay_delta) / 1000.0;
}

//dtor calls close()
msocket_c::~msocket_c() {
	close_socket();
}

// try to open the socket in the given range. 
// returns 0 on failure, returns 1..65535 (port number) on success
int msocket_c::open_socket(int minport, int maxport) {

	int port_number = 0;

	close_socket();  // if opened, close first
	
	for (int port = minport; port <= maxport; port++) {
		sock = zig_open_udp_socket(port);
		if (sock != NL_INVALID) {
			port_number = port;
			break; // OK!
		}
	}

	// failure - can't open in any of the ports in the given range!
	if (sock == NL_INVALID)
		return 0;

	// set socket to nonblocking
	zig_set_blocking_io(sock, 0);		

	return port_number; 	// ok!
}

// try to close the socket if it is opened. if it is not opened, this 
// method does nothing.
// this method also deletes ALL station_c objects (rendering the pointers 
// returned by create_station() INVALID)
void msocket_c::close_socket() {

	// call msocket_closed() only if it has something to "close"
	// (avoids calling for open_socket() when it is closed already)
	if ((stations.size() > 0) || (sock != NL_INVALID)) {

		// close the NLsocket
		if (sock != NL_INVALID) {
			zig_close_socket(sock);
			sock = NL_INVALID;
		}

		// delete all station_c objects if any
		map<address_c, station_c*>::iterator it = stations.begin();
		while (it != stations.end()) {
			station_c* st = it->second;
			SAFEDELETE(st);
			it++;
		}
		stations.clear();

		// tell app that socket was closed 
		beh->msocket_closed();
	}
}

// instantiate a new subordinate station_c object and register it with a given 
// source address (IP:PORT). any packets arriving on the UDP socket that are 
// from the given IP:PORT will be routed to this station_c object. 
// returns true if success, false if a station is already registered with the 
// given address.
station_c *msocket_c::create_station(address_c& src_addr) {

	if (find_station(src_addr) != NULL) return NULL;

	station_c* st = new station_c( src_addr );

	st->set_msocket(this);
	st->set_console(con);
  st->set_packet_loss_emulation(m_dDefaultLossEmulation);
  st->set_packet_latency_emulation(m_dDefaultBaseLatencyEmulation, m_dDefaultDeltaLatencyEmulation);

	stations.insert(pair<address_c, station_c*>(src_addr, st));

	return st;
}

// deletes a station, given a reference to it. returns TRUE if station was 
// found on the map of stations for removal, FALSE otherwise.
bool msocket_c::delete_station(address_c& station_addr) {

	map<address_c, station_c*>::iterator it = stations.find(station_addr);
	if (it != stations.end()) {

		//found
    //delete station
    SAFEDELETE(it->second);

    //erase from map
		stations.erase(it);

		return true; 		// success
	}
	else return false;
}

// finds a station in stations map.
// If doesn't exist station associate with remote address in the map, return NULL.
station_c *msocket_c::find_station(address_c& remote_addr) {
	std::map<address_c, station_c*>::iterator it = stations.find(remote_addr);
	if (it == stations.end()) return NULL;
	else return it->second;
}

// sends a packet to remote address IP:PORT by the NLsocket
// this method is only a "interface" to nlWrite, and just send the
// buffer_c data, then it may be used to send RUDP and raw packets
// returns the number of bytes sent by NLsocket, -1 on error
int msocket_c::send_packet(buffer_c packet, address_c remote_addr, double delay) {

	// closed socket
	if (sock == NL_INVALID) return -1;

	// null message sent
	if (packet.size() <= 0) return 0;

	// invalid address
	if (remote_addr.valid() == false) return -1;

	// address_c translation to NLaddress
	NLaddress nl_addr = remote_addr.get_nl_address();
	NLboolean set_addr = nlSetRemoteAddr(sock, &nl_addr);
	if (set_addr == NL_FALSE) return false;

	// latency emulation
	if (delay > 0.0) {

		delayed_packet_c aux;
		aux.packet = packet;
		aux.dest = remote_addr;
		aux.time = get_time() + delay;

		delaylist.push_back( aux );
		return 0;
	}
	else {
		//con->xprintf(0, "msocket_c::send_packet enviando pacote\n");
		//packet.print(con);
		return zig_send_packet(sock, packet.data(), packet.size(), remote_addr.get_address().c_str());
	}
}

/// Sets a emulation of packet loss for all stations created by this msocket
void msocket_c::set_packet_loss_emulation(double loss) {

	m_dDefaultLossEmulation = loss;

	std::map<address_c, station_c*>::iterator it = stations.begin();
	while (it != stations.end()) {

		station_c *station = (*it).second;
		station->set_packet_loss_emulation(loss);
		it++;
	}
}

/// Sets a emulation of aleatory delay before send a packet for all stations created by this msocket
void msocket_c::set_packet_delay_emulation(double base_delay, double delta_delay) {
  
	m_dDefaultBaseLatencyEmulation = base_delay;
	m_dDefaultDeltaLatencyEmulation = delta_delay;

	std::map<address_c, station_c*>::iterator it = stations.begin();
	while (it != stations.end()) {

		station_c *station = (*it).second;
		station->set_packet_latency_emulation(base_delay, delta_delay);
		it++;
	}
}


bool check_packet_syntax(buffer_c &packet) {

	packet.seek( 0 );

	if ( packet.size_left() < static_cast<int>( sizeof(NLushort) ) ) return false;
	NLushort id = packet.getShort();

	switch( id ) {

		// Special packet, no more mandatory data
		case STATION_SPECIAL_PACKET_ID:
			break;

		// Connection request packet, need a connection ID (1 byte)
		case STATION_CONNECT_REQUEST_PACKET_ID:
			if ( packet.size_left() < static_cast<int>( sizeof(NLubyte) ) ) return false;
			break;

		// Connection accept, no more mandatory data
		case STATION_CONNECT_ACCEPT_PACKET_ID:
			if ( packet.size_left() < static_cast<int>( sizeof(NLubyte) ) ) return false;
			break;

		// Connection reject, need a connection ID (1 byte)
		case STATION_CONNECT_REJECT_PACKET_ID:
			if ( packet.size_left() < static_cast<int>( sizeof(NLubyte) ) ) return false;
			break;

		default:
			break;
	}

	packet.seek( 0 );
	return true;
}
void q() {}
// non-blocking operation: read and process any and all packets that are ready 
// to read from the UDP socket. packets will be forwarded to the station_c 
// components and station callbacks for incoming packets will be called.
void msocket_c::process_nonblocking() {

	// closed socket
	if (sock == NL_INVALID) return;

	// send delayed packets
	std::list<delayed_packet_c>::iterator iter = delaylist.begin();
	while (iter != delaylist.end()) {

		delayed_packet_c aux = (*iter);

		// check if must send
		if ( aux.time < get_time() ) {

			// send packet now!
			send_packet(aux.packet, aux.dest, 0.0);

			// remove from list
			iter = delaylist.erase(iter);
		}
		// else advance to next delayed packet
		else iter++;
	}

	// run process_nonblocking on ALL stations
	std::map<address_c, station_c*>::iterator it = stations.begin();
	while (it != stations.end()) {

		station_c *station = (*it).second;

		// increment it BEFORE call station_c::process_nonblocking
		// (if a timeout occurs, upper layer will destroy station and remove it from msocket map)
		it++;

		station->process_nonblocking();
	}

	// read packets
	// loop until "break;" from error or no more packets 
	while (true) {

		// important!
		udp_read_buf.clear();

		char source[1000];

		// read one packet. udp_buf has all the space needed (64K) allocated from the get-go
		int read_result = zig_receive_packet(sock, udp_read_buf.data(), ZIG_UDP_BUFFER_SIZE, source, 100);

		// just read a zero-sized packet OR no more packets. since we can't tell the 
		// difference, just break and let the packet flushing continue on the next cycle
		//
		// FIXME: come up with some way to tell "zero-sized packets" from "no packets"
		//
		if (read_result == 0)
		{
			break;
		}
		else if (read_result < 0)
		{
			// notify socket reading error to app, which will decide what to do.
			// gives the NLsocket to be checked.
			beh->msocket_read_error( sock );

			break;
		}
		else {  // if (read_result > 0) 

			// fix udp_read_buf size!
			udp_read_buf.manual_put_block( read_result );		// ulen = read_result
			udp_read_buf.seek( 0 );                         // pos = 0

			//con->xprintf(0, "msocket::proc_nb recebi pacote\n");
			//udp_read_buf.print(con);

			int target_vport;
			try {
				// ZIGLITE 1.1: check if needed and do the decompression for the data on udp_read_buf
				// result is placed on udp_read_buf.
				// ALSO: read the VPORT from the packet's id
				target_vport = preprocess_packet();
			}
			catch (int ) {
				// ignore malformed packets
				return;
			}

			// no error reading, give packet "as-is" to the registered station OR to 
			// the default msocket packet handler if no station is found

			address_c last_addr(source);

			// ZIGLITE 1.1: address includes the target VPORT value!
			//
			//  FIXME/REVIEW: target_vport can be -1 for packets with less than 2 bytes. -1 
			//    means no station will match it and that it will go straight to unreg()
			//
			last_addr.set_virtual_port( target_vport );

			// try to find it in the map
			map<address_c, station_c*>::iterator it = stations.find( last_addr );

			// se não existe a station associada ao endereço do cara que enviou 
			// o pacote que chegou ...
			if (it == stations.end()) {

				//con->xprintf(1, "packet for no station.. #1\n");

				// ZIGLITE 1.1: avoid malformed packets: too few data
				// since it is from an unknown/unconnected station, just discard it
				// FIXME/REVIEW: IP suspend/ban possible here
				if ( check_packet_syntax(udp_read_buf) == false ) continue;
				
				// ZIGLTE 1.1: check for msocket/station connection request packets
				NLushort packet_id = udp_read_buf.getShort();

				NLubyte remote_connection_id;
				buffer_c connect_data;
				buffer_c answer;
				buffer_c packet;
				station_c* st;

				if ( packet_id == STATION_CONNECT_REQUEST_PACKET_ID ) {

					remote_connection_id = udp_read_buf.getByte();

					// the rest of the packet is the custom connect data
					connect_data.putBlock( udp_read_buf.data_cur(), udp_read_buf.size_left() );

					// ask upper layer if connection should be accepted (also fills "answer")
					if ( beh->msocket_accept_station( last_addr, static_cast<const buffer_c&>(connect_data), answer ) ) {

						//con->xprintf(1, "packet for no station.. - app accepted! #3\n");

						// accept!
						st = create_station( last_addr );

						// this SENDS the accept packet, sending the WHOLE of "answer" as the "hello" parameter
						st->accept_connection(remote_connection_id, answer, static_cast<const buffer_c&>(connect_data));

						// call back app to tell of the new remote station that connected on us
						connect_data.seek(0);
						beh->msocket_station_connected(*st, connect_data); 
					}
					else {

						//con->xprintf(1, "packet for no station.. - app REJECTED! #4\n");

						// rejected! send a reject packet
						packet.putShort( STATION_CONNECT_REJECT_PACKET_ID );
						packet.putByte( remote_connection_id );
						packet.putBlock( answer.data(), answer.size() );

						send_packet( packet, last_addr );
					}
				}
				else if ( packet_id == STATION_SPECIAL_PACKET_ID ) {

					// no registered station - give packet to the default handler
					beh->msocket_incoming_unreg( last_addr, static_cast<const buffer_c&>(udp_read_buf) );
				}
			}
			else {

				// found registered station. 
				station_c* st = it->second;

				// ZIGLITE 1.1: avoid malformed packets: too few data
				// FIXME: must notify upper layer through a new msocket::station_read_error() callback!
				if ( check_packet_syntax(udp_read_buf) == false ) continue;

				// check for station internal packets (connection accepted, rejected, request)
				NLushort packet_id = udp_read_buf.getShort();
				if ( packet_id == STATION_CONNECT_REQUEST_PACKET_ID ) {

					//con->xprintf(1, "connection request for a connecting station\n");
					// **************************************
					//  FIXED:
					//    the packet is for a registered station BUT it may be a connection 
					//    request packet (internal station protocol packet)
					// this happens when two stations try to connect to each other simultaneously
					// must deal with this somehow
					// **************************************
					//
					// ANYWAY: _MUST_ prevent any packets with "special" station IDs to reach the app!
					// **************************************

					if ( st->is_connecting() == false ) continue;

					NLubyte remote_connection_id = udp_read_buf.getByte();

					// set up st-> to send the confirmation packets (REPLY SAME DATA!!)
					st->accept_connection(remote_connection_id, st->connect_data, static_cast<const buffer_c&>(udp_read_buf));

					// NEW: _NOW_ append answer to temp

					// call back app to tell of the new remote station that connected on us.
					// "true": SPECIAL connected callback ("forced"), when there is a simultaneous connection 
					//   attempts. in this case, you won't call msocket_accept_station() on the app to see 
					//   if it returns true or false to accept, and, consequently, the app won't have a chance 
					//   to fill the 3rd parameter of msocket_accept_station(), which is the buffer that contains 
					//   something like the generated ClientID or something, and then we don't have that buffer 
					//   here to pass as parameter, so we send a dummy, and set the 'true' below to signal that.
					// the rest of the packet is the custom connect data
				}
				else if ( packet_id == STATION_CONNECT_ACCEPT_PACKET_ID ) {

					// if not st->is_connecting, this just ignores the packet
					if ( st->is_connecting() ) {
						st->incoming_connection_accept(udp_read_buf);
					}
				}
				else if ( packet_id == STATION_CONNECT_REJECT_PACKET_ID) {

					//con->xprintf(1, "connect reject packet =(\n");
					// if not st->is_connecting, this just ignores the packet

					if ( st->is_connecting() ) {
						st->incoming_connection_reject( udp_read_buf );
					}
				}
				else if (packet_id == STATION_RTT_ECHO) {
					if ( st->is_connected() ) {
						buffer_c reply;
						reply.putShort(STATION_RTT_REPLY);
						reply.putDouble(udp_read_buf.getDouble());
						send_packet(reply, last_addr, 0.0);
					}
				}
				else if (packet_id == STATION_RTT_REPLY) {
					if ( st->is_connected() ) {
						st->update_rtt_info(udp_read_buf);
					}
				}
				else if ( packet_id == STATION_ABORT_PACKET_ID ) {

						if ( st->is_connected() ) {
							con->xprintf(4, "msocket_c::proc_nb chegou pacote abort request\n");
							st->incoming_connection_abort( udp_read_buf );
						}
				}
				else {

					// verify duplicate packet
					if (st->update_loss_info(packet_id)) {

						st->update_delay_info(packet_id);

						if ( st->is_connected() ) {
							// NOT an internal packet -- feed the packet to the station as-is

							// station will do whatever is needed with it (unzip, check internal msg, process, notify app...)
							st->incoming_msocket_packet();  //udp_read_buf  : IMPLICIT
						}
						//else con->xprintf(1, "Pacote com %d bytes dropado (station ainda nao esta connected)\n", read_result);
					}
				}
			} // found registred station
		} // read_result > 0
	} // while (true)
}

//helper method: uncompress packet and read the vport. input & output at udp_read_buf
// return value: target vport for the incoming packet
int msocket_c::preprocess_packet() {

	// read vport value. default is -1 which means that even the vport bit could not be read
	// (packet with less than 2 bytes)
	int read_vport = -1;

	buffer_c *packet = &udp_read_buf;

	// ...
	int result = packet->size();

	// fool the "old" unzip code by setting udp_buf_zipped to the udp packet
	char *udp_buf_zipped = packet->data();

	if (result > 0) {

		//NEW: "udp_buf" is an alias either to "udp_buf_zipped" or "udp_buf_unzipped", depending if
		//  we had to decompress the incoming packet or not
		char *udp_buf = udp_buf_zipped;		//defaults to the first stage buffer

		//NEW: first, we must check if the packet is compressed. if yes, we must decompress it first.
		if (result >= 2) {    //2 == at least packet id (2). it BETTER have at least 2 bytes!
			
			int cursor = 0;
			NLushort packet_id;
			readShort(udp_buf_zipped, cursor, packet_id);   //PACKET ID

#ifndef ZIG_NO_BZIP2 // don't include BZIP2

			if ((packet_id & STATION_COMPRESSED_PACKET_BIT) != 0) {

				//compressed. must uncompress first.

				// v1.3.0: avoid allocation of a fixed-size buffer for doing the decompression. 
				// instead, work with a buffer that grow "as needed" to read from the socket.
				// - LOOP using the current allocated buffer; if decompression fails by lack of
				//   space, it is increased and the iteration continues
				int decompress_result;
				unsigned int dest_size;
				char *udp_buf_unzipped = 0;
				int capac_step = 1024;
				do {

					// - fool the previous decompression code by setting udp_buf_unzipped
					udp_buf_unzipped = udp_buf_unzipped_b.data();
					int ucapac = udp_buf_unzipped_b.get_capacity();

					//- write the fixed packet ID (without the compressed bit)
					int u_cursor = 0;
					writeShort(udp_buf_unzipped, u_cursor, packet_id - STATION_COMPRESSED_PACKET_BIT);

					//- uncompress the data there
					//unsigned int dest_size = sizeof(udp_buf_unzipped) - 2;  //-2 : size of the ID
					//NEW v1.3.0: using buffer_c now, so use buffer_c::get_capacity()
					dest_size = ucapac - 2;  //-2 : size of the ID
					unsigned int compressed_size = result - 2;		//-2 : amount read with nl Read minus size of the ID

					//con->xprintf(1,"COMPRESSED SIZE =  %i\n", compressed_size);

					decompress_result = BZ2_bzBuffToBuffDecompress(
						(char*) &(udp_buf_unzipped[2]),	//dest buffer initial position ptr
						&dest_size,											//dest buffer available size and return value = decompressed size written to dest
						(char*) &(udp_buf_zipped[2]),	  //src buffer initial position ptr
						&compressed_size,								//src buffer amount of compressed data to uncompress
						0,0);

					// v1.3.0: decompression failing because of lack of space? then increase it!
					if (decompress_result == BZ_OUTBUFF_FULL) {
            udp_buf_unzipped_b.ensure_capacity( ucapac + capac_step ); // resize buffer for the next try
						if (capac_step < 16384)
							capac_step *= 2; // if looping again, next time increase by double amount (max 16K step)
					}
					else
						break; // if OK or any other error, just quit

				} while (true);

				// check error. if error, then just DROP silently the packet and go back to reading the next 
				// packet... pretend that the EVIL NETWORK dropped your packet :-)
				if (decompress_result != BZ_OK) {
					return false;
				}

				// was compressed. add up statistics
				zig_stat_normal_bytes += (dest_size + 2);	//result AFTER unzipping are the NORMAL bytes! (result= is fixed below see)
				zig_stat_zipped_bytes += result;					//before unzipping

				//all done: udp_buf_unzipped contains the actual data for the rest of the code
				//update udp_buf so the rest of the code stays happy
				udp_buf = udp_buf_unzipped;		//point to the second stage buffer
				result = (dest_size + 2);		// FIX nl Read()'s "RESULT"! (we use it below as a "udp packet read" size)
			}
			else {

				// was not compressed. add up statistics
				zig_stat_normal_bytes += result;
				zig_stat_zipped_bytes += result;
			}

			// IF decompression took place, must move unzipped data back to udp_read_buf
			if (udp_buf == udp_buf_unzipped_b.data() ) {  //FIXED : copy needed only if had to decompress packet
				packet->clear();
				packet->putBlock(udp_buf, result);  // result == packet.size() (HACK)
				packet->seek(0);
			}
#endif // #ifndef ZIG_NO_BZIP2 // don't include BZIP2

			//NEW (ZIGLITE 1.1): now that the packet is uncompressed, we can read the vport bit AND 
			//  eventually the extra vport data that follows the ID
			if ((packet_id & STATION_VPORT_BIT) != 0) {
				
				packet->seek(2);  // sizeof( packet ID )
				read_vport = packet->get32K(); // read the vport value with get32K()	(1 or 2 bytes, 0..32767)
				
				packet->seek(0);	// seek back to 0 to leave it ready for parsing
			}
			else
				read_vport = 0; // vport value for when the vport bit is unset

		}//>= 2

	}//>0

	// returns the read vport value. defaults to 0
	return read_vport;
}
