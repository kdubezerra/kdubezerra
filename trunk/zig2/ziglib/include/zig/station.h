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

	reliable UDP socket+brains

*/

#ifndef _ZIG_HEADER_STATION_H_
#define _ZIG_HEADER_STATION_H_

#include "zig.h"

#include <nl.h>

#include "leetnet.h"
#include "buffer.h"
#include "console.h"
#include "utils.h"
#include "address.h"
#include "policy.h"

// STL includes
#include <map>
#include <vector>
#include <list>
#include <deque>

// forward decl 
class msocket_c;

//possible return codes for send_packet():  (added in v1.3.0)
#define SENDPACKET_OK                     0    // no error
#define SENDPACKET_SOCKET_ERROR          -1    // when nlWrite() returns NL_INVALID

// this defines the circular queue size ( the true "sliding window" now is variable =) )
#define MAX_INCOMING_MESSAGES 2048
#define STATION_DEFAULT_WINDOW 256

// NEW (ziglite 1.1):  station protocol now includes a "connection handshake" of sorts
// ZIG 2.0: remover support to disconnection 2-way handshake
enum {
	STATION_SPECIAL_PACKET_ID = 0               // first ushort of a special ("non-gaming") packet
	,STATION_CONNECT_REQUEST_PACKET_ID = (ZIG_LAST_PACKET_ID + 1)
	                                            // first ushort of a "connect handshake request" packet
	,STATION_CONNECT_ACCEPT_PACKET_ID          // first ushort of a "connect handshake accept" packet
	,STATION_CONNECT_REJECT_PACKET_ID          // first ushort of a "connect handshake reject" packet

	,STATION_ABORT_PACKET_ID                    // first ushort of an "abort connection" packet
	,STATION_RTT_ECHO                           // first ushort of a "rtt echo" packet
	,STATION_RTT_REPLY                          // first ushort of a "rtt reply" packet
};

// NEW (ziglite 1.2): Possible states of a station
enum {
	STATION_CLOSED,
	STATION_CONNECTING,
	STATION_CONNECTED,
	STATION_DISCONNECTING
};

// flag smuggled on the "ID ushort" tells if packet is compressed or not
#define STATION_COMPRESSED_PACKET_BIT 0x8000 // 0b1000...00

// flag smuggled on the "ID ushort" tells if additional vport ID data follows
#define STATION_VPORT_BIT             0x4000 // 0b0100...00

// flag smuggled on "message ID" tells if message needed be acked or not (reliable x unreliable)
#define STATION_RELIABLE_BIT          0x8000 // 0b1000...00

#define STATION_DEFAULT_CONNECT_ATTEMPTS (10)
#define STATION_DEFAULT_CONNECT_INTERVAL (0.2)
#define STATION_DEFAULT_CONNECT_TIMEOUT (5.0)		  // Fabio: do not put anything greater than 20.0 here

#define STATION_DEFAULT_ABORT_ATTEMPTS (3)
#define STATION_DEFAULT_ABORT_INTERVAL (0.1)

#define STATION_INFINITY (99999999999999.9)

#define STATION_DELAY_WINDOW (ZIG_PACKET_ID_RANGE / 100)

#define STATION_MAX_RTT_TABLE (50)
#define STATION_DEFAULT_RTT_INTERVAL (4)

//enum: buffer codes for process_incoming_packet()
enum {
	STATION_INCOMING_PACKET_NORMAL   = 0,		// regular game packet (id, acks, reliables....)
	STATION_INCOMING_PACKET_SPECIAL  = 1,		// special packet (id==0 and a custom chunk)
	MAX_STATION_INCOMING_PACKET
};

#ifndef NO_DOXYGEN		// internal class -- do not document

// an outgoing message in the queue
class omsg_c {
	public:
		int      id;           // the message id
		bool     ack_expected; // is the ack of this message being expected?
		buffer_c body;         // the message body
		double   next_send;    // time of next send of this msg (timeout interval)
};

// an incoming message in the queue
class imsg_c {
	public:
		int      id;    // the message id
		bool     dummy; // dummy message flag
		buffer_c body;  // the message body
};

// useful default constants
extern const policy_c DefaultPolicy;
extern const address_c InvalidAddress;
extern const buffer_c EmptyBuffer;

class station_c;

// NEW (ziglite 1.2)
class stream_c {

	// public methods
	public:
		stream_c(int id, int type = STREAM_INDIVIDUAL_ACK, policy_c _policy = DefaultPolicy);

		virtual ~stream_c() { SAFEDELETE(policy); }

		policy_c* get_policy()    { return policy;    }
		int       get_type()      { return type;      }
		double    get_next_send() { return next_send; }

		void set_policy(policy_c _policy) {
			if (policy != NULL) delete policy;
			policy = new policy_c();
			*policy = _policy;
		}
		void   increment_next_send() { if (policy) next_send = get_time() + policy->get_send_interval(); }

		void reset_state();

		// process message that arrived
		// NEW (ziglite 1.2): Field "last reliable msg before this msg"
		void process_incoming_message(NLushort msgid, int msgsize, buffer_c& in);

		// reads next message in the arrival queue. 
		// returns buffer_c with the message and ".code" = 0 if no more messages to read, 1 if success.
		buffer_c get_message();

		const bool test_send(const unsigned int iPacketQuota);

		const bool fill_packet(buffer_c& oPacket, const unsigned int iPacketQuota);

	// protected members
	//protected:

		int m_iStreamId;

		// policy for sending packets
		policy_c* policy;

		// time of next send of a stream
		double next_send;

		// only to cumulative ack streams: flag that says if station must send an ack without any messages in its next incoming flow
		bool send_ack;

		// time when the last incoming packet containing this stream arrived
		double last_incoming_packet_time;

		// time when the last outgoing packet containing this stream left
		double last_outgoing_packet_time;

		// stream type
		int type;

		//ids
		NLushort idgen_reliable_send;       // outgoing reliable messages id generator
		NLushort idgen_reliable_rec;        // incoming reliable messages id generator

		// INCOMING messages queue
		// - fixed size & circular (fastest insert & retrieve, waste memory but memory is cheap)
		// - "sliding window": can only always retrieve the next message in the number sequence ("current")
		// - too many messages: messages start to be ignored until app picks them up
		//   OR: receiving message greater than current+bufcap : discard
		// - old messages (< current) discarded
		// - dup messages : will be discovered when trying to put it into the queue (will already have
		//   been inserted)
		vector<imsg_c> message;
		int queue_size;
		NLushort msg_current;		// current expected message id - 3/march/2004: changed from NLulong to fix a warning

		// NEW (ziglite 1.2): last reliable msg arrived
		// only useful for INDIVIDUAL_ACK and CUMULATIVE_ACK
		// 
		// NEW ZIG2: only useful to STREAM_NO_ACK type
		// As a STREAM_NO_ACK stream has no order guarantee, unreliable messages sent through these streams
		// could not have an associated message ID. So, to simplify code, message IDs are ignored in STREAM_NO_ACK
		// streams, and their messages are inserted in the message buffer in a FIFO fashion.
		// This variable keeps the last message inserted in the message buffer IF the stream is STREAM_NO_ACK
		NLushort last_reliable_msg;

		// messages received that need acks to be sent
		std::vector<NLushort> ackbuf;

		// NEW (ZIGLITE 1.2): cumulative ack
		int cumulative_ack;
		
		// the packet buffer's reliable messages (OUTGOING)
		// also works as the "messages yet to be acked" list
		// since all unacked messages are always sent, this includes those that were not sent a single time yet.
		std::deque<omsg_c> reliable;

		// id of outgoing messages that have been acked by the receiver, and whose acks are being
		// expected by us. FIFO.
		std::deque<int> racks;

		// the owner of this stream
		station_c* st;
};

//GLOBAL (process-wide) defaults for packet loss and packet latency emulation

/* currently not showing on doxygen. use the functions at utils.h to configure

		\brief Global (process-wide) variable which configures the packet loss emulator, in percent chance to drop 
		any outgoing packets.

		Leave at the default value of zero (0) so that no artificial packet loss is created.

		Set a value between 1 and 100 to change the probability, in percent, for any outgoing packets to be dropped 
		(not sent) on purpose by ZIG. For instance, a value of 50 will give a 50 percent chance that any outgoing 
		packet will not get sent.		
*/
extern int zig_global_packet_loss_percent;

/* currently not showing on doxygen. use the functions at utils.h to configure

		\brief Global (process-wide) variable which configures the base (constant) value for the network latency 
		emulator, in milliseconds.

		The value of zig_global_packet_delay_base determines the amount of real time, in milliseconds, that all 
		outgoing packets will be held for before being actually sent.

		For instance, if zig_global_packet_delay_base is set to \c 100, then any packets that are sent, for instance, 
		through zigclient_c::send_packet() or zigserver_c::send_packet(), will be held artificially for 100 milliseconds 
		before the actual socket calls to send UDP packets are performed.

		This "base" delay is added to the delta delay value specified by zig_global_packet_delay_delta (the 
		"variable" component of the global network delay emulator).

		Leave at the default value of zero (0) so that no constant artificial packet delay is caused. 
		
		\see zig_global_packet_delay_delta which adds a variable value of delay to outgoing packets.
*/
extern int zig_global_packet_delay_base;

/*  currently not showing on doxygen. use the functions at utils.h to configure

		\brief Global (process-wide) variable which configures the delta (variable) value for the network latency 
		emulator, in milliseconds.

		The value of zig_global_packet_delay_delta determines that, for any outgoing packet, that it will be artificially 
		delayed for an amount of milliseconds chosen randomly and uniformly from the interval 
		[0, zig_global_packet_delay_delta].

		For instance, if zig_global_packet_delay_delta is set to \c 300, then any packets that are sent, for instance, 
		through zigclient_c::send_packet() or zigserver_c::send_packet(), will be held artificially for a period of time 
		between 0 and 300 milliseconds.

		This "delta" delay is added to the base delay value specified by zig_global_packet_delay_base (the 
		"constant" component of the global network delay emulator).
		
		Leave at the default value of zero (0) so that no variable artificial packet delay is caused.

		\see zig_global_packet_delay_base which adds a constant value of delay to outgoing packets.
*/
extern int zig_global_packet_delay_delta;

//callback interface
class station_callback_i {
public:

	/// Incoming data read by a station from the msocket. it can either be
	/// "special" packets (packet_id == 0) or "regular" game packets
	/// (packet_id > 0 && packet_id <= 14000)
	/// @param pkt: unreliable chunk ("stream 0") of packet
	/// @param packet_id: Unique identifier of arrived packet
	virtual void station_incoming_data(buffer_c& pkt, int packet_id) = 0;

	/// Connection handshake: connection accepted by remote station
	/// @param connection_pkt: Accept packet sent by remote station (it may be
	/// also the request packet, when two stations connecting simultanealy!)
	/// @param forced: true if two endhosts have tried to connect simultaneously and the
	/// connection_pkt parameter is the data of connection request.
	virtual void station_connected(buffer_c& connection_pkt, const bool forced) = 0;

	/// Connection handshake: connection attempt timed out
	virtual void station_connection_timed_out() = 0;

	/// Connection handshake: connection refused by remote station
	/// @param reject_pkt: Reject packet with (potential) rejection reason
	virtual void station_connection_refused(buffer_c& reject_pkt) = 0;

	/// ZIG 2.1.0: Station has disconnected. When this is called, the station 
	/// has given up on the remote peer. 
	/// PARAM: remote: if "remote" is "true", then this callback has been called because a remote 
	///    "disconnect" packet, sent by the remote station, has been received locally.
	///    If "remote" is "false", then the disconnection is local.
	/// PARAM: reason: if "remote" is "true", then it is information from the remote peer that called 
	///    station_c::disconnect() on his own station.
	///    if "remote" is "false", then this is the data that we (local station) sent to the remote 
	///    peer, because station_c::disconnect() was called locally. If "remote" is "false", then the 
	///    buffer might also be empty for other reasons, like an exceptional "death" situation for the 
	///    station. However currently there are no exceptions that cause stations to disconnect other 
	///    than a local or a remote station_c::disconnect() (FIXME/REVIEW this!)
	virtual void station_disconnected(bool remote, buffer_c& reason) = 0;
	
	virtual ~station_callback_i() {}
};

// reliable UDP socket + brains
class station_c {

	friend class msocket_c;

	/// Default constructor
	private: station_c(const address_c& _addr);

	/// Private destructor (app need call msocket_c::delete_station() to remove
	/// station from underlying msocket's map before destruction)
	private: virtual ~station_c();

public:

  static station_c& get_invalid_station() { static station_c oStation(InvalidAddress); return oStation; }

	/// Sets connect data sent with connection packets
	void set_connect_data(buffer_c _connect_data) {
		connect_data = _connect_data;
		con->xprintf(0, "station_c connect_data.size = %i\n");
	}

	/// Sets disconnect data sent with abort packets
	void set_disconnect_data(buffer_c& _disconnect_data) {
		abort_packet = _disconnect_data;
	}

	/// Init connection handshake with remote station
	void connect(double timeout = STATION_DEFAULT_CONNECT_TIMEOUT);

	/// ZIG 2.0: Immediately abort connection with remote station
	void disconnect(const buffer_c& reason = EmptyBuffer);

	//set callback interface
	void set_callback_interface(station_callback_i *beh) { this->beh = beh; }

	//set owner msocket. MUST do this before using this instance
	void set_msocket(msocket_c *msocket);

	//set debug string
	void set_debug(const char *debugstr) { if (strlen(debugstr) < 64) strcpy(dstr, debugstr); }

	//set console
	void set_console(console_c *conio) { con = conio; }

	//NEW (ziglite 1.1.0): configure maximum outgoing packet size caps.
	void set_max_outgoing_packet_size(int _max = ZIG_DEFAULT_MAX_OUTGOING_PACKET_SIZE) {
		max_outgoing_packet_size = _max;
	}

	void set_rtt_protocol_interval(double _interval) {
		rtt_protocol_interval = _interval;
	}

	//NEW (ziglite 1.0.1): configure emulated packet loss on "send packet" operations. 
	// parameter is a probability chance (in interval [0, 1]) of dropping a packet.
	// a value of 0.0 disables the packet loss emulator (default value)
	void set_packet_loss_emulation(double drop_chance = 0.0) { packet_loss = drop_chance; }

	double get_loss_estimative() const;

	double get_delay_estimative() const;

	double get_rtt_estimative() const;

	console_c* get_console() { return con; }

	//NEW (ziglite 1.0.1): configure emulated communication latency on "send packet" operations. 
	// "delay_base" is the minimum added latency in seconds and "delay_delta" is the maximum
	// amount of added latency in seconds over the base value. 
	//how it works: each outgoing packet will have an added (artificial) latency choosen uniformly 
	// from the interval [delay_base, delay_base + delay_delta].
	//a value of 0.0 for both parameters (the default) means no added artificial latency
	void set_packet_latency_emulation(double _delay_base = 0.0, double _delay_delta = 0.0) {
		delay_base = _delay_base;
		delay_delta = _delay_delta;
	}

	// configure usage of packet compression. to turn on the sending of compressed packets at any time, 
	// pass a value >=0 as argument, which specifies the minimum size that the packet must have, in bytes, 
	// to be compressed (pass 0 to compress all outgoing packets). to turn off the sending of compressed 
	// packets at any time, call this method with no parameters (or a negative value as parameter). 
	// NOTE: this call resets the compression stats.
	void set_compression(int minimum_packet_size = -1) { compression_config = minimum_packet_size; }

	// NEW (ziglite 1.2): creates a stream and returns its identifier
	int  create_stream(int type = STREAM_CUMULATIVE_ACK, policy_c policy = DefaultPolicy);

	// NEW (ziglite 1.2): deletes a stream from map
	// returns true if success, false otherwise 
	bool delete_stream(int stream_id = -1);

	// useful methods to "percorrer" all open streams list
	void seek_first_stream();

	// retrieve next stream ID in the map
	// CAUTION! DON'T erase or insert streams between two get_next() calls! (iterators
	int get_next_stream();

	//NEW (ziglite 1.0.1): this is called periodically for the station by the underlying msocket object, which 
	// in turn is called periodically by the upper layers (client_c, server_c, which are ultimately called 
	// by the app)
	//the current uses of station_c::process_nonblocking() are:
	//  - send pending packets that have been delayed because of set_packet_latency_emulation()
	//and that's it for now. if emulation is not being used, don't need to call this method.
	void process_nonblocking();

	// get remote address
	address_c get_remote_address() { return remote_addr; }

	//get the local address
	address_c get_local_address() { return local_addr; }

	// set/change the remote address. returns 0 on failure, 1 on success. notation is "bla.bla.bla.bla:portnum". resets state.
	int set_remote_address(char* address);

	// set/change the remote address. returns 0 on failure, 1 on success. NL address struct. resets state.
	int set_remote_address(NLaddress some_addr);

	// set/change the remote address. returns 0 on failure, 1 on success. resets state.
	int set_remote_address(const address_c &some_addr);

	//change the port of the remote address. resets state (optional, default)
	int set_remote_port(NLushort port, bool do_not_reset_state = false);

	//get the port of the remote address
	NLushort get_remote_port();

	//get the port of the local address
	NLushort get_local_port();
	
	// NEW (ZIGLITE 1.1): get the virtual port (vport) of this station (this parameter cannot be 
	// changed after the station_c object is created)
	int get_vport() const { return remote_addr.get_virtual_port(); }

	// NEW: polling multiple stations support: 
	// get this station's internal socket object. currently you will need it you want to poll several 
	// stations for read/write status. 
	msocket_c* get_socket();

	bool set_default_stream_id(int stream_id) {

		std::map<int, stream_c*>::iterator it = streams.find( stream_id );
		if ( it == streams.end() ) return false;

		default_stream_id = stream_id;
		return true;
	}

	int get_default_stream_id() const { return default_stream_id; }

	// incoming packet from msocket
	// NOTE/HACK: in and out buffers are all shared buffers inside the msocket instance.
	void incoming_msocket_packet();

	// process incoming packet. returns unreliable data portion of packet. also returns in the ".code"
	// field of the buffer_c returned, the type of packet that arrived. possible types:
	// STATION_INCOMING_PACKET_NORMAL: a regular game packet
	// STATION_INCOMING_PACKET_SPECIAL: a special packet (sent with send_raw_packet())
	// "packet_id_ptr" is an optional pointer to an int which will receive the id of the incoming packet.
	buffer_c process_incoming_packet(buffer_c &packet, int *packet_id_ptr = 0);

	// reads next message of the stream 
	// returns 0 if no more messages to read, 1 if success.
	// NEW ZIGLITE 1.2: streams	
	int read(buffer_c &msg, int stream_id = -1);

	// reads next message of the stream 
	// returns buffer_c with the message and ".code" = 0 if no more messages to read, 1 if success.
	buffer_c read(int stream_id = -1);

	// appends reliable message to the queue. this will be sent as many times as needed until
	// it's acknowledged by the other side.
	//
	// msg_id: if you want to be notified when this message is ACK'ed by the receiver, pass a valid int
	// pointer via msg_id. the pointer will be filled with the ID of the outgoing message, that can be
	// used to identify the ack when it comes through read_ack()
	//
	// NEW ZIGLITE 1.2: streams
	int write(buffer_c &msg, int stream_id = -1, bool realiable = true, int* msg_id = NULL);
	int write_reliable(buffer_c &msg, int stream_id = -1, int* msg_id = NULL);
	int write_unreliable(buffer_c &msg, int stream_id = 0);

	// read a pending reliable message ack. does not block. returns -1 if no acks to be read,
	// else returns the id of the message that was acked (same "msg_id" of write_reliable())
	// call this after a packet arrives (that's the only time the acks are updated)
	//
	// NEW ZIGLITE 1.2: streams
	int read_ack(int steam_id = -1);

	// checks whether a send_packet() call would dispatch a "dummy packet", that is, 
	// a packet without any useful payload (reliable message acks, reliable message 
	// retransmissions or unreliable data block). 
	// returns TRUE if there is nothing to send (in which case, following with a 
	// send_packet() call would be wasteful, unless it was intended as a keepalive or 
	// something like that), FALSE otherwise.
	bool nothing_to_send();

	// flush the packet buffers as an UDP packet to the remote address, returns "id"
	// for the assigned packet id. this call resets the unreliable data buffer (see write() above).
	// POSSIBLE RETURN CODES: 
	//   SENDPACKET_OK                     0    // no error
	//   SENDPACKET_SOCKET_ERROR          -1    // when nlWrite() returns NL_INVALID
	int send_packet(int *id);

	// send a raw UDP packet to the destination. use when you want to send a message to the other
	// side without the "RUDP protocol" overhead. UDP packets with the first short == 0 are "special"
	// and not treated as rudp packets (see process_incoming_packet()). returns result of nlWrite()
	int send_raw_packet(buffer_c &udp);

	// called by msocket when a connection is accepted
	/// @param accept_pkt: your local connection ID replied by remote station + 
	/// remote connection ID + custom connection data sent in accept packet
	void incoming_connection_accept(buffer_c& accept_pkt);

	// called by msocket when a connection is rejected
	/// @param reject_pkt: connection ID replied by remote station + 
	/// custom rejection data sent in reject packet
	void incoming_connection_reject(buffer_c& reject_pkt);

	/// Called by msocket when this station has to send
	/// "connection accepted" packets
	/// @param _connection_id: Remote connection identifier
	/// @param answer: buffer with custom response to remote connect request
	void accept_connection(NLubyte _connection_id, buffer_c& answer, const buffer_c& data);

	/// Called by msocket when abort connection packet arrives.
	/// This method close the station and also inform application through
	/// "station_connection_aborted" callback
	/// @param abort_pkt: buffer with custom reason from abort
	void incoming_connection_abort(buffer_c& abort_pkt);

	/// @return true if station is connecting with a remote station, false otherwise
	bool is_connecting() { return (station_state == STATION_CONNECTING); }

	/// @return true if station is connected with a remote station, false otherwise
	bool is_connected() { return (station_state == STATION_CONNECTED); }

	/// @return true if station is disconnecting from a remote station, false otherwise
	bool is_disconnecting() { return (station_state == STATION_DISCONNECTING); }

	/// @return true if station is closed, false otherwise
	bool is_closed() { return (station_state == STATION_CLOSED); }

	//debug console
	console_c	*con;

protected:

	//reset state
	void reset_state();

	// emulate send latency & packet loss. This method also append the packet on msocket delaylist.
	// return number of bytes written in system socket (mesmo se pacote delayed)
	// If packet was lost, return 0.
	int emulate_latency_and_loss(buffer_c packet, address_c remote);

	// retrieve a stream instance from map	
	stream_c* search_stream(int stream_id = -1);

	bool update_loss_info(NLushort packet_id);

	void update_delay_info(NLushort packet_id);

	void update_rtt_info(const buffer_c& packet);

//---- MEMBER VARS -------------------------------------------------------

	//callback interface
	station_callback_i *beh;

	//msocket instance that owns this station
	msocket_c *msocket;

	//debug string (id)
	char dstr[64];

	// ZIGLITE
	//buffer pointers - points to the shared buffers at the owner mstation_c
	buffer_c* udp_read_buf;
	buffer_c* sendbuf;
	buffer_c* sendbuf_zipped_b;

	//addresses of the socket
	address_c local_addr;
	address_c remote_addr;

	// packet loss: between [0,1]
	double packet_loss;

	// minimum delay for send packets: in seconds
	double delay_base;

	// delta interval for random delay: in seconds
	double delay_delta;

	// minimum packet size for packet compression
	int    compression_config;

	// max absolute packet size
	int    max_outgoing_packet_size;

	int base_packet_id, last_packet_id;
	int base_o_packet_id, last_o_packet_id;		//outgoing

	// buffer of unreliable data
	buffer_c unrelbuf;

	//ZIGLITE 1.1: connect() in station support
	int      station_state;             // closed, connecting, etc

	buffer_c connect_data;              // data sent in connection packets
	double   connect_timeout;           // time when connection is finished
	double   connect_interval;          // time between two connect attempts
	int      connect_attempts;          // total number of connection packets
	double   connect_packet_next_time;  // time for next packet
	int      connect_packet_left_count; // packets left

	buffer_c accept_packet;             // data sent in accept packets

	buffer_c abort_packet;                 // data sent in abort packets
	double   abort_interval;               // time between two disconnect attempts
	int      abort_attempts;               // total number of disconnection packets

	//ZIGLITE 1.1: connection IDs
	NLubyte    local_connection_id;        // useful for drop old packets
	NLubyte    remote_connection_id;       // (random id, NOT 100% secure)

	//ids
	NLushort idgen_packet_send;            // outgoing packet id generator

	//ZIGLITE 1.2: streams map. Each stream has an associated policy for sending & receiving packets
	std::map<int, stream_c*> streams;

	// ZIGLITE 1.2: default stream ID (useful for minimize methods parameters)
	int default_stream_id;

	// NEW (ziglite 1.2): iterator for current stream (list all streams)
	std::map<int, stream_c*>::iterator current_stream;

	// NEW: packet loss utility
	private: NLushort max_incoming_packet_id;

	private: bool received_packet_ids[ZIG_PACKET_ID_RANGE];

	private: unsigned long total_packets;

	private: unsigned long total_received_packets;

	// NEw: packet delay utility
	private: NLushort delay_base_id;

	private: NLushort delay_last_id;

	private: NLushort delay_packets[ZIG_PACKET_ID_RANGE];

	// NEW: RTT estimative utility
	private: int last_rtt_estimative;
	private: int first_rtt_estimative;
	private: double rtt_protocol_interval;
	private: double last_rtt_sent;
	private: double rtt_table[STATION_MAX_RTT_TABLE];
};

#endif // #ifndef NO_DOXYGEN		// internal class -- do not document

#endif

