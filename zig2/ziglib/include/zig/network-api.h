/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2007, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/

#ifndef _ZIG_HEADER_NETWORK_API_H_
#define _ZIG_HEADER_NETWORK_API_H_

int zig_open_udp_socket(unsigned short port);

int zig_close_socket(int sock);

int zig_set_blocking_io(int sock, int value);

int zig_send_packet(int sock, const void* data, unsigned int length, const char* target);

int zig_receive_packet(int sock, void* data, unsigned int max_data_length,
                       char* source, unsigned int max_source_length);

#endif

