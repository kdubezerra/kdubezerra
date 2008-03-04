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

	utils

*/


#ifndef _ZIG_HEADER_UTILS_H_
#define _ZIG_HEADER_UTILS_H_

#include <nl.h>
#include <assert.h>
#include "log.h"

/*! \file utils.h
    \brief Offers misc utility functions/macros/etc. The main features are:

		GET CURRENT TIME (HIGH PRECISION): get_time() - You will be using this a LOT...

		NETWORK PACKET LOSS AND LATENCY EMULATION:
		- zig_emulate_network_packet_loss() - Packet loss
		- zig_emulate_network_latency() - Network lag

		PACKET COMPRESSION: 
		- zig_set_compression_level() - Configure the 'aggressivity' of the BZip2 compression algorithm
		- zig_avg_compression_gain() - Statistics
		- zig_reset_compression_stats() - Statistics

		NETWORK USAGE STATISTICS:
		- zig_avg_total()
		- zig_avg_out()
		- zig_avg_in()

		RANDOM NAME GENERATION: RandomName()
*/

/*! \brief Asserts that argument \c X is 0 (NULL) before conditionally ('if (X != 0)') attempting to delete and 
		nullify it. 
*/
#define SAFEDELETE(X) { /*assert(X != 0);*/ if (X) { delete X; X = 0; } }

/*! \brief Makes the program/debugger halt by assert(false) and throwing a random int value exception. */
#define FAILSTOP() { assert(false); throw 666; }

//#define LOG_NL_ERRORS(LEV) { con->xprintf(LEV, "HawkNL errors: %i %s %i %s\n", nlGetError(), nlGetErrorStr(nlGetError()), nlGetSystemError(), nlGetSystemErrorStr(nlGetSystemError())); }

#include <string>
//using namespace std;

/*! \brief Makes the current thread sleep (free the CPU) for the specified amount of milliseconds. 

		\param X Amount of time to sleep, in milliseconds.
*/
#if defined WINDOWS || WIN32 || WIN64
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#define SLEEP(X) Sleep(X)
#else
#include <time.h> //you should have the nanosleep() function available
#define SLEEP(X) { timespec ts1,ts2; ts1.tv_sec = (X)/1000; ts1.tv_nsec = ((X)%1000)*1000000; nanosleep(&ts1,&ts2); }
#endif

/*! \brief Returns the current (real) time, in seconds.

		<B>IMPORTANT</B>: There is no guarantee about the time counter being reset to zero when the process starts, 
		nor there is no guarantee that the value of the counter is relative to some absolute date (e.g.: UNIX time). 

		<B>This function should be used solely to measure time differences.</B> 
		
		IMPLEMENTATION NOTE: On Windows, this function is implemented using the 'performance counter' Windows APIs. 
		On other systems, it uses the portable <B>nlTime()</B> HawkNL API (which on Windows probably compiles to 
		the 'performance counter' APIs as well... so... whatever...)

		<B>EXAMPLE</B> - Obtaining a real-time difference with get_time():
		<PRE>
    double earlier = get_time();
    SLEEP(2500); // waits for 2.5 seconds of real-time ('wall-clock' time)
    double later = get_time();
    double delta = later - earlier; // delta should be "2.5" or something very close to that </PRE>
		
		\return Current wall-clock (real) time, in seconds, which is relative to some other undetermined value or event.
*/
inline double get_time() {
#ifndef WIN32
	NLtime ts;
	nlTime(&ts);
	return (((double)ts.seconds)+(((double)ts.useconds)/1000000.0));
#else
  static bool bInit=false;
  static LARGE_INTEGER stFrequency;
  LARGE_INTEGER stTime;
  if (!bInit) {
    QueryPerformanceFrequency(&stFrequency);
    bInit=true;
  }
  QueryPerformanceCounter(&stTime);
  return (stTime.QuadPart/(double)stFrequency.QuadPart);
#endif
}

/*! \brief Gets a random name. Useful for naming bots or giving default nicknames to unnamed players.

		This is the fantastic, legendary, (in)famous <B>RandomName<sup>TM</sup></B> routine!!! (!!! insert fanfare 
		sounds here !!!) created by Renato Hentschke :-)

		Some of the marvelous names that this function can generate:
		- <TT>Whafo Cepe Nixa</TT>
		- <TT>Doko Jo</TT>
		- <TT>Feri Rise</TT>
		- <TT>A</TT>

		\param npal Number of words that the name should have (optional). If left at default value (-1), will generate 
		names with a random number of words, between 1 and 3.

		\return A string that contains the generated random name.
*/
std::string RandomName(int npal=-1);

// packet loss & network latency emulator
// -------------------------------------------------------------------------------

/*! \brief Enables or disables the global (process-wide) network packet loss emulator. Default is <b>disabled</b>!

		Call this method passing, as parameter, a value between 1 and 100, to change the probability, in percent, for 
		any outgoing packets to be dropped (not sent) on purpose by ZIG. For instance, a value of 50 will give a 50 
		percent chance that any outgoing packet will not get sent.

		By default, there is a zero (0) percent chance that packets get dropped on purpose by ZIG, which means that 
		the network packet loss emulator is always disabled by default unless you explicitly enable it.

		If an invalid parameter is passed to this function, the network packet loss emulation will be disabled.

		\param drop_chance_percent Percent chance, between 0 and 100, of any outgoing packet being dropped on 
		purpose by ZIG.
*/
void zig_emulate_network_packet_loss(int drop_chance_percent);

/*! \brief Enables or disables the global (process-wide) network latency emulator. Default is <b>disabled</b>!

		Call this method passing the minimum and maximum amount of artificial delay, in milliseconds, that ZIG should 
		impose on all outgoing packets. 

		For instance, the following call...
		<PRE>
		zig_emulate_network_latency(120, 350) </PRE>
		
		...will cause any outgoing packet to be artificially delayed for an amount of milliseconds chosen randomly 
		and uniformly from the interval [120, 350]. In this example, any given outgoing packet might get delayed by 
		120ms, or 350ms, or 200ms... any amount of time between 120ms and 350ms.

		This works for any and all outgoing packets generated by ZIG, including the ones sent by zigclient_c::send_packet() 
		and zigserver_c::send_packet(). 

		By default, the minimum and maximum delay values are both set to zero (0), which means that the network delay 
		emulator is always disabled by default unless you explicitly enable it.

		If an invalid set of parameters is passed to this function, the network latency emulation will be disabled.

		\param min_delay_ms Minimum amount of delay, in milliseconds, that is to be added to all outgoing packets.
		\param max_delay_ms Maximum amount of delay, in milliseconds, that is to be added to all outgoing packets.
*/
void zig_emulate_network_latency(int min_delay_ms, int max_delay_ms);

// socket statistics -- a bunch of NO-OP name wrappers for the HawkNL functions
// -------------------------------------------------------------------------------

/*! \brief Gets the current average bandwidth (bytes per second) being used by all sockets of this process, for 
		both incoming and outgoing traffic.

		\return Average network traffic (bytes per second) passing through the sockets of this process.
*/ 
int zig_avg_total();

/*! \brief Gets the current average bandwidth (bytes per second) being used by all sockets of this process, for 
		outgoing traffic only.

		\return Average network traffic (bytes per second) being sent by the sockets of this process.
*/ 
int zig_avg_out();

/*! \brief Gets the current average bandwidth (bytes per second) being used by all sockets of this process, for 
		incoming traffic only.

		\return Average network traffic (bytes per second) being received by the sockets of this process.
*/ 
int zig_avg_in();

// compression statistics & setup
// -------------------------------------------------------------------------------

/*! \brief Sets the global level of compression for all BZip2 packet compressions performed by this process.

		The default compression level is 1.

		\param level The desired level of compression, between 1 (fastest compression) and 9 (best compression).
*/ 
void zig_set_compression_level(int level);

/*! \brief Gets the current gain, in percent, being obtained by compression of both incoming and outgoing 
		UDP packets.

		This average is calculated as follows. For each outgoing or incoming packet:
		- Add the compressed size, in bytes, to a 'compressed' accumulator;
		- Add the uncompressed size, in bytes, to an 'uncompressed' accumulator;

		If the particular packet is not compressed at all, both accumulators will increase by the same value.

		The average gain is obtained by the formula:
		
		<PRE>
    ((uncompressed - compressed) / uncompressed) * 100.0 </PRE>

		HINT: Calling zig_reset_compression_stats() will reset both byte counters to zero.

    \return Average compression gain, in percent value between \c 0.0 and \c 100.0.
*/
double zig_avg_compression_gain();

/*! \brief Resets the compression gain counters.

		This method is useful if you have changed compression parameters (for instance: compression level or ZIG 
		client/server size threshold for compressing packets). 

		\see zig_avg_compression_gain() which sets the 'compressed' and 'uncompressed' byte counters to zero. 
*/
void zig_reset_compression_stats();

// -------------------------------------------------------------------------------

/*! \brief Checks if a given IP address is a 'private' address.

		The text below is from Wikipedia ( http://en.wikipedia.org/wiki/IPv4#Private_networks ): 

		"Of the 4 billion addresses allowed in IPv4, four ranges of address are reserved for private networking use only. 
		These ranges are not routable outside of private networks, and private machines cannot directly communicate with 
		public networks. They can, however, do so through network address translation.

<PRE>
The following are the four ranges reserved for private networks:
Name          IP address range              number of IPs  classful description     largest CIDR block
24-bit block  10.0.0.0 – 10.255.255.255     16,777,216     single class A           10.0.0.0/8
20-bit block  172.16.0.0 – 172.31.255.255   1,048,576      16 contiguous class Bs   172.16.0.0/12
16-bit block  192.168.0.0 – 192.168.255.255 65,536         256 contiguous class Cs  192.168.0.0/16
16-bit block  169.254.0.0 – 169.254.255.255 65,536         256 contiguous class Cs  169.254.0.0/16
</PRE>

		In addition to private networking, the IP range 127.0.0.0 – 127.255.255.255 (or 127.0.0.0/8 in CIDR notation) 
		is reserved for localhost communication. Any address within this range should never appear on an actual network 
		and any packet sent to this address does not leave the source computer, and will appear as an incoming packet on 
		that computer (known as Loopback)."

		If the IP address provided as argument to this function falls in any of the categories above, this function 
		returns \c true. Otherwise, this function returns \c false.

		\param address The IP address you want to check, in string form and in IP dot notation ("X.X.X.X"). For instance, 
		"143.54.12.74" or "10.0.0.2".

		\return Whether the given IP address is 'private' IP address or not:
		- \c true Yes it is.
		- \c false No it isn't.
*/
bool check_private_IP(char *address);

/*! \brief Checks if a given IP address is a 'private' address.

		See check_private_IP(char*) above for the Wikipedia explanation.
		
		\param address The IP address you want to check, as a HawkNL NLaddress object.

		\return Whether the given IP address is 'private' IP address or not:
		- \c true Yes it is.
		- \c false No it isn't.
*/
bool check_private_IP(NLaddress &address);

/*! \brief String tokenizer. On a given string, returns the next sequence of characters which is delimited by a 
		pair of whitespace and tab characters at both ends or, optionally, by a pair of double-quotes (ASCII 34).

		This function can be used to tokenize a null-terminated string, by breaking it into sequences of chars that 
		are separated by sequences of spaces and tabs at both ends. For instance, if you have the following string:
		<TT>/connect 127.0.0.1</TT> which is a command-line on your game console or a line read from a script, you 
		would probably like to break it up in <TT>/connect</TT> and in <TT>127.0.0.1</TT>, so that you can interpret 
		it as being a command ("connect to a server") and its argument (the IP address of the target server).

		However, each call to getcmd() can extract only one token. So, a getcmd() call must be placed in a loop or 
		in a chain of "if" statements. The \c cmd argument is a pointer to a char pointer that you have. The char pointer, 
		initially, should point to the beginning (first char) of the whole command string. After a call to getcmd(), 
		considering the example above, the char pointer will be modified by getcmd() to point to the first char of the 
		second token, which happens to be the first char after the whitespace which signals the end of the first token.
		The pointer to the modified char pointer is then supplied again as argument to getcmd(), which will start parsing 
		the input string from that position instead of the beginning, and thus now returning the next token on the sequence,
		which on the example above is <TT>127.0.0.1</TT>. 
		
		To extract all tokens, getcmd() should be called until it returns zero or a negative value, which means that there 
		are no more tokens to read.

    \param cmd A pointer, to a pointer which points to one of the chars of a null-terminated string. The 
		actual char being pointed at is the current position of the string tokenizer;
		\param buf A writable buffer that will receive the next token, which is extracted from \c cmd by a call to 
		this function;
		\param doubleQuote Whether double quotes should indicate a single token, overriding the whitespace separator:
		- \c true to recognize "this is a token" as a single token/substring/command:  ("this is a token")
		- \c false to recognize "this is a token" as the following set of four tokens:  ("this) (is) (a) (token")

		\return Result of the operation:
		- A positive value (greater than zero): Next token read OK, token is returned through \c buf;
		- \c 0 (zero): String is empty, no more tokens to read from the input string;
		- \c -1: Syntax error (end of string reached while looking for an enclosing double-quote (") char.
*/
int getcmd(char* &cmd, char *buf, bool doubleQuote);

/*! \brief Converts int to string.

		\param val The int value to be converted to string.

		\return STL std::string which contains the integer value (e.g.: 100) as a string of chars ("100").
*/
std::string itoa(int val);

/*! \brief Convert float to string.

		\param val The float value to be converted to string.

		\return STL std::string which contains the float value (e.g.: 1.5) as a string of chars ("1.5").
*/ 
std::string ftoa(float val);

/*! \brief Convert string to int.

		\param str The string value (STL std::string) to be converted to int.

		\return int value (e.g.: 100) that corresponds to the given string ("").
*/ 
int atoi(std::string str);

/*! \brief Convert string to double.

		\param str The string value (STL std::string) to be converted to double.

		\return double value (e.g.: 1.5) that corresponds to the given string ("1.5").
*/ 
double atof(std::string str);

/*! \brief Rounds double to the nearest integer.

		Works with negative values.

		\param val \c double value that is to be rounded.

		\return Integer value that is nearest to \c val .
*/
int droundi(double val);

#ifndef NO_DOXYGEN		// internal code -- do not document

// Old packet id API
// these functions enable LEETNET to use just an unsigned short (unsigned 16bit) value
// as a packet id, by handling when the id value wraps around 16K back to zero.

// identifier zero is reserved to special packets (bypassed directly to higher level)
#define ZIG_FIRST_PACKET_ID 1

// we will be using one bit of the 16 bits to check if the packet is compressed or not, 
// and another bit to signal for an extra "vport ID" (virtual port), so, there are 14 bits
// to encode the packet ID
#define ZIG_LAST_PACKET_ID 16319 // packets in the [16320, 16384) are for internal use
#define ZIG_HALF_PACKET_ID ( (ZIG_FIRST_PACKET_ID + ZIG_LAST_PACKET_ID + 1) / 2)

// invalid packet identifier (all bits in 1)
#define ZIG_INVALID_PACKET_ID 65535

#define ZIG_PACKET_ID_RANGE (ZIG_LAST_PACKET_ID - ZIG_FIRST_PACKET_ID + 1)

/// increment the packet id, wrapping around appropriately
int inc_packet_id(NLushort *id);
bool older_packet_id(NLushort new_id, NLushort base_id);
int  diff_packet_id (NLushort new_id, NLushort current_id);

// ZIG 2: New message id API
#define ZIG_FIRST_MSG_ID 0
#define ZIG_LAST_MSG_ID 32767
#define ZIG_HALF_MSG_ID ( (ZIG_FIRST_MSG_ID + ZIG_LAST_MSG_ID + 1) / 2)

// invalid message identifier (all bits in 1)
#define ZIG_INVALID_MSG_ID 65535

int  inc_msg_id  (NLushort* id);
bool older_msg_id(NLushort new_id, NLushort base_id);
int  diff_msg_id (NLushort new_id, NLushort current_id);

#endif // internal code -- do not document

#endif


