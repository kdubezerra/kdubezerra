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

*/

#ifndef _ZIG_HEADER_LEETNET_H_
#define _ZIG_HEADER_LEETNET_H_

#ifndef NO_DOXYGEN		// internal class -- do not document

//stoopid #include <map> warningz
#if (defined _MSC_VER)
#pragma warning (disable: 4786)
#endif

//byte commands in/out of the server's request port
// ZIGLITE NEW:
//   LEET_* messages removed: now they all belong to ushort 0 (special packet) + byte STC / CTS msgs

//byte commands in/out of server and client's LAN broadcast port
enum {
	LEET_BCCOMMAND_SERVERINFO = 1,
	LEET_BCRESULT_SERVERINFO
};

// STC_* and CTS_* are unsigned byte commands encoded in a game packet that starts 
// with an unsigned short "packet id" equal to 0 ("special packet"):
//  USHORT 0 followed by UBYTE (STC or CTS code) followed by (..rest of message..)

//special byte commands to the client
enum {

	STC_DISCONNECT = 1,				//server tells client that he must disconnect now
	STC_SERVERINFO						//server gives server-info
};

//special byte commands to the server
enum {

	CTS_DISCONNECT = 1,			//client tells server that he is leaving
	CTS_SERVERINFO					//client wants server-info
};

#endif //#ifndef NO_DOXYGEN		// internal class -- do not document

#endif

