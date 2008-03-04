/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/

#include "zig/network-api.h"
#include <nl.h>

int zig_open_udp_socket(unsigned short port)
{
	return (int) nlOpen(port, NL_UNRELIABLE);
}

int zig_close_socket(int sock)
{
	return (nlClose((NLsocket) sock) == NL_TRUE ? 1 : 0);
}

int zig_set_blocking_io(int sock, int value)
{
	NLboolean boolean = (value == 0 ? NL_FALSE : NL_TRUE);
	return (nlSetSocketOpt((NLsocket) sock, NL_BLOCKING_IO, boolean) == NL_TRUE ? 1 : 0);
}

int zig_send_packet(int sock, const void* data, unsigned int length, const char* target)
{
	NLaddress addr;

	if ( (data == NULL) || (target == NULL) ) return -1;
	if (nlStringToAddr(target, &addr) == NL_FALSE) return -1;

	if (nlSetRemoteAddr((NLsocket) sock, &addr) == NL_FALSE) return -1;
	return (int) nlWrite((NLsocket) sock, data, length);
}

int zig_receive_packet(int sock, void* data, unsigned int max_data_length,
                       char* source, unsigned int max_source_length)
{
	int result = 0;
	NLaddress addr;

	if ( (data == NULL) || (source == NULL) ) return -1;
	if (max_source_length < 32) return -1;

	result = (int) nlRead((NLsocket) sock, data, max_data_length);
	if (result <= 0) return result;

	if (nlGetRemoteAddr((NLsocket) sock, &addr) == NL_FALSE)
	{
		source[0] = '\0';
	}
	else if (nlGetNameFromAddr(&addr, source) == NULL)
	{
		source[0] = '\0';
	}

	return result;
}
