#ifndef _ZIG_STREAMTEST_PEER_H_
#define _ZIG_STREAMTEST_PEER_H_

#include <list>

#include <zig/station.h>
#include <zig/msocket.h>

#define NULL_FLOW 0
#define UNIFORM_FLOW 1
#define GAUSSIAN_FLOW 2

extern int cfg_max_packet_size;
extern int cfg_min_port;
extern int cfg_max_port;

typedef struct {
	int flow_type;
	double messages_interval;
	double next_send;
} LoadInjector;

class Peer: public msocket_callback_i, public station_callback_i {

public:

	Peer(console_c* con);

	~Peer();

	void process_nonblocking();

	bool connect(address_c remote_addr);

	bool create_stream(int type, int flow, double messages_interval, policy_c policy);

	virtual void msocket_incoming_unreg(address_c& address, const buffer_c& packet) {}

	virtual void msocket_read_error(NLsocket &socket);

	virtual bool msocket_accept_station(address_c &address, const buffer_c& connection_data, buffer_c& answer) { return true; }

	virtual void msocket_station_connected(station_c &station, bool forced_accept, buffer_c& answer);

	virtual void msocket_closed();
	
	virtual void station_incoming_data(buffer_c& packet, int packet_id);

	virtual void station_connected(buffer_c& connection_packet);

	virtual void station_connection_timed_out();

	virtual void station_connection_refused(buffer_c& reject_reason);

	virtual void station_disconnected(bool remote, buffer_c& abort_reason);

	virtual bool station_stream_request(int id, int type, policy_c policy) {
		create_stream(type, NULL_FLOW, 0.0, policy);
		return true;
	}

	virtual void station_stream_accepted(int id) {}

	virtual void station_stream_rejected(int id, const buffer_c& reject_reason) {}

protected:

	bool peer_connected;

	console_c* con;

	station_c* st;

	msocket_c sock;

	double last_send;

	std::map<int, LoadInjector> streams;
};

#endif

