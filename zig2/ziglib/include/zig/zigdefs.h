/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, F�bio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*! \file zigdefs.h
    \brief Provides internal library compilation switches that controls some aspects of the ZIG implementation.

		Please look at the original <zig/zigdefs.h> file on the library source code, if you are really interested in 
		changing these options. Look out especially for the "TWEAK OPTION:" defines.

		A special define that should be looked into is <B><TT>ZIG_NO_BZIP2</TT></B>, which removes BZip2 compression 
		code from the library. Some people argue it should be compiled separate but methinks that it is more 
		noob-friendly to include a good default compression library -- and by "default" it is not implied that ZIG 
		can currently link to and make use of multiple compression libraries or anything... but, you know, in the 
		future...
*/

#ifndef _ZIG_HEADER_ZIGDEFS_H_
#define _ZIG_HEADER_ZIGDEFS_H_

//#ifndef NO_DOXYGEN // if defines are commented out, it funks up the doxygen output

// TWEAK OPTION: define this and recompile the lib to disable support for packet compression. this 
// reduces the lib size a bit if you're not using compression
//#define ZIG_NO_BZIP2

// TWEAK OPTION: enable the switch below to use the OLD (pre-v1.5.0) way of generating client IDs 
// for new connected clients. By the old method, if a client with ID=0 disconnects and a new 
// client just after that quickly connects, the new client will also get ID=0: the value of 
// client IDs tends to be small even after many clients connect and disconnect. When this 
// switch is NOT defined (the ZIGlib default), then new players connecting will always get 
// the "next" possible ID in the full ID range, until it reaches the hardcoded client limit 
// (64K) and goes back to 0 (but still checking so that no two simultaneously connected 
// clients will ever have the same ID).
//#define ZIG_USE_COMPACT_CLIENT_ID_GENERATOR

// TWEAK OPTION: a magic number agreed between client and server. MUST BE A DOUBLE
#define ZIGNET_IMPLICIT_PING_MS 3200.0

// allocate maximum buffer size for UDP read/write and packet zip/unzip functionality. 
// since now we have only one socket per machine, this is no longer wasteful.
#define	ZIG_UDP_BUFFER_SIZE	65536

// maximum number of simultaneously opened log files (by <zig/log.h>)
#define	ZIG_MAX_LOG_FILES	32

// default network tick of zigclient_c. this parameter doesn't matter much because it is overriden with the value 
// of the zigserver's network tick just after the client connects to it
#define ZIGCLIENT_DEFAULT_TICK 0.1

// default value for maximum outgoing packet sizes. change this if you want a 
// quick & dirty global switch for this config. however, using zigclient::set_max_outgoing_packet_size() 
// and zigserver::set_max_outgoing_packet_size() and zigserver::set_max_outgoing_packet_size_for_client() 
// is preferred.
#define ZIG_DEFAULT_MAX_OUTGOING_PACKET_SIZE 65536

// ZIGNET version unsigned byte
//version 8: zig 2
//version 7: ziglite 1.1.0 (support for msocket_c/station_c vports)
//version 6: ziglite 1.1.0 (nack/nrel up to 2K, 1 to 3 bytes)
//version 5: ziglite 1.0.0
//version 4: zig 1.5.0 (all acks and message IDs sent as unsigned shorts -- no more byte-sized deltas)
//version 3: zig 1.3.0 (support for reliable messages larger than 255 bytes)
//version 2: zig 1.1 (compression support) 
//version 1: zig 1.0.x
#define ZIG_NET_VERSION 8

// ZIGNET SERVER connect rejection codes
enum {

	ZIGNET_REJECT_VERSION = 1,	//zig rejected you by version mismatch, next byte is the server's version
	ZIGNET_REJECT_GAMESERVER		//zig accepted you but the gameserver rejected you, follows custom binary
															//reason block from your gameserver implementation.
};

//#endif // #ifndef NO_DOXYGEN // if defines are commented out, it funks up the doxygen output

// ---- Doxygen initial docs page ----

/*! \mainpage ZIG Game Networking Library
 *
 * This is the documentation for the ZIG library generated by doxygen.
 *
 * All versions of ZIG tagged "2.x" belong to a new branch of the library named "ZIG2", which has as its main 
 * feature the support for separate ordering streams of reliable messages inside of the same connection. Also, 
 * each stream can configure itself to use a different algorithm for message acknowledgement and resending and 
 * configure parameters specific to each algorithm. The Streams feature replaces the need for "TCP streams" which 
 * were offered by the ZIG 1.x branch. The Streams feature was implemented by Diego Midon Pereira.
 *
 * This ZIG2 branch makes all the older branches (ZIG 1.x, ZIGLITE, ZIGLITE/zig-exp(cvs)) obsolete and should 
 * definately be used by all new projects.
 *
 * ZIG2 depends on <a href="http://www.hawksoft.com/hawknl/">HawkNL version 1.7 beta 1</a> or later.
 *
 * ZIG2, as the old ZIGLITE branch, supports running clients and servers with only one UDP socket opened, which 
 * helps to better support game clients and game servers behind NATs and firewalls). Please see the README.TXT 
 * files and CHANGES.TXT for more details.
 *
 * These docs cover documentation of the public header files, classes and methods of ZIG2. For more information 
 * regarding the library, please check the TXT files bundled with your ZIG2 release (README.TXT, CHANGES.TXT, etc).
 *
 * Any questions can be directed to the <a href="http://sourceforge.net/mail/?group_id=61409">zige-users mailing list</a>
 * or to Fabio Cecin (fcecin@gmail.com OR fcecin@inf.ufrgs.br)
 */

#endif

