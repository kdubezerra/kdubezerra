#define _POSIX_C_SOURCE 199309
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#include <iostream>
#include <string>
using namespace std;

#include <zig/zig.h>

#include "peer.h"
#include "fake_console.h"

#define DEFAULT_PORT 1024

bool exit_flag = false;
console_c* con = NULL;
sem_t sem;

string next_token(string& s) {

	int index = s.find_first_of(" :");
	string result = s.substr(0, index);
	s.erase(0, index);
	index = s.find_first_not_of(" ");
	s.erase(0, index);
	return result;
}

void* read_keyboard(void* args) {

	Peer* peer = static_cast<Peer*>(args);

	string s;
	while (!exit_flag) {

		getline(cin, s);
		string cmd = next_token(s);

		if (cmd.compare("quit") == 0) {
			exit_flag = true;
		}
		else if (cmd.compare("/c") == 0) {

			address_c remote_addr; 
			remote_addr.set_address("localhost");	

			if (s.length() == 0) {
				remote_addr.set_port(DEFAULT_PORT);
			}
			else {
				remote_addr.set_port(atoi(s.c_str()));
			}

			con->printf("connecting with %d ...\n", remote_addr.get_port());
			peer->connect(remote_addr);
		}
		else if (cmd.compare("/cs") == 0) {

			int type = STREAM_NO_ACK;
			if (s.length() > 0) {
				string token = next_token(s);

				if (token.compare("cum") == 0) type = STREAM_CUMULATIVE_ACK;
				else if (token.compare("ind") == 0) type = STREAM_INDIVIDUAL_ACK;
				else if (token.compare("no") == 0) type = STREAM_NO_ACK;
				else continue;
			}

			bool discard = false;
			if (s.length() > 0) {
				string t = next_token(s);
con->printf(", %s", t.c_str());
			if (t.compare("disc") == 0) discard = true;
			}

			int flow = NULL_FLOW;
			if (s.length() > 0) {
				string t = next_token(s);
con->printf(", %s\n", t.c_str());
				if (t.compare("uni") == 0) { con->printf("uni\n"); flow = UNIFORM_FLOW;}
				else if (t.compare("gau") == 0) flow = GAUSSIAN_FLOW;
				else if (t.compare("null") == 0) flow = NULL_FLOW;
			}

			double messages_interval = 0.01;
			if (s.length() > 0) {
				messages_interval = (double) atof(next_token(s).c_str());
			}

			double send_interval = 0.1;
			if (s.length() > 0) {
				send_interval = (double) atof(next_token(s).c_str());
			}

			double resend_interval = 0.2;
			if (s.length() > 0) {
				resend_interval = (double) atof(next_token(s).c_str());
			}

			int max_outgoing_size = 1024;
			if (s.length() > 0) {
				max_outgoing_size = atoi(next_token(s).c_str());
			}

			policy_c policy(send_interval, resend_interval, max_outgoing_size);

			sem_wait(&sem);
			peer->create_stream(type, flow, messages_interval, policy);
			sem_post(&sem);
		}
	}

	return NULL;
}


int main(int argc, char** argv) {

	con = new fake_console_c(&cerr);
	con->set_xprintf_level(0);
	zig_init(con);

	sem_init(&sem, 0, 1);
	Peer* peer = new Peer(con);
	pthread_t reader;
	pthread_create(&reader, NULL, read_keyboard, peer);

	struct timespec sleep_time;
	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = 500 * (1000000);

	while (!exit_flag) {

		sem_wait(&sem);
		peer->process_nonblocking();
		sem_post(&sem);

		nanosleep(&sleep_time, NULL);
	}

	return 0;
}
