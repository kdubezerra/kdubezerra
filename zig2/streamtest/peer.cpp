#include <stdlib.h>
#include <math.h>

#include <iostream>
using std::iostream;

#include "peer.h"

int cfg_max_packet_size = 65535;
int cfg_min_port = 1024;
int cfg_max_port = 65535;

Peer::Peer(console_c* con) {

	this->con = con;
	st = NULL;
	peer_connected = false;
	last_send = get_time();

	sock.set_callback_interface( this );
	sock.set_console(con);
	if (!sock.open_socket(cfg_min_port, cfg_max_port)) {
		con->printf("error while open udp socket port in [%d, %d] range. quitting program...", cfg_min_port, cfg_max_port);
		exit(0);
	}
}

Peer::~Peer() {
	sock.close_socket();
	streams.clear();
}

bool Peer::create_stream(int type, int flow_type, double messages_interval, policy_c policy) {

	if (!peer_connected) return false;

	int id = st->create_stream(type, policy);

	LoadInjector* li = (LoadInjector*) malloc(sizeof(LoadInjector));
	li->flow_type = flow_type;
	li->messages_interval = messages_interval;
	
	if (flow_type == NULL_FLOW) li->next_send = -1.0;
	else li->next_send = get_time();

	
	streams[id] = (*li);

	return id;
}

void Peer::msocket_read_error(NLsocket &socket) {

	con->printf("msocket_read_error callback. quitting program...\n");
	exit(0);
}

void Peer::msocket_station_connected(station_c& station, bool forced_accept, buffer_c& answer) {

	this->st = &station;

	st->set_callback_interface( this ); 

	st->set_max_outgoing_packet_size( cfg_max_packet_size );

	st->set_console(con);

	st->set_debug("");

	peer_connected = true;

	con->xprintf(1, "msocket_station_connected callback. stations connected.\n");
}

void Peer::msocket_closed() {

	con->xprintf(1, "msocket_closed callback.\n");
	peer_connected = false;
	streams.clear();
}

void Peer::station_incoming_data(buffer_c& packet, int packet_id) {

	con->xprintf(1, "Incoming packet ID = %d\n", packet_id);
	// TODO: count incoming/missing packets, latency, etc
}

void Peer::station_connected(buffer_c& connection_packet) {
	con->xprintf(1, "station connected.\n");
	peer_connected = true;
}

void Peer::station_connection_timed_out() {

	con->xprintf(1, "station connection timed out. closing station\n");

	address_c addr = st->get_remote_address();
	sock.delete_station(addr);
	st = NULL;
	peer_connected = false;

	streams.clear();
}

void Peer::station_connection_refused(buffer_c& reject_reason) {}

void Peer::station_disconnected(bool remote, buffer_c& abort_reason) {

	con->xprintf(1, "station connection timed out. closing station\n");

	address_c addr = st->get_remote_address();
	sock.delete_station(addr);
	st = NULL;
	peer_connected = false;;

	streams.clear();
}

double exp_generator(double inv_lambda) {

	double uniform = ((double) (rand() % 1000000000)) / 1000000000.0;
	return -1.0 * (log(uniform) * inv_lambda);
}


void Peer::process_nonblocking() {

	sock.process_nonblocking();

	double now = 0.0;

	map<int, LoadInjector>::iterator it = streams.begin();
	while (it != streams.end()) {

		LoadInjector li = it->second;
		if (li.flow_type == NULL_FLOW) { 
			con->xprintf(0, "touch, next send = %lf\n", li.next_send);
		}
		else {
			now = get_time();
			if (now > li.next_send) {

				con->xprintf(0, "I will send message (now = %lf)\n", now);
	
				buffer_c data;
/*
				char* garbage = (char*) malloc(100);
				data.putBlock(garbage, 100);
				free(garbage);
*/
				int msgid;
				st->write(data, it->first, true, &msgid);
				
				if (li.flow_type == UNIFORM_FLOW) {
					(it->second).next_send = now + li.messages_interval;
				}
				else if (li.flow_type == GAUSSIAN_FLOW) {
					(it->second).next_send = now + exp_generator((double) li.messages_interval);
					
				}

				con->xprintf(1, "sended message %d\n", msgid);

			}
		}

		it++;
	}

	now = get_time();
	if ( peer_connected && (now > (last_send + 0.05)) ) {

		if ((st->nothing_to_send()) == false) {

			int packet_id;
			st->send_packet(&packet_id);
			con->printf("sending packet %d (now = %lf)\n", packet_id, now);

			last_send = now;
		}
	}
}

bool Peer::connect(address_c remote_addr) {

	if (peer_connected) return false;

	st = sock.create_station(remote_addr);	

	st->set_callback_interface( this ); 

	st->set_max_outgoing_packet_size( cfg_max_packet_size );

	st->set_console(con);

	st->set_debug("");

	st->connect(2.0);

	return true;
}
