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

	station.cpp
*/

#include "zig/bzip2.h"		//for packet compression/decompression
#include <nl.h>
#include <stdio.h>
#include "zig/station.h"
#include "zig/msocket.h"
#include <algorithm>
#include <math.h>

using std::map;

//turn this on to show the uglier debug stuff
//int debug = 0;

//debug print macro
//#define LOG(P0) { if (con != 0) con->xprintf(1,P0); }
//#define LOG1(P0,P1) { if (con != 0) con->xprintf(1,P0,P1); }
//#define LOG2(P0,P1,P2) { if (con != 0) con->xprintf(1,P0,P1,P2); }
//#define LOG3(P0,P1,P2,P3) { if (con != 0) con->xprintf(1,P0,P1,P2,P3); }
//#define LOG4(P0,P1,P2,P3,P4) { if (con != 0) con->xprintf(1,P0,P1,P2,P3,P4); }
//#define LOG5(P0,P1,P2,P3,P4,P5) { if (con != 0) con->xprintf(1,P0,P1,P2,P3,P4,P5); }

//extern vars from <utils.cpp>
extern int zig_stat_zipped_bytes;  // stats
extern int zig_stat_normal_bytes; 
extern int zig_bzip2_compression_level; // bzip2 global compression level

#include <iostream>
using namespace std;

//GLOBAL (process-wide) defaults for packet loss and packet latency emulation
// default to 0 - no emulated loss or latency
int zig_global_packet_loss_percent = 0;
int zig_global_packet_delay_base = 0;
int zig_global_packet_delay_delta = 0;

// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// NEW (ziglite 1.2): policy_c class

const policy_c DefaultPolicy;
const address_c InvalidAddress;
const buffer_c EmptyBuffer;

string buffer_print(buffer_c& buf) {

	string result;

	char *data = buf.data();
	int   size = static_cast<int>( buf.size() );

	char aux[10];

	for (int i = 0; i < size;) {

		string s;
		for (int j = 0; (j < 16) && (i < size); i++, j++) {
			sprintf(aux, "%02X ", static_cast<unsigned char>( data[i] ) );
			s.append(aux);
		}

		s.append("\n");
		result.append(s);
	}

	return result;
}

stream_c::stream_c(int id, int _type, policy_c _policy) { 

	m_iStreamId = id;

	policy = NULL;
	set_policy(_policy);

	next_send = get_time();
	type = _type;

	if (type != STREAM_INDIVIDUAL_ACK) policy->set_resend_interval(0.0);
	
	msg_current = ZIG_FIRST_MSG_ID;
	idgen_reliable_send = ZIG_FIRST_MSG_ID;
	idgen_reliable_rec = ZIG_FIRST_MSG_ID;

	// NEW (ZIGLITE 1.2): ID of last reliable msg sent.
	last_reliable_msg = ZIG_INVALID_MSG_ID;
	cumulative_ack = ZIG_INVALID_MSG_ID;
	
	imsg_c dummy;
	dummy.dummy = false;

	// init message queue
	message.reserve( MAX_INCOMING_MESSAGES );
	for ( int i = 0; i < MAX_INCOMING_MESSAGES; i++ ) {
		message.push_back( dummy );
	}
	queue_size = 0;

	reset_state();
}

void stream_c::reset_state() {

	idgen_reliable_send = ZIG_FIRST_MSG_ID;
	idgen_reliable_rec = ZIG_FIRST_MSG_ID;

	//reset base packet id stuff too
	ackbuf.clear();

	for (int i = 0; i < static_cast<int>( message.size() ); i++) {
		message[i].body.clear();
		message[i].dummy = false;
	}
	queue_size = 0;

	msg_current = ZIG_FIRST_MSG_ID;
	last_reliable_msg = ZIG_INVALID_MSG_ID;
	send_ack = true;

	last_incoming_packet_time = get_time();
	last_outgoing_packet_time = last_incoming_packet_time;

	reliable.clear();
}

//ctor
station_c::station_c(const address_c& _addr) { 

	msocket = NULL; // MUST call set_msocket() after construction!

	set_remote_address( _addr );
	
	beh = NULL;
	con = NULL; 
	dstr[0] = '\0';
	
	set_compression(-1); // By defult, do not compress packets.

	connect_attempts = STATION_DEFAULT_CONNECT_ATTEMPTS;
	connect_interval = STATION_DEFAULT_CONNECT_INTERVAL;

/*
	disconnect_attempts = STATION_DEFAULT_DISCONNECT_ATTEMPTS; 
	disconnect_interval = STATION_DEFAULT_DISCONNECT_INTERVAL;
*/
	abort_attempts = STATION_DEFAULT_ABORT_ATTEMPTS;
	abort_interval = STATION_DEFAULT_ABORT_INTERVAL;

	reset_state();

	//set default max packet value as defined in ZIGDEFS.H
	set_max_outgoing_packet_size();

	//set defaults for loss & latency from GLOBAL variables (those default to 0 - no emulated loss or latency)
	double loss = ( static_cast<double>( zig_global_packet_loss_percent ) ) / 100.0;
	double base = ( static_cast<double>( zig_global_packet_delay_base ) ) / 1000.0;
	double delta = ( static_cast<double>( zig_global_packet_delay_delta ) ) / 1000.0;

	set_packet_loss_emulation( loss );
	set_packet_latency_emulation( base, delta );
}

/// private destructor (only friend class msocket can destroy stations,
/// application must only call msocket_c::close_socket())
station_c::~station_c() {

	// ZIG 2.0.0: bug fixed: memory leak on streams. Thanks Milan Mimica for spotting this with Valgrind 
	// and Diego Pereira to explaining what was needed for the fix.
	//
	// streams are the only member vars of station_c that allocate memory and that are not deleted 
	// automatically when this station_c object is deleted.
	// we need to release any streams memory that wasn't explicitly released with delete_stream() calls 
	// beforehand by delete_stream() calls (NOTE: calling delete_stream() should never be required from 
	// code that uses station/msocket!).
	// since the object is going away, this is the best place to delete any stream_c objects that still 
	// weren't freed.
	//
	for (std::map<int, stream_c*>::iterator iter = streams.begin();
	     iter != streams.end(); iter++) {
		delete iter->second;
	}
	streams.clear();	// came along with the CTRL+C/CTRL+V from reset_state()...
}	

// return number of bytes writes in socket
int station_c::emulate_latency_and_loss(buffer_c packet, address_c remote_addr) {

	// "granularidade" do sorteio (probabilidades consideradas até 4a casa decimal)
	int emu_loss = (int) (packet_loss * 10000.0);

	// convert latency from seconds to miliseconds
	// (rand() operates only with integers)
	int emu_latency_base = (int) (delay_base * 1000.0);   // in miliseconds
	int emu_latency_delta = (int) (delay_delta * 1000.0); // in miliseconds

	// emulate packet loss
	int dice = rand() % 10000;
	if (dice < emu_loss) return 0;

	// random latency
	int espera = emu_latency_base;
	if (emu_latency_delta > 0) espera += rand() % emu_latency_delta;

	double delay = ( static_cast<double>(espera) ) / 1000.0;
	return msocket->send_packet(packet, remote_addr, delay);
}

/// Init connection handshake with remote station
void station_c::connect(double timeout) {

	buffer_c cdata = connect_data;
	// Reset station state
	reset_state();
	connect_data = cdata;

	// setup for connection 
	station_state = STATION_CONNECTING;
	connect_timeout = get_time() + timeout;		// Fabio: leave "timeout" here; if needed pass parameter

	connect_packet_left_count = connect_attempts;
	connect_packet_next_time = get_time();

	// generates an "unique" identifier for connection, to avoid old packets
	// disturb connection handshake
	local_connection_id = (rand() % 255) + 1;
}

/// ZIG 2.0: removed support to disconnection 2-way handshake
void station_c::disconnect(const buffer_c& abort_reason) {

	if (is_connected())
	{
		abort_packet.clear();
		abort_packet.putShort( STATION_ABORT_PACKET_ID );
		abort_packet.putByte( remote_connection_id );
		abort_packet.putBlock( abort_reason.data(), abort_reason.size() );

		double next_time = 0.0;
		for (int i = 0; i < abort_attempts; i++) {
			msocket->send_packet(abort_packet, remote_addr, next_time);
			next_time += abort_interval;
		}
	}

	station_state = STATION_CLOSED;

	// get rid of const crap, give arg for app to play with
	buffer_c reason = abort_reason;	

	// CALL SELF -- we also want to know when the connection is aborted!!  / Fabio
	beh->station_disconnected( false, reason );
}

//set owner msocket. MUST do this before using this instance
void station_c::set_msocket(msocket_c *msocket) { 

	this->msocket = msocket;

	// point to the shared buffers
	this->udp_read_buf     = &(msocket->udp_read_buf);
	this->sendbuf          = &(msocket->sendbuf);
	this->sendbuf_zipped_b = &(msocket->sendbuf_zipped_b);
}

/// Reset station state
void station_c::reset_state() {

	//taken off
	idgen_packet_send = ZIG_FIRST_PACKET_ID;

	for (std::map<int, stream_c*>::iterator iter = streams.begin();
	     iter != streams.end(); iter++)
	{
		delete iter->second;
	}
	streams.clear();

	// clear 
	unrelbuf.clear();

	// station will only accept incoming connection request packets
	station_state = STATION_CLOSED;

	// time when connection is finished
	connect_timeout = 0.0;
	// data sent in every connection request packet
	connect_data.clear();
	// connection packets left
	connect_packet_left_count = 0;
	// time for next connection packet
	connect_packet_next_time = 0.0;

	// data sent in every accept packet
	accept_packet.clear();

	abort_packet.clear();
	abort_attempts = STATION_DEFAULT_ABORT_ATTEMPTS;
	abort_interval = STATION_DEFAULT_ABORT_INTERVAL;

	local_connection_id = 0;
	remote_connection_id = 0;

	default_stream_id = 0;

	//ZIG 2.0 added after being removed by zig-exp(?)
	//  counters for the full 32-bit int packet id 
	base_packet_id = 0;
	last_packet_id = 0;
	base_o_packet_id = 0;
	last_o_packet_id = 0;

	max_incoming_packet_id = ZIG_INVALID_PACKET_ID;
	for (int i = 0; i < ZIG_PACKET_ID_RANGE; i++) {
		received_packet_ids[i] = false;
	}
	total_packets = 0;
	total_received_packets = 0;

	delay_base_id = ZIG_FIRST_PACKET_ID;
	delay_last_id = ZIG_FIRST_PACKET_ID;
	for (int index = 0; index < ZIG_PACKET_ID_RANGE; index++) {
		delay_packets[index] = ZIG_INVALID_PACKET_ID;
	}

	first_rtt_estimative = 0;
	last_rtt_estimative = -1;
	rtt_protocol_interval = STATION_DEFAULT_RTT_INTERVAL;
	last_rtt_sent = 0.0;
	for (int i = 0; i < STATION_MAX_RTT_TABLE; i++) {
		rtt_table[i] = -1.0;
	}
}

// this method is called periodically for the station by the underlying msocket object, which 
// in turn is called periodically by the upper layers (client_c, server_c, which are ultimately called 
// by the app)
// the current uses of station_c::process_nonblocking() are:
//  - send connections/disconnections requests/responses
//  - check for station connection/disconnections timeouts

void station_c::process_nonblocking() {

	buffer_c packet;

	switch (station_state) {

		case STATION_CONNECTING:

			// check for timeout while waiting for connection answer packets
			if (connect_timeout < get_time()) {

				// connection has failed due to timeout
				station_state = STATION_CLOSED;
				connect_packet_left_count = 0;
				connect_packet_next_time = STATION_INFINITY;
				connect_timeout = STATION_INFINITY;

				// call back upper layer telling of timeout disconnection
				beh->station_connection_timed_out();

				// CAUTION: upper layer callback can delete 'this', so we can't
				// access any instance member until end of method
				return;
			}

			// sending connection request packets
			if (connect_packet_left_count > 0) {
				if (connect_packet_next_time < get_time()) {

					connect_packet_next_time += connect_interval;
					connect_packet_left_count--;

					// send it
					packet.putShort( STATION_CONNECT_REQUEST_PACKET_ID );
					packet.putByte( local_connection_id );
					
					packet.putBlock( connect_data.data(), static_cast<int>( connect_data.size() ) );

					// Added packet loss and latency to connection handshake packets
					emulate_latency_and_loss(packet, remote_addr);

				} // connect_packet_next_time < get_time()
			} // connect_packet_left_count > 0

			break;

		// case station is connected ( after receive connection request and
		// before call disconnect()... )
		case STATION_CONNECTED:

			// sending connection accept packets
			if (connect_packet_left_count > 0) {
				
				if (connect_packet_next_time < get_time()) {
				con->printf("enviando connect reply\n");
					connect_packet_next_time += connect_interval;
					connect_packet_left_count--;

					// Added loss & delay packet emulation in connection handshake packets
					emulate_latency_and_loss(accept_packet, remote_addr);

				} // accept_packet_next_time < get_time()
			} //accept_packet_left_count

			double now = get_time();
			if ( (rtt_protocol_interval > 0.0) && (now - last_rtt_sent > rtt_protocol_interval) ) {

				buffer_c packet;
				packet << (NLushort) STATION_RTT_ECHO;
				packet << now;
				msocket->send_packet(packet, remote_addr, 0.0);
				last_rtt_sent = now;
			}
			break;
	}
}

// called by msocket when a connection is accepted
void station_c::incoming_connection_accept(buffer_c& accept_pkt) {
	if (accept_pkt.getByte() == local_connection_id) {

		remote_connection_id = accept_pkt.getByte();
		connect_packet_left_count = 0;
		station_state = STATION_CONNECTED;

		buffer_c accept_data;
		accept_data.putBlock( accept_pkt.data_cur(), accept_pkt.size_left() );
		accept_data.seek(0);

		beh->station_connected(accept_data, false);
	}
}

// called by msocket when a connection is rejected
void station_c::incoming_connection_reject(buffer_c& reject_pkt) {

	int b = (NLubyte) reject_pkt.getByte();

	if (b == local_connection_id) {
		connect_packet_left_count = 0;
		station_state = STATION_CLOSED;

		buffer_c reject_reason;
		reject_reason.putBlock( reject_pkt.data_cur(), reject_pkt.size_left() );
		beh->station_connection_refused( reject_reason );
	}
}

// called by msocket when a abort connection arrives
void station_c::incoming_connection_abort(buffer_c& abort_pkt) {

	int b = (NLubyte) abort_pkt.getByte();

	if (b == local_connection_id) {
		station_state = STATION_CLOSED;

		// DEBUG
		con->xprintf(1,"   >>> connection abort received at: %s\n", this->dstr);

		buffer_c abort_reason;
		abort_reason.putBlock( abort_pkt.data_cur(),
		                       abort_pkt.size_left() );

		// ZIG 2.0 bug fixed: reset position of the buffer before passing it to the callback
		abort_reason.seek(0);

		// inform app that disconnection has incoming
		beh->station_disconnected( true, abort_reason );
	}
}


// process message that arrived
void stream_c::process_incoming_message(NLushort msgid, int msgsize, buffer_c& in) {

	// Special case: STREAM_NO_ACK streams has no delivery or order guarantees (for unreliable but order
	// data, use STREAM_CUMULATIVE_ACK with only unreliable messages). So, any incoming message is
	// "accepted" and bypassed to higher level
	if (type == STREAM_NO_ACK) {

		int index = 0;
		if (last_reliable_msg == ZIG_INVALID_MSG_ID) index = ZIG_FIRST_MSG_ID;
		else index = (last_reliable_msg + 1) % MAX_INCOMING_MESSAGES;

		if (queue_size == MAX_INCOMING_MESSAGES) {
			in.skip( msgsize );
			return;
		}

		// paranoia
		message[index].body.clear();

		message[index].dummy = false;
		// save the message in the right place
		message[index].body.putBlock( in.data_cur(), msgsize);

		// update queue top
		if (last_reliable_msg == ZIG_INVALID_MSG_ID) last_reliable_msg = ZIG_FIRST_MSG_ID;
		else last_reliable_msg = inc_msg_id (&last_reliable_msg);
		queue_size++;

		st->con->xprintf(0, "%d - putting message %d in slot %d, queue_size = %d\n", st->get_local_port(), msgid, index, queue_size);

		// advance "in" parsing
		in.skip( msgsize );
		return;
	}

	//check old messages (discard)
	if ( older_msg_id(msgid, msg_current) ) {

		// advance buffer parsing ANYWAY
		in.skip( msgsize );	

		return;
	}

	//check window overflow (discard)
	if ( diff_msg_id(msgid, msg_current) >= MAX_INCOMING_MESSAGES ) {

		// advance buffer parsing ANYWAY
		in.skip( msgsize );		

		return;
	}

	//calc index of the message in the array.
	int index = msgid % MAX_INCOMING_MESSAGES;
	
	//con->xprintf(0, "stream_c:: index = %d\n", index);

	// NEW (ziglite 1.2)
	// detect lost unreliable msgs and fill their positions
	// in circular buffer with "dummy" msgs
	if ( (type == STREAM_CUMULATIVE_ACK) &&
	     older_msg_id(last_reliable_msg, msgid) ) {

		// hackish: unuseful test in last reliable message to find dummy message
		// (NEVER happens, but it simplifies the code, because we can set the last
		// reliable msg to the first message identifier)
		if (last_reliable_msg == ZIG_INVALID_MSG_ID) {
			last_reliable_msg = ZIG_FIRST_MSG_ID;
		}

		// starts to mark messages ONLY after current position (also)
		if ( older_msg_id(last_reliable_msg, msg_current) ) {
			last_reliable_msg = msg_current;
		}

		// fill with unreliable msgs interval (last_reliable, index)
		// hackish: test last message to simplify code in the case when
		// there's no last reliable message
		int dummy = last_reliable_msg % MAX_INCOMING_MESSAGES;

		// dont remove last reliable!
		//dummy = (dummy + 1) % MAX_INCOMING_MESSAGES;
	
		while (dummy != index) {

			// se não tem mensagem entre última mensagem reliable e atual, então é unreliable perdida!
			if ( message[dummy].body.size() == 0) {
				//con->xprintf(0, "discard msg posicao %d (pois last_reliable = %d)\n", dummy, last_reliable);
				message[dummy].dummy = true;
				queue_size++;
			}

			// next message in circular buffer
			dummy = (dummy + 1) % MAX_INCOMING_MESSAGES;
		}
	}

	//check for duplicate discard
	if ( ( message[index].body.size() > 0) && (message[index].dummy == false) ) {

		//	if (debug) LOG1("DUPLICATE DISCARD @ index = %i\n", index);
		in.skip( msgsize );
		
		return;
	}

	if ( (last_reliable_msg == ZIG_INVALID_MSG_ID) || (msgid > last_reliable_msg) ) last_reliable_msg = msgid;

	//con->xprintf(0, "saving the msg in appropriate place (%d)\n", index);

	//save the message
	message[index].body.clear();			//paranoia

	// if (debug) LOG2("in.datacur = %s msgsize = %i\n", in->data_cur(), msgsize);
	message[index].body.putBlock( in.data_cur(), msgsize);		//

	message[index].body.id = msgid;

	queue_size++;

	st->con->xprintf(0, "%d - putting message %d in slot %d, queue_size = %d\n", st->get_local_port(), msgid, index, queue_size);

	// advance "in" parsing
	in.skip( msgsize );
}

// set/change the remote address. returns 0 on failure, 1 on success. notation is "bla.bla.bla.bla:portnum". resets state.
int station_c::set_remote_address(char* address) {
	if (address == NULL) return 0;

	address_c temp(address);
	if (temp.valid() == false) return 0;
	
	reset_state();
	remote_addr = temp;
	return 1;
}

// set/change the remote address. returns 0 on failure, 1 on success. NL address struct. resets state.
int station_c::set_remote_address(NLaddress some_addr) {
	if (some_addr.valid == NL_FALSE) return 0;

	reset_state();
	remote_addr.set_address(some_addr);

	return 1;
}

// set/change the remote address. returns 0 on failure, 1 on success. resets state.
int station_c::set_remote_address(const address_c &some_addr) {
	
	if (some_addr.valid() == NL_FALSE) return 0;
	
	reset_state();
	remote_addr = some_addr;

	return 1;
}


//change the port of the remote address. resets state.
int station_c::set_remote_port(NLushort port, bool do_not_reset_state) {

	//v1.5.0: needed
	if (do_not_reset_state == false) {
		//resets state -- no messages pending to receive or send, cause changed port!
		reset_state();
	}

	remote_addr.set_port(port);

	return 1;
}

//get the port of the remote address
NLushort station_c::get_remote_port() { return (NLushort) remote_addr.get_port(); }

//get the port of the local address
NLushort station_c::get_local_port() {
	NLaddress addr;
	nlGetLocalAddr(msocket->get_sock(), &addr);
	return nlGetPortFromAddr(&addr);
}

// NEW: polling multiple stations support: 
// get this station's internal socket object. currently you will need it you want to poll several 
// stations for read/write status. 
msocket_c* station_c::get_socket() {
	return msocket;
}

// incoming packet from msocket
void station_c::incoming_msocket_packet() {

	bool drop_packet = false;
	int packet_id;
	buffer_c gamepacket; 

	connect_packet_left_count = 0;

	try {
		// process incoming packet
		gamepacket = process_incoming_packet( (*udp_read_buf), &packet_id);

	} catch (int ex) {	
		// EX_EOB, EX_NULL: just ignore packets that give errors when being read/parsed
		if ((ex == EX_EOB) || (ex == EX_NULL)) {
			drop_packet = true;
		}
	} 

	if (drop_packet == false) {
		beh->station_incoming_data(gamepacket, packet_id);
	}

	// else: IGNORE bad packets
	//
	// FIXME/REVIEW: maybe should notify app so an IP ban or something like that 
	//  can be arranged? risky, because IP addresses can be forged and this would 
	//  result in wrong IP banning anyway. better to just drop game packets that 
	//  are detected as malformed either on the "client" or on the "server".. ?
	//
	// NOTE: the application can still detect EX_EOB / EX_NULL when it is doing 
	//  its parsing its OWN protocol. this only discards packets that don't 
	//  conform to the LEETNET/ZIGNET protocol.
}

// process incoming packet. returns unreliable data portion of packet. also returns in the ".code"
// field of the buffer_c returned, the type of packet that arrived. possible types:
// STATION_INCOMING_PACKET_NORMAL: a regular game packet
// STATION_INCOMING_PACKET_SPECIAL: a special packet (sent with send_raw_packet())
buffer_c station_c::process_incoming_packet(buffer_c &in, int *packet_id_ptr) {

	//con->xprintf(1, "(%s) station_c::process_incoming_packet\n", dstr);
	con->xprintf(0, "port %d - raw packet:\n%s", get_local_port(), buffer_print(in).c_str());

	// |------------------------------------------------------------------------|
	// | IMPORTANT! 32K encoding                                                |
	// | Zige uses a special way for enconding numbers in range [0, 32767]:     |
	// |                                                                        |
	// | --> Numbers lower than 128 are codified in only a byte, with first bit |
	// | (most significantely) off, and the 7 bits remains with the traditional |
	// | binary codification of the number.                                     |
	// |                                                                        |
	// | --> Numbers greater or equal than 128 are codified in 2 bytes, with    |
	// | first bit of 1st byte on, the remains 7 bits of first byte with LESS   |
	// | significantely bits and the 2rd byte with remains 8 bits of number.    |
	// |                                                                        |
	// | Remeber this when see variable numbers of bytes in protocol            | 
	// |------------------------------------------------------------------------|
	//
	// PACKET FORMAT
	//
	// --> (2) NLushort id
	//         BITS:
	//               (1) "vport bit": indicates if next byte if the vport byte
	//               (1) "compression bit": indicates if rest of packet was compressed with bzip2 algorithm
	//               (14) (0..16384) id: id of packet (implement wraparound)
	//
	// --> if "vport bit" of previous short is set...
	//     (1-2) vport (32K)
	//
	// --> for every stream sent in packet
	//
	//         see "stream_c::fill_packet" method for more details
	//
	// --> after id stream 0...
	//     REST OF MESSAGE: unrealiable data chunk

	string description;
	char auxiliar_string[1024];

	// Buffer created for error conditions
	buffer_c special_pkt = in;
	special_pkt.code = STATION_INCOMING_PACKET_SPECIAL;
	special_pkt.seek(0);

	int i;
	
	// list of acks for each stream in the packet. they are used just in this function
	int messages_acks[2048];

	//reset pos
	in.seek(0);

	//packet id
	NLushort packet_id = in.getShort();

	sprintf(auxiliar_string, "packet ID = %d\n", packet_id);
	description.append(auxiliar_string);

	// NEW (ZIGLITE 1.1): if vport bit is set then must read the extar vport ID data. actually
	// the vport ID it is not needed here (data was already read by msocket and routed to this 
	// station based on the vport ID...) but we must read it out anyways because it is not 
	// removed by the msocket read-packet routine.
	if (packet_id & STATION_VPORT_BIT) in.get32K();

	// NEW (ziglite): by default, fill packet_id_ptr with the ushort packet_id read 
	// from the packet. if it was 0 or some special value, it will show to the caller.
	if (packet_id_ptr != 0) (*packet_id_ptr) = (int) packet_id;
	
	// SPECIAL CASE: packet_id == 0 means a special packet or something like that
	// do not process it, pass unchanged to caller
	if (packet_id == 0) return special_pkt;

	// *** FIXED : now that we're past the weird high-value special-case packet ids (65***)..
	//             ...do this:

	//KEEP TRACK OF BASE PACKET ID:
	//base = 0, id = 65280, int = 65280
	//base = 0, id = 1, new base = 65280, int = 65281
	//base = 65280, id = 65280, int = 130560
	//base = 65280, id = 1, new base = 130560, id = 130561

		// if packet id is less than it's last value, then it's a wrap-around
		// v1.5.0: SERIOUS bug: it might NOT be a wrap around but instead a LATE PACKET! (duh!)
		//
		//    if (packet_id < last_packet_id)
		//      base_packet_id += LAST_USHORT_ID;
		//
		// fix:
		//  a wrap-around (forwards or backwards) occured if the abs() of the 
	    //     difference value is BIG (> HALF_USHORT_ID)
		//  if difference new-old is positive, back wrap
	    //  if difference new-old is negative, fwd wrap
		//
	int id_dif = packet_id - last_packet_id;
	if (abs(id_dif) > ZIG_HALF_PACKET_ID) { 
		// dif is big
		if (id_dif > 0) { // back wraparound
			base_packet_id -= ZIG_LAST_PACKET_ID; // subtract base packet id
		}
		else {	// forward wraparound
			base_packet_id += ZIG_LAST_PACKET_ID; // add base packet id 
		}
	}

	// record last packet id value
	last_packet_id = packet_id;

	// return the packet id if asked to...
	// NEW : wrap around packet id nicely, so when the 16-bit incoming ID reaches ZIG_LAST_PACKET_ID, 
	//       the returned "int" ID (32-bit signed) will keep increasing
	if (packet_id_ptr != NULL) {
		// int packed_id = base_packet_id + USHORT packet_id (1..16320)
		(*packet_id_ptr) = base_packet_id + (int) packet_id;
	}

	// NEW (ZIGLITE 1.2): Read stream ID
	NLushort stream_id;
	stream_c* str;

	while (in.size_left() > 0) {

		stream_id = in.get32K();
		if (stream_id == 0) break;

		str = search_stream((int) stream_id);
		if (str == NULL) return buffer_c::getEmptyBuffer();

		string stream_name;
		switch (str->type) {
			case STREAM_CUMULATIVE_ACK: stream_name = "cumulative ack"; break;
			case STREAM_INDIVIDUAL_ACK: stream_name = "individual ack"; break;
			case STREAM_NO_ACK: stream_name = "unreliable"; break;
			default: stream_name = "unknown";
		}

		sprintf(auxiliar_string, "  stream #%d (%s)\n", stream_id, stream_name.c_str());
		description.append(auxiliar_string);

		// read nack (number of acks in the packet) and nrel (number of reliable messages in the packet)
		// ZIGLITE v1.1: 1 to 3 bytes (see above)

		//11-bit values
		int nack = 0;
		int nrel = 0;		

		if (str->type == STREAM_CUMULATIVE_ACK ) {
			nack = 1;
			nrel = in.get32K();
		}
		else if (str->type == STREAM_NO_ACK) {
			nack = 0;
			nrel = in.get32K();
		}
		else if (str->type == STREAM_INDIVIDUAL_ACK) {
		
			nrel = in.get32K();
			nack = in.get32K();
		}
	
		sprintf(auxiliar_string, "    Number of messages: %d. Number of acks = %d\n", nrel, nack);
		description.append(auxiliar_string);

		if (nack > 0) {
			description.append("    ACKS:");
		}

		// all ack/rel IDs are full shorts
		for (i = 0; i < nack; i++) {
			
			NLushort ack = in.getShort(); 
			if ( (str->type == STREAM_CUMULATIVE_ACK) && (ack == ZIG_INVALID_MSG_ID) ) {
				nack = 0;
				description.append(" none");
				break;
			}
			else {
				messages_acks[i] = ack & (~STATION_RELIABLE_BIT);
				sprintf(auxiliar_string, " %d", messages_acks[i]);
				description.append(auxiliar_string);
			}
		}

		if (nack > 0) {
			description.append("\n");
		}

		// 1.5.0 NEW: all ack/rel IDs are full shorts (no more delta)
		NLushort msgid;
		int msgsize;
		bool reliable;

		if (nrel > 0) {
			description.append("    Messages IDs:");
		}

		string messages_string;
		for (i = 0; i < nrel; i++) {

			//message ID (full)
			msgid = in.getShort();
			reliable = (msgid & STATION_RELIABLE_BIT) != 0;

			// reliable bit off
			msgid &= ~STATION_RELIABLE_BIT;

			sprintf(auxiliar_string, " %d%c", msgid, (reliable ? 'R' : 'U'));
			description.append(auxiliar_string);

			//message size (upto 32K)
			msgsize = in.get32K();

			buffer_c temp;
			temp.putBlock( in.data_cur(), msgsize );
			sprintf(auxiliar_string, "    Message %d: %d bytes, data = ", msgid, msgsize);
			messages_string.append(auxiliar_string);

			if (msgsize == 0) messages_string.append("<empty>\n");
			else messages_string.append(buffer_print(temp));

			// station will process the incoming reliable message
			str->process_incoming_message(msgid, msgsize, in);

			if (str->type == STREAM_INDIVIDUAL_ACK) {
				str->ackbuf.push_back( msgid );
			}
			else if (str->type == STREAM_CUMULATIVE_ACK) {
				str->send_ack = true;
				// possible "roles" in message sequence ALWAYS are unreliable msgs
				if ( (str->cumulative_ack == ZIG_INVALID_MSG_ID) || (msgid > str->cumulative_ack) ) str->cumulative_ack = msgid;
			}
			// else NO_ACK, skip...
		}

		if (nrel > 0) {
			description.append("\n");
			description.append(messages_string);
		}

		// (3) for every incoming ack, check if exist a reliable message in the outgoing buffer
		//     for it. if yes, delete it from the buffer (id = -1 and clear buffers)

		// check all acks incoming...
		if (str->type == STREAM_CUMULATIVE_ACK) {

			// for every message in the outgoing buffer...
			deque<omsg_c>::iterator it = str->reliable.begin();
			while (it != str->reliable.end()) {

				if (nack == 0) break;
	
				omsg_c& omsg = (*it);
				//con->xprintf(0, "CUMULATIVE: conferindo ack %d com a msg %d\n", messages_acks[0], omsg.id);
				if (messages_acks[0] >= (omsg.id & (~STATION_RELIABLE_BIT)) ) {

					//if (debug) LOG1("*** ACKED OUTGOING RELIABLE MESSAGE ID = %i ***\n", omsg.id);
					//con->xprintf(0, "(%s) deleting msg %d from reliable list (incoming ack >)\n", dstr, omsg.id);

					// if ack was expected, insert it into reliable acks deque
					if (omsg.ack_expected) str->racks.push_back( omsg.id );

					// match : message acked - remove
					it = str->reliable.erase(it);
				}
				else break;
			}

		}
		else if (str->type == STREAM_INDIVIDUAL_ACK) {
			for (int a = 0; a < nack; a++) {

				// for every message in the outgoing buffer...
				deque<omsg_c>::iterator it = str->reliable.begin();
				while (it != str->reliable.end()) {

					omsg_c& omsg = (*it);
					//con->xprintf(0, "conferindo ack %d com a msg %d\n", messages_acks[a], omsg.id);
					if ( messages_acks[a] == (omsg.id & (~STATION_RELIABLE_BIT)) ) {

						//if (debug) LOG1("*** ACKED OUTGOING RELIABLE MESSAGE ID = %i ***\n", omsg.id);
						//con->xprintf(0, "(%s) deleting msg %d from reliable list\n", dstr, omsg.id);

						// if ack was expected, insert it into reliable acks deque
						if (omsg.ack_expected) str->racks.push_back( omsg.id );

						// match : message acked - remove
						str->reliable.erase(it);
						break;
					}

					it++;
				}
			}
		}
		// else NO_ACK, skip...
		
		str->last_incoming_packet_time = get_time();
	}

	//resto do packet: udp unreliable

	//if (debug) LOG1("SIZE LEFT unreliable block size = %i\n", in.size_left() );
	//con->xprintf(0, "(%s) unreliable block (%d bytes) \n", dstr, in.size_left());
	buffer_c umsg;

	umsg.putBlock( in.data_cur(), in.size_left() );
	umsg.code = STATION_INCOMING_PACKET_NORMAL;
	umsg.seek(0);

	if (umsg.size() > 0) {
		sprintf(auxiliar_string, "  stream #0 : %d bytes, data = ", umsg.size());
		description.append(auxiliar_string);
		description.append(buffer_print(umsg));
	}

	con->xprintf(0, "%s", description.c_str());

	return umsg;
}

// reads next message in the arrival queue into the supplied buffer. 
buffer_c stream_c::get_message() {

	buffer_c msg;

	// calc index of the message in the array
	int index = msg_current % MAX_INCOMING_MESSAGES;

	// NEW (ziglite 1.2): Discard dummy messages (unreliable lost)
	while (message[index].dummy) {
		st->con->xprintf(0, "%d - discard lost unreliable msg %d, new queue size = %d\n", st->get_local_port(), index, queue_size-1); 
		index = (index + 1) % MAX_INCOMING_MESSAGES;
		msg_current = inc_msg_id( &idgen_reliable_rec );
		queue_size--;
	}

	//check if current is loaded yet
	if (message[index].body.size() == 0) {
		msg.code = 0;
//		st->con->xprintf(0, "%d - looking for position %d in the buffer (queue_size = %d)... empty\n", st->get_local_port(), index, queue_size);
		return msg;
	}

	//message present - create return data
	msg = message[index].body;

	//reset pos, ready for parsing!
	msg.seek(0);

	//clear msg (slide window)
	message[index].body.clear();
	
	//update msg_current (slide window)
	msg_current = inc_msg_id( &idgen_reliable_rec );

	queue_size--;	
	//ok
	msg.code = 1;

	st->con->xprintf(0, "%d - give message %d to app (queue_size = %d)\n", st->get_local_port(), index, queue_size);

	return msg;
}

const bool stream_c::test_send(const unsigned int iPacketQuota)
{
	// Verifica se pacote precisa ser enviado agora
	const double dNow = get_time();
	if (dNow < get_next_send()) return false;

/* TODO: if we avoid message starvation, so we don't need to control stream size to verify if stream will be sent in next packet

	// Numero maximo de bytes a ser colocado no pacote
	unsigned int iStreamQuota = get_policy()->get_max_outgoing_flow();
	if (iStreamQuota > iPacketQuota) iStreamQuota = iPacketQuota;

	unsigned int iStreamSize = 0;
	switch (type)
	{
		// cumulative ack: stream id + numMessages + cumulative ack
		case STREAM_CUMULATIVE_ACK:
			iStreamSize = 2 + 2 + 2;
			break;

		// individual: stream id + numMessages/numAcks + ack list
		case STREAM_INDIVIDUAL_ACK:
			iStreamSize = 2 + 3 + (2 * ackbuf.size());
			break;

		// no_ack: stream id + numMessages
		case STREAM_NO_ACK:
			iStreamSize = 2 + 2;
			break;
	}
*/
	for (deque<omsg_c>::iterator it = reliable.begin(); it != reliable.end(); it++)
	{
		// TODO: avoid message starvation OR control stream size?
		// if (iStreamSize + it->body.size() > iStreamQuota) continue;

		// send only reliable msg after resend interval
		if (dNow >= it->next_send) return true;
	}

	if ( (type == STREAM_CUMULATIVE_ACK) && (send_ack == true) ) return true;
	if ( (type == STREAM_INDIVIDUAL_ACK) && (ackbuf.size() > 0) ) return true;

	return false;
}

const bool stream_c::fill_packet(buffer_c& oPacket, const unsigned int iPacketQuota)
{
	// Stream syntax
	//         (1-2) "stream_id" (32K)
	//         ATTENTION: if stream_id = 0: END OF STREAMS (next bytes = unreliable big chunk)
	//
	//         (1-2) "numMessages": number of messages in packet (32K)
	//
	//     --> if stream type is CUMULATIVE ACK
	//         (2) cumulative ack
	//
	//     --> else if stream type is INDIVIDUAL_ACK
	//         (1-2) "numAcks": number of acks in packet (32K)
	//
	//         --> "numAcks" times...
	//         (2) NLushort "ack_id": acked message id
	//
	//     --> "numMessages" times...
	//             (2) NLushort message id
	//             (1-2) "msize" message size (32K)
	//             (msize) chars: message data

	const double dNow = get_time();
	if (test_send(iPacketQuota) == false) return false;

	// Numero maximo de bytes a ser colocado no pacote
	unsigned int iStreamQuota = get_policy()->get_max_outgoing_flow();
	if (iStreamQuota > iPacketQuota) iStreamQuota = iPacketQuota;

	unsigned int iNumMessages = 0;
	unsigned int iStreamSize = 0;

	// stream ID
	iStreamSize += (m_iStreamId >= 0x80) ? 2 : 1;

	// number of reliable messages (estimative)
	iStreamSize += (m_iStreamId >= 0x80) ? 2 : 1;

	switch (type)
	{
		// cumulative ack: cumulative ack
		case STREAM_CUMULATIVE_ACK:
			iStreamSize += 2;
			break;

		// individual: numAcks + ack list
		case STREAM_INDIVIDUAL_ACK:
			iStreamSize += (ackbuf.size() >= 0x80) ? 2 : 1;
			iStreamSize += (2 * ackbuf.size());
			break;
	}

	// (1) while less than max quota, add messages.
	//     this loop outputs to the "numMessages" variable.
	for (deque<omsg_c>::iterator it = reliable.begin(); it != reliable.end(); it++)
	{
		// encoding limit reached (individual ACK)
		if (iNumMessages == 32767) break;

		// msg ID + msg length + msg body
		const unsigned int iMessageSize = 2 + 2 + it->body.size();

		// avoid message starvation (sends at least ONE message)
		if ( (iNumMessages > 0 ) && (iStreamSize + iMessageSize > iStreamQuota) ) continue;

		// NEW (send only reliable msg after resend interval)
		if (dNow >= it->next_send)
		{
			// so we're adding it (msg ID + msg length + msg body)
			iStreamSize += iMessageSize;
			iNumMessages ++;
		}
	}

	oPacket.put32K(m_iStreamId);
	oPacket.put32K(iNumMessages);

	if (type == STREAM_CUMULATIVE_ACK)
	{
		oPacket.putShort(cumulative_ack);
		send_ack = false;
	}
	else if (type == STREAM_INDIVIDUAL_ACK)
	{
		NLushort iNumAcks = ackbuf.size();      // put all acks in there (OPTIMIZATION-POINT)
		if (iNumAcks > 2047) iNumAcks = 2047;   // 0..2047 : 11-bit limit
		oPacket.put32K(iNumAcks);

		//NEW (v1.5.0): no more packet ID deltas -- send full ushort ID always
		for (vector<NLushort>::iterator it = ackbuf.begin(); (it != ackbuf.end()) && (iNumAcks > 0);)
		{
			oPacket.putShort(*it);

			// next...
			iNumAcks--; // avoid fitting more acks in a packet than the limit
			it = ackbuf.erase(it);
		}
	}
	// else ("no ack") skip...

	// write "iNumMessages" messages in the send buffer
	for (deque<omsg_c>::iterator it = reliable.begin(); (it != reliable.end()) && (iNumMessages > 0);)
	{
		if (it->next_send > dNow)
		{
			it++;
			continue;
		}

		// msg ID
		NLushort iMessageId = it->id;

		// put msg id, length, data
		oPacket.putShort( iMessageId );
		oPacket.put32K( it->body.size() );
		oPacket.putBlock( it->body.data(), it->body.size() );

		iNumMessages--;

		// NEW (ZIGLITE 1.2): Remove unreliable messages from list
		if (!(iMessageId & STATION_RELIABLE_BIT))
		{
			it = reliable.erase(it);
			continue;
		}
		else
		{
			// NEW: policy controls when send this message after timeout interval
			it->next_send = dNow + policy->get_resend_interval();
			it++;
		}
	}

	increment_next_send();

	// ZIG 2.0: Delete unreliable unsent messages if needed
	if ( policy->get_discard_unsent_unreliables() == true )
	{
		for(deque<omsg_c>::iterator it = reliable.begin(); it != reliable.end();)
		{
				// msg ID
				NLushort iMessageId = it->id;

				// remove unreliable messages from list
				if (iMessageId & STATION_RELIABLE_BIT != STATION_RELIABLE_BIT)
				{
					it = reliable.erase(it);
				}
				else
				{
					it++;
				}
		}
	}

	last_outgoing_packet_time = get_time();

	return true;
}

// reads next message of an stream 
// returns 0 if no more messages to read, 1 if success.
int station_c::read(buffer_c &msg, int stream_id) {

	if (stream_id < 0) stream_id = default_stream_id; 
	stream_c* str = search_stream(stream_id);
	if (str == NULL) return 0;

	msg = str->get_message();

	return msg.code;
}
	
// reads next message of an stream
// returns buffer_c with the message and ".code" = 0 if no more messages to read, 1 if success.
buffer_c station_c::read(int stream_id) {

	if (stream_id < 0) stream_id = default_stream_id; 
	stream_c* str = search_stream(stream_id);
	if (str == NULL) return buffer_c();

	return str->get_message();
}

// appends message to the queue. if it's a realiable message, it will be sent as many times
// as needed until it's acknowledged by the other side. If it's a unrealible message, it will
// be send only one time.
//
// msg_id: if you want to be notified when this message is ACK'ed by the receiver, pass a valid int
// pointer via msg_id. the pointer will be filled with the ID of the outgoing message, that can be
// used to identify the ack when it comes through read_ack()
int station_c::write(buffer_c &msg, int stream_id, bool reliable, int *msg_id){

	if (stream_id == 0) {
		// appends unreliable data to the packet buffer. the unreliable data are
		// collapsed and are sent as a big chunk when send_packet() is called.
		//con->xprintf(0, "station_c::write unreliable big chunck += %d bytes\n", msg.size());

		unrelbuf.putBlock(msg.data(), msg.size());
		return 1;
	}

	if (stream_id < 0) stream_id = default_stream_id;
	stream_c* str = search_stream(stream_id);
	if (str == NULL) return 0;

	// FIXED (v1.3.0): messages longer than the maximum message size limit are not sent and an 
	// error code is returned (0)
	if (msg.size() > 32767) return 0;

	// create outgoing msg obj
	omsg_c omsg;
	omsg.body = msg;                            	// copy body
	
	omsg.id = str->idgen_reliable_send;         	// set a new unique id
	inc_msg_id(&(str->idgen_reliable_send));	// generate next unique id

	// NEW (ziglite 1.2): smuggled flag in 1st bit (reliable or unreliable message)
	if (reliable) omsg.id |= STATION_RELIABLE_BIT;

	// message is expecting ack?
	if (msg_id != NULL) {
		(*msg_id) = omsg.id;                               // pass generated id of message to caller
		omsg.ack_expected = true;
	}
	else omsg.ack_expected = false;                        // no ack expected

	// NEW: policy controls when a message will be sent (1st send is immediately)
	omsg.next_send = get_time();

	str->reliable.push_back(omsg);

/*
	if (reliable) con->xprintf(0, "station_c::write putting reliable msg (%d bytes) in list (%d msgs)\n", omsg.body.size(), str->reliable.size());
	else con->xprintf(0, "station_c::write putting unreliable msg (%d bytes) in list (%d msgs)\n", omsg.body.size(), str->reliable.size());
*/

	return 1; // OK
}

// alias for 'generic' method
int station_c::write_reliable(buffer_c &msg, int stream_id, int* msg_id) { return write(msg, stream_id, true, msg_id); }
int station_c::write_unreliable(buffer_c &msg, int stream_id) { return write(msg, stream_id, false, NULL); }

// read a pending reliable message ack. does not block. returns -1 if no acks to be read,
// else returns the id of the message that was acked (same "msg_id" of write() call)
// call this after a packet arrives (that's the only time the acks are updated)
int station_c::read_ack(int stream_id) {
	if (stream_id < 0) stream_id = default_stream_id;
	stream_c* str = search_stream(stream_id);
	if (str == NULL) return -1;

	//nothing to read
	if (str->racks.size() == 0) return -1;

	//read next, take from deque
	int acked_msg_id = str->racks[0];
	str->racks.pop_front();

	return acked_msg_id;
}

// checks whether a send_packet() call would dispatch a "dummy packet", that is, 
// a packet without any useful payload (reliable message acks, reliable message 
// retransmissions or unreliable data block). 
// returns TRUE if there is nothing to send (in which case, following with a 
// send_packet() call would be wasteful, unless it was intended as a keepalive or 
// something like that), FALSE otherwise.
bool station_c::nothing_to_send() {

	// has to send: unreliable chunck
	if (unrelbuf.size() > 0) return false;

	stream_c* str = NULL;
	for (map<int, stream_c*>::iterator it = streams.begin(); it != streams.end(); it++)
	{
		str = it->second;
		if (str->test_send(max_outgoing_packet_size) == true) return false;
	}

	return true;
}

// flush the packet buffers as an UDP packet to the remote address, returns "id"
// for the assigned packet id. this call resets the unreliable data buffer (see write() above).
int station_c::send_packet(int *id_ptr)
{
	if (nothing_to_send() == true) return SENDPACKET_OK;

	int iPacketId = 0;
	
	// assign id to the outgoing packet
	iPacketId = (int) idgen_packet_send;
	inc_packet_id(&idgen_packet_send);

	// FIXED : id_ptr: receber INTEGER outgoing packet id, não ushort id
	if (iPacketId < last_o_packet_id) base_o_packet_id += ZIG_LAST_PACKET_ID;

	last_o_packet_id = iPacketId;

	if (id_ptr) (*id_ptr) = base_o_packet_id + iPacketId;

	// |------------------------------------------------------------------------|
	// | IMPORTANT! 32K encoding                                                |
	// | Zige uses a special way for enconding numbers in range [0, 32767]:     |
	// |                                                                        |
	// | --> Numbers lower than 128 are codified in only a byte, with first bit |
	// | (most significantely) off, and the 7 bits remains with the traditional |
	// | binary codification of the number.                                     |
	// |                                                                        |
	// | --> Numbers greater or equal than 128 are codified in 2 bytes, with    |
	// | first bit of 1st byte on, the remains 7 bits of first byte with LESS   |
	// | significantely bits and the 2rd byte with remains 8 bits of number.    |
	// |                                                                        |
	// | Remeber this when see variable numbers of bytes in protocol            | 
	// |------------------------------------------------------------------------|
	//
	// PACKET FORMAT
	//
	// --> (2) NLushort id
	//         BITS:
	//               (1) "vport bit": indicates if next byte if the vport byte
	//               (1) "compression bit": indicates if rest of packet was compressed with bzip2 algorithm
	//               (14) (0..16384) id: id of packet (implement wraparound)
	//
	// --> if "vport bit" of previous short is set...
	//     (1-2) vport (32K)
	//
	// --> for every stream sent in packet
	//
	//         see "stream_c::fill_packet" method for more details
	//
	// --> after id stream 0...
	//     REST OF MESSAGE: unrealiable data chunk

	//clear sendbuf (important!!)
	sendbuf->clear();

	int vport = remote_addr.get_virtual_port();
	//ZIGLITE 1.1: if this station's vport > 0, turn on the VPORT BIT on the packet id ushort
	if (vport > 0) iPacketId += STATION_VPORT_BIT;

	// How many bytes we can put in packet (max)
	int packet_size_left = max_outgoing_packet_size;

	//packet id, WITHOUT the compression flag set. we will rewrite the packet id later below if the data
	//size exceeds "compression_config" bytes
	sendbuf->putShort(iPacketId);

	//ZIGLITE 1.1: if this station's vport > 0, then must write, after the packet ID, the vport value
	if (vport > 0) sendbuf->put32K( vport );

	stream_c* str;        // pointer to stream instance
	int stream_id;
	buffer_c oStreamData;

	for (std::map<int, stream_c*>::iterator it = streams.begin(); it != streams.end(); it++)
	{
		packet_size_left = max_outgoing_packet_size - sendbuf->size();

		str = it->second;
		stream_id = it->first;

		oStreamData.clear();
		if (str->fill_packet(oStreamData, packet_size_left) == true)
		{
			con->xprintf(0, "(%s) stream %d has the following buffer to add: %s", dstr, stream_id, buffer_print(oStreamData).c_str());
			sendbuf->putBlock(oStreamData.data(), oStreamData.size());
		}
	}

	if ( unrelbuf.size() <= packet_size_left) {
		unrelbuf.seek(0);
		//con->xprintf(0, "%d bytes in unreliable big chunck\n", unrelbuf.size());
		sendbuf->putByte( 0 );
		sendbuf->putBlock( unrelbuf.data(), unrelbuf.size() );
		unrelbuf.clear();
	}

	// NEW: may send sendbuf or sendbuf_zipped
	char *the_sendbuf_data = sendbuf->data();  //default
	int   the_sendbuf_size = sendbuf->size();

	con->xprintf(0, "I will send %s", buffer_print(*sendbuf).c_str());

#ifndef ZIG_NO_BZIP2 // don't include BZIP2

	// NEW: check if we must compress the packet!
	if ((compression_config >= 0) && (sendbuf->size() >= compression_config)) {

		//v1.3.2: move this up since this allows to calculate the amount of data to be compressed
		// (sendbuf.size_left())
		sendbuf->seek(2);		//seek source to skip the ID byte

		// v1.3.0: trick to eliminate need to allocate a "char sendbuf_zipped[]" with a fixed size:
		// - first check if enough space allocated on sendbuf_zipped_b (a buffer_c) to fit the
		// amount of data that is being compressed ( sendbuf.size_left() )
		// from BZIP2: "To guarantee that the compressed data will fit in its buffer, allocate 
		// an output buffer of size 1% larger than the uncompressed data, plus six hundred extra bytes."
		sendbuf_zipped_b->ensure_capacity( (int) ((double) sendbuf->size_left() * 1.01f) + 601 );
		// - second, trick the old code into using sendbuf_zipped which points to a buffer
		// that has enough space to allocate the compressed data 
		char *sendbuf_zipped = sendbuf_zipped_b->data();

		// must zip stuff! zipped packet:
		// - packet ID with higher bit (32768/STATION_COMPRESSED_PACKET_BIT) set
		int count = 0;
		writeShort(sendbuf_zipped, count, (NLushort)(iPacketId + STATION_COMPRESSED_PACKET_BIT));

		// - everything else in compressed form
		unsigned int dest_size = sendbuf_zipped_b->size() - 2; //v1.3.1: bug fixed, was "= sizeof(sendbuf_zipped) - 2";
		
		int compress_result = BZ2_bzBuffToBuffCompress(
			(char*) &(sendbuf_zipped[2]),		//dest buffer initial position ptr
			&dest_size,                     //dest buffer available size and return value = compressed size written to dest
			sendbuf->data_cur(),							//src buffer initial position ptr
			(unsigned int)sendbuf->size_left(), //src buffer amount of uncompressed data to compress
			zig_bzip2_compression_level,0,0);

		// if compression error, just give up and drop the packet
		if (compress_result != BZ_OK) return SENDPACKET_OK;

		// it is possible that zipped data somehow is larger than the normal one. 
		// if the compression was usefull use the compressed data, otherwise normal data will be used
		if (the_sendbuf_size > 2 + (int) dest_size) {

			// send zipped buffer
			the_sendbuf_data = sendbuf_zipped;
			the_sendbuf_size = 2 + dest_size;  //2 (ID!!) + zipped content
		}
	}

	//add up statistics
	zig_stat_normal_bytes += sendbuf->size();
	zig_stat_zipped_bytes += the_sendbuf_size; //the_sendbuf_size can be zipped or not

#endif // #ifndef ZIG_NO_BZIP2 // don't include BZIP2

	buffer_c temp_pkt;
	temp_pkt.putBlock(the_sendbuf_data, the_sendbuf_size);

	// Unuseful packets will be NEVER sent (if you want keep-alive connection, please use
	// RTT estimative protocol or send_raw_packet!)
	if (temp_pkt.size() <= 3) return SENDPACKET_OK;
 	
	int result = emulate_latency_and_loss(temp_pkt, remote_addr);

	//con->xprintf(1, "(%s) Sending %d bytes to remote station (packet ID = %d)\n", dstr, the_sendbuf_size, id);

	switch (result) {
		case -1:
			//ERROR: should close connection.
			return SENDPACKET_SOCKET_ERROR;
		case 0 :
			// nlWrite didn't sent any data. Just ignores and go ahead!
			// It is not a problem because nlWrite returns 0 when its network buffers
			// are full; so we need to send the data later. (Check the nlWrite Reference on Web)
			// Reliable messages will be sent later anyway by zig engine :-)
			// and Unreliable messages can be lost, anyway :-)
			break;
		default:
			if (result < the_sendbuf_size) {
				// TODO: What should we do in the case nlWrite send some (but not all) data?
			}
			// NOTE: if result == the_sendbuf_size, then sending was successful!
			break;
	}

	//ok
	return SENDPACKET_OK;
}

// send a raw UDP packet to the destination. use when you want to send a message to the other
// site without the "RUDP protocol" overhead. UDP packets with the first short == 0 are "special"
// and not treated as rudp packets (see process_incoming_packet()). returns result of msocket::send_packet()
int station_c::send_raw_packet(buffer_c &udp) {

	int result = 0;	

	//can't send to invalid address!
	if (remote_addr.valid() == false) {
		//con->xprintf(1,"CAN'T SEND RAW PACKET TO INVALID remote_addr!\n");
		return 0;
	}

	result = emulate_latency_and_loss(udp, remote_addr);

	if (result > 0) {
		// add up statistics
		zig_stat_normal_bytes += result; // the original buffer's size
		zig_stat_zipped_bytes += result; // 2 (ID!) + zipped content
	}

	return result;
}

/// Called by msocket when this station has to send "connection accepted" packets
/// @param _connection_id: Connection identifier ("avoid" old packets)
/// @param answer: buffer with custom response to remote connect request
void station_c::accept_connection(NLubyte _connection_id, buffer_c& answer, const buffer_c& data) {

	con->printf("accept_connection setting to %i\n", _connection_id);
	remote_connection_id = _connection_id;

	bool flag = is_connecting();
	if (flag == false) local_connection_id = (rand() % 255) + 1;

	accept_packet.clear();
	accept_packet.putShort( STATION_CONNECT_ACCEPT_PACKET_ID );
	accept_packet.putByte( remote_connection_id );
	accept_packet.putByte( local_connection_id );
	accept_packet.putBlock( answer.data(), answer.size() );

	connect_packet_left_count = connect_attempts;
	connect_packet_next_time = get_time();

	station_state = STATION_CONNECTED;
	if (flag) {
		con->printf("calling p2psel0, connect packet left count = %i\n", connect_packet_left_count);
		beh->station_connected(const_cast<buffer_c&>(data), true);
	}
}

// create a new stream, with own policy and type (cumulative or individual ACK)
int station_c::create_stream(int type, policy_c policy) {
	int new_id;

	// find a unique ID for new stream (need optimization)
	// HINT: "ID generator" (wrap)
	for (new_id = 1; search_stream(new_id) != NULL; new_id++);

	// create stream and put it in the STL map
	streams[new_id] = new stream_c(new_id, type, policy);

	streams[new_id]->st = this;

	return new_id;
}

// find the stream ID in the map
stream_c* station_c::search_stream(int stream_id) {

	if (stream_id < 0) stream_id = default_stream_id;
	map<int, stream_c*>::iterator itr = streams.find(stream_id);

	if (itr == streams.end()) return NULL;
	else return itr->second;
}

// deletes a stream from the map
bool station_c::delete_stream(int stream_id) {

	if (stream_id < 0) stream_id = default_stream_id;
	map<int, stream_c*>::iterator itr = streams.find( stream_id );
	if (itr == streams.end()) return false;

	delete itr->second;
	streams.erase(itr);
	return true;
}

// metodos uteis para "percorrer" toda a lista de streams abertos
void station_c::seek_first_stream() {
	current_stream = streams.begin();
}

// retrieve next stream ID in the map
// CAUTION! DON'T erase or insert streams between two get_next() calls! (iterators
int station_c::get_next_stream() {
	if ( current_stream == streams.end() ) return -1;

	int stream_id = current_stream->first;

	current_stream++;

	return stream_id;
}

bool station_c::update_loss_info(NLushort packet_id) {

	if (max_incoming_packet_id == ZIG_INVALID_PACKET_ID) {
		max_incoming_packet_id = ZIG_FIRST_PACKET_ID;
		total_packets++;					
	}

	if (older_packet_id(max_incoming_packet_id, packet_id)) {
		total_packets += diff_packet_id(packet_id, max_incoming_packet_id);

		for (NLushort index = max_incoming_packet_id; index != packet_id; inc_packet_id(&index)) {
			received_packet_ids[index - ZIG_FIRST_PACKET_ID] = false;
		}
		received_packet_ids[max_incoming_packet_id - ZIG_FIRST_PACKET_ID] = true;
		received_packet_ids[packet_id - ZIG_FIRST_PACKET_ID] = false;

		max_incoming_packet_id = packet_id;
	}

	if (received_packet_ids[packet_id - ZIG_FIRST_PACKET_ID] != true) {
		received_packet_ids[packet_id - ZIG_FIRST_PACKET_ID] = true;
		total_received_packets++;
		return true;
	}
	else return false;
}

double station_c::get_loss_estimative() const {
	if ( (station_state == STATION_CONNECTED) && (total_packets > 0)) {
		return 1.0 - ((double) total_received_packets) / (double) total_packets;
	}
	else return -1.0;
}

void station_c::update_delay_info(NLushort packet_id) {

	if ( diff_packet_id(delay_last_id, delay_base_id) >= STATION_DELAY_WINDOW ) {
		inc_packet_id(&delay_base_id);
	}

	delay_packets[delay_last_id - ZIG_FIRST_PACKET_ID] = packet_id;
	inc_packet_id(&delay_last_id);
}

int compare_packet_ids(const void* const a, const void* const b) {
	
	if (older_packet_id(*((NLushort*) a), *((NLushort*) b))) return -1;
	else if (older_packet_id(*((NLushort*) b), *((NLushort*) a))) return +1;
	else return 0;
}

double station_c::get_delay_estimative() const {

	if (station_state != STATION_CONNECTED) return -1.0;

	NLushort temp[STATION_DELAY_WINDOW];
	NLushort temp2[STATION_DELAY_WINDOW];
	NLshort delta[STATION_DELAY_WINDOW];
	NLushort size = diff_packet_id(delay_last_id, delay_base_id);

	if (size == 0) return -1.0;

	for (NLushort index = delay_base_id, pos = 0; pos < size; inc_packet_id(&index), pos++) {
		temp[pos] = delay_packets[index - ZIG_FIRST_PACKET_ID];
		temp2[pos] = delay_packets[index - ZIG_FIRST_PACKET_ID];
	}

	qsort(temp, size, sizeof(NLushort), compare_packet_ids);

	for (NLushort index = 0; index < size; index++) {

		NLushort pos = 0;
		for (; pos < size; pos++) {
			if (temp2[pos] == temp[index]) break;
		}
		delta[index] = pos - index;
	}

	double result = 0.0;
	for (int i = 0; i < size; i++) {
		result += ((double) delta[i]) * ((double) delta[i]);
	}

	double divisor = ((double) ((size - 1) * size * (size+1))) / 3.0;

	result = result / divisor;
	return 1.0 - sqrt(result);
}

void station_c::update_rtt_info(const buffer_c& buf) {

	double now = get_time();
	buffer_c::iterator it = buf.begin();

	it.getShort();
	double start = it.getDouble();
	double diff = now - start;

	if (last_rtt_estimative == -1) {
		rtt_table[0] = diff;
		last_rtt_estimative = 0;
		return;
	}

	last_rtt_estimative = (last_rtt_estimative + 1) % STATION_MAX_RTT_TABLE;
	if (last_rtt_estimative == first_rtt_estimative) {
		first_rtt_estimative = (first_rtt_estimative + 1) % STATION_MAX_RTT_TABLE;
	}
	rtt_table[last_rtt_estimative] = diff;
}

double station_c::get_rtt_estimative() const {

	double result = 0.0;

	double factor = 1.0;

	if (last_rtt_estimative == -1) return -1.0;

	for (int index = last_rtt_estimative; index != first_rtt_estimative; index = (index + STATION_MAX_RTT_TABLE - 1) % STATION_MAX_RTT_TABLE) {
		factor = factor / 2.0;
		result += factor * rtt_table[index];
	}

	result += factor * rtt_table[first_rtt_estimative];

	return result;
}
