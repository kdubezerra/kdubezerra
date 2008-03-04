/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*

	ZIG global data & functionality

*/

#include "zig/zigdefs.h"

#include "zig/zig.h"
#include <nl.h>

//initialization of the library, call this before using other stuff
//just starts HawkNL (network init, select IP network)
bool zig_init(console_c *con) {

	// init HawkNL

	if (nlInit() == NL_FALSE) {
		if (con) con->xprintf(4,"ERROR: zig_init(): cannot nlInit()\n");
		return false;
	}
	if (nlSelectNetwork(NL_IP) == NL_FALSE) {
		if (con) con->xprintf(4,"ERROR: zig_init(): cannot nlSelectNetwork(NL_IP)\n");
		return false;
	}

	return true;
}

//shutdown of the library, call this when exiting your app
//just shuts down HawkNL
void zig_shutdown(console_c *con) {

	nlShutdown();
}


