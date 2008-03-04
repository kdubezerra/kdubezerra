/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*

	shared buffer, version 2 - no base class

*/

	//temporary code - for debugging
	//#include <allegro.h>
	//#include <winalleg.h>
	//#include "../include/zig/console.h"
	//#include "../../minigame/conalleg.h"
	//extern conalleg_c *con;

// my header
#include "zig/buffer.h"

#include "zig/zig.h"
#include "zig/utils.h"
#include <stdio.h>

using namespace std;

//THE TYPEREG stuff
//#include "../include/zig/typereg.h"

//BZIP2
#include "../include/zig/bzip2.h"

//extern vars from <utils.cpp>
extern int zig_bzip2_compression_level; // bzip2 global compression level

#ifndef BUFFER_NO_STATS

mutex_c cbuffer_mutex;
volatile int cbuffer_count = 0;
volatile int cbuffer_blocks = 0;
volatile int cbuffer_totmem = 0;
volatile int cbuffer_maxmem = 0;

void check_cbuffer_maxmem(int yeah) {
	cbuffer_mutex.lock();
	if (yeah > cbuffer_maxmem)
		cbuffer_maxmem = yeah;
	cbuffer_mutex.unlock();
}

#endif

//#define THROW(X) FAILSTOP()
#define THROW(X) throw X;

// --------------------------------------------------------------
// begin default implementation of serializable_c::read() and write()
// --------------------------------------------------------------

/// read fields from stream. 
/// WARNING: you should override this! serializable_c provides a default implementation that does 
/// a simple bitwise memory copy. this isn't very safe, accurate or even portable! (remember 
/// the lessons on "C++ copy constructors")
/// WARNING #2: the default implementation only supports objects that are under 32K in size. if 
/// the object is larger, the result is undefined behavior.
void serializable_c::read(buffer_c &in) {

	int object_size = in.get32K();
	in.getBlock((char *) this, object_size);
}

void serializable_c::read(buffer_c::iterator& in) {

	int object_size = in.get32K();
	in.getBlock((char *) this, object_size);
}

/// write fields to stream.
/// WARNING: you should override this! serializable_c provides a default implementation that does 
/// a simple bitwise memory copy. this isn't very safe, accurate or even portable! (remember 
/// the lessons on "C++ copy constructors")
/// WARNING #2: the default implementation only supports objects that are under 32K in size. if 
/// the object is larger, the result is undefined behavior.
void serializable_c::write(buffer_c &out) const {

	int object_size = ZIG_GetMySizeof();
	out.put32K(object_size);
	out.putBlock((char*) this, object_size);
}

// --------------------------------------------------------------
// end default implementation of serializable_c::read() and write()
// --------------------------------------------------------------


//================================================================

//ctor
buffer_c::buffer_c(): _begin(*this), _end(*this) {

#ifndef BUFFER_NO_STATS

	//stats : new object
	cbuffer_mutex.lock();
	cbuffer_count++;
	cbuffer_mutex.unlock();

#endif
	
	pos = 0;
	code = -666;
	id = -666;
	//internal = -666;
	
	assert(buf.data == 0);  // MUST have been constructed before!
	assert(buf.alen == 0);
	assert(buf.ulen == 0);

	_end.pos = buf.ulen;
}	

//initial capacity ctor
buffer_c::buffer_c(int size): _begin(*this), _end(*this) {

#ifndef BUFFER_NO_STATS

	//stats : new object
	cbuffer_mutex.lock();
	cbuffer_count++;
	cbuffer_mutex.unlock();

#endif

	pos = 0;
	code = -666;
	id = -666;
	//internal = -666;

	assert(buf.data == 0);  // MUST have been constructed before!
	assert(buf.alen == 0);
	assert(buf.ulen == 0);

	grow(size, false);

	_end.pos = buf.ulen;
}

//copy ctor
buffer_c::buffer_c(const buffer_c &b): _begin(*this), _end(*this) {

#ifndef BUFFER_NO_STATS

	//stats : new object
	cbuffer_mutex.lock();
	cbuffer_count++;
	cbuffer_mutex.unlock();

#endif

	//deep copy b to this
	makecopy(b);

	_end.pos = buf.ulen;
}

//assignment operator
buffer_c& buffer_c::operator=(const buffer_c &b) {
	
	if (this != &b) {

		//purge old contents
		purge();

		//deep copy b to this
		makecopy(b);

	}

	_end.pos = buf.ulen;
	return (*this);
}

//makecopy (private method)
void buffer_c::makecopy(const buffer_c &b) {

	buf.alen = b.buf.ulen;		//buffer will be trimmed
	buf.ulen = b.buf.ulen;

	//data to copy?
	if (b.buf.data != 0) {

#ifndef BUFFER_NO_STATS

		//stats
		cbuffer_mutex.lock();
		cbuffer_blocks++;
		cbuffer_totmem += buf.alen;
		cbuffer_mutex.unlock();
		check_cbuffer_maxmem(buf.alen);

#endif

		//alloc buf.data and make deep copy from b.buf
		buf.data = new char[buf.alen];
		assert(buf.data);
		memcpy(buf.data, b.buf.data, b.buf.ulen);
	}
	else	
		buf.data = 0; //no data

	pos = b.pos;
	code = b.code;
	id = b.id;
	//internal = b.internal;
}

//dtor
buffer_c::~buffer_c() {

#ifndef BUFFER_NO_STATS

	//stats: less one object
	cbuffer_mutex.lock();
	cbuffer_count--;
	cbuffer_mutex.unlock();

#endif

	//purge
	purge();
}

///tells the buffer object to wrap the previously allocated buffer "buffer"
///of size "buffer_size". the buffer object will be responsible for freeing
///the buffer when all references to it are gone.
void buffer_c::wrap(char *buffer, int buffer_size) {

#ifndef BUFFER_NO_STATS

	//stats: another block
	cbuffer_mutex.lock();
	cbuffer_blocks++;								//another  block
	cbuffer_totmem += buffer_size;
	cbuffer_mutex.unlock();
	check_cbuffer_maxmem(buffer_size);

#endif
	
	//discard old contents, if any
	purge();

	//set new contents.
	buf.data = buffer;
	buf.alen = buffer_size;
	buf.ulen = buffer_size;

	_end.pos = buf.ulen;
}

///wipe out all memory allocated for this buffer
void buffer_c::purge() {

	if (buf.data != 0) {
		
#ifndef BUFFER_NO_STATS

		//status: less one block
		cbuffer_mutex.lock();
		cbuffer_blocks--;
		cbuffer_totmem -= buf.alen;
		cbuffer_mutex.unlock();

#endif

		delete[] buf.data;  // v1.3.2 fixed: was "delete buf.data" (memory leak) fixed by Milan Mimica
		buf.data = 0;
	} 

	buf.ulen = buf.alen = pos = 0;

	_end.pos = buf.ulen;
}

/// ensure minimum allocated capacity on the internal buffer pointer that is 
/// returned by data()
void buffer_c::ensure_capacity(int capacity) {

	// grow() is relative to current pos, so take off "pos" off the equation, and 
	// the desired effect is obtained
	grow(capacity - pos, false);

	_end.pos = buf.ulen;
}

/// get the current allocated capacity of the internal buffer that is 
/// returned by data()
int buffer_c::get_capacity() const {

	int retval = buf.alen;
	return retval;
}

///erase buffer contents
void buffer_c::clear() { 
	
	buf.ulen = pos = 0; 
	_end.pos = buf.ulen;
}

///seek
void buffer_c::seek(int ptarg) { 

	//avoid negative seeks
	if (ptarg < 0)
		ptarg = 0;

	pos = ptarg;
}

///seek end (insertion position)
void buffer_c::seek_end() { 
	seek( size() ); 
}

///skip
void buffer_c::skip(int amount) { 
	seek( getpos() + amount ); 
}

///get pos
int buffer_c::getpos() const { 
	int p = pos; 
	return p;
}

///get size
int buffer_c::size() const { 
	int u = buf.ulen; 
	return u;
}

///get size of remaining to read
int buffer_c::size_left() const { 
	int d = (buf.ulen - pos); 
	return d;
}

//get the buffer pointer
char *buffer_c::data() const {
	// WARNING: POINTER RETURNED IS NOT AUTOMAGICALLY SAFE TO USE
	return buf.data;
}

//get a buffer pointer + current pos
char *buffer_c::data_cur() const {
	// WARNING: POINTER RETURNED IS NOT AUTOMAGICALLY SAFE TO USE
	return &(buf.data[pos]);
}

///advance ulen and pos (USE ONLY IF YOU KNOW WHAT YOU'RE DOING!)
void buffer_c::manual_put_block(int amount) { 

	buf.ulen += amount; 
	pos += amount; 
	if (buf.ulen > buf.alen) 
		FAILSTOP(); 

	_end.pos = buf.ulen;
}

//read operations
NLubyte buffer_c::getByte() {
// v1.5.0: removing redundant asserts	assert(buf.data);  
	if (buf.data == 0)
		THROW(EX_NULL);
// v1.5.0: removing redundant asserts	assert(pos + (int)sizeof(NLubyte) <= buf.ulen);
	if (pos + (int)sizeof(NLubyte) > buf.ulen)
		THROW(EX_EOB);
	NLubyte b;
	readByte(buf.data, pos, b);
	return b;
}
NLbyte buffer_c::getBytes() {
// v1.5.0: removing redundant asserts	assert(buf.data);
	if (buf.data == 0)
		THROW(EX_NULL);
// v1.5.0: removing redundant asserts	assert(pos + (int)sizeof(NLbyte) <= buf.ulen);
	if (pos + (int)sizeof(NLbyte) > buf.ulen)
		THROW(EX_EOB);
	NLbyte b;
	readByte(buf.data, pos, b);
	return b;
}
NLushort buffer_c::getShort() {
// v1.5.0: removing redundant asserts	assert(buf.data);
	if (buf.data == 0)
		THROW(EX_NULL);
// v1.5.0: removing redundant asserts	assert(pos + (int)sizeof(NLushort) <= buf.ulen);
	if (pos + (int)sizeof(NLushort) > buf.ulen)
		THROW(EX_EOB);
	NLushort b;
	readShort(buf.data, pos, b);
	return b;
}
NLshort buffer_c::getShorts() {
// v1.5.0: removing redundant asserts	assert(buf.data);
	if (buf.data == 0)
		THROW(EX_NULL);
// v1.5.0: removing redundant asserts	assert(pos + (int)sizeof(NLshort) <= buf.ulen);
	if (pos + (int)sizeof(NLshort) > buf.ulen)
		THROW(EX_EOB);
	NLshort b;
	readShort(buf.data, pos, b);
	return b;
}
NLulong buffer_c::getLong() {
// v1.5.0: removing redundant asserts	assert(buf.data);
	if (buf.data == 0)
		THROW(EX_NULL);
// v1.5.0: removing redundant asserts	assert(pos + (int)sizeof(NLulong) <= buf.ulen);
	if (pos + (int)sizeof(NLulong) > buf.ulen)
		THROW(EX_EOB);
	NLulong b;
	readLong(buf.data, pos, b);
	return b;
}
NLlong buffer_c::getLongs() {
// v1.5.0: removing redundant asserts	assert(buf.data);
	if (buf.data == 0)
		THROW(EX_NULL);
// v1.5.0: removing redundant asserts	assert(pos + (int)sizeof(NLlong) <= buf.ulen);
	if (pos + (int)sizeof(NLlong) > buf.ulen)
		THROW(EX_EOB);
	NLlong b;
	readLong(buf.data, pos, b);
	return b;
}
NLfloat buffer_c::getFloat() {
// v1.5.0: removing redundant asserts	assert(buf.data);
	if (buf.data == 0)
		THROW(EX_NULL);
// v1.5.0: removing redundant asserts	assert(pos + (int)sizeof(NLfloat) <= buf.ulen);
	if (pos + (int)sizeof(NLfloat) > buf.ulen)
		THROW(EX_EOB);
	NLfloat b;
	readFloat(buf.data, pos, b);
	return b;
}
NLdouble buffer_c::getDouble() {
// v1.5.0: removing redundant asserts	assert(buf.data);
	if (buf.data == 0)
		THROW(EX_NULL);
// v1.5.0: removing redundant asserts	assert(pos + (int)sizeof(NLdouble) <= buf.ulen);
	if (pos + (int)sizeof(NLdouble) > buf.ulen)
		THROW(EX_EOB);
	NLdouble b;
	readDouble(buf.data, pos, b);
	return b;
}
void buffer_c::getBlock(char *buffer, int length) {
// v1.5.0: removing redundant asserts	assert(buf.data);
	if (buf.data == 0)
		THROW(EX_NULL);
// v1.5.0: removing redundant asserts	assert(pos + length <= buf.ulen);
	if (pos + length > buf.ulen)
		THROW(EX_EOB);
	readBlock(buf.data, pos, buffer, length);	
}
string buffer_c::getString() {
// v1.5.0: removing redundant asserts	assert(buf.data);
	if (buf.data == 0)
		THROW(EX_NULL);

	//must have at least one byte left on buffer (the zero of zero terminated string)
// v1.5.0: removing redundant asserts	assert(pos + (int)sizeof(NLubyte) <= buf.ulen);
	if (pos + (int)sizeof(NLubyte) > buf.ulen)
		THROW(EX_EOB);
	
	char big_buffer[16384];
	readString(buf.data, pos, big_buffer);

	return string(big_buffer);
}

//special read operation: read data and nlWrite(...) it to a socket. returns result of nlWrite(...)
int buffer_c::getDataToSocket(NLsocket sock, int length) {
	
// v1.5.0: removing redundant asserts	assert(buf.data);
	if (buf.data == 0)
		THROW(EX_NULL);
// v1.5.0: removing redundant asserts	assert(pos + length <= buf.ulen);
	if (pos + length > buf.ulen)
		THROW(EX_EOB);

	int result = nlWrite(sock, &(buf.data[pos]), length);
	if (result > 0)
		pos += result;

	//	assert(result == length);
	//	if (result != length)
	//		FAILSTOP();

	return result;
}

buffer_c& buffer_c::operator>>(NLubyte &v) {
	v = getByte();
	return *this;
}

buffer_c& buffer_c::operator>>(NLbyte &v) {
	v = getBytes(); return *this;
}

buffer_c& buffer_c::operator>>(NLushort &v) {
	v = getShort();
	return *this;
}

buffer_c& buffer_c::operator>>(NLshort &v) {
	v = getShorts();
	return *this;
}

buffer_c& buffer_c::operator>>(NLulong &v) {
	v = getLong();
	return *this;
}

buffer_c& buffer_c::operator>>(NLlong &v) {
	v = getLongs();
	return *this;
}

buffer_c& buffer_c::operator>>(NLfloat &v) {
	v = getFloat();
	return *this;
}

buffer_c& buffer_c::operator>>(NLdouble &v) {
	v = getDouble();
	return *this;
}

buffer_c& buffer_c::operator>>(std::string &v) {
	v = getString();
	return *this;
}

//write operations	
void buffer_c::putByte(NLubyte i) {
	grow(sizeof(NLubyte));
	writeByte(buf.data, pos, i);
	_end.pos = buf.ulen;
}
void buffer_c::putBytes(NLbyte i) {
	grow(sizeof(NLbyte));
	writeByte(buf.data, pos, i);
	_end.pos = buf.ulen;
}
void buffer_c::putShort(NLushort i) {
	grow(sizeof(NLushort));
	writeShort(buf.data, pos, i);
	_end.pos = buf.ulen;
}
void buffer_c::putShorts(NLshort i) {
	grow(sizeof(NLshort));
	writeShort(buf.data, pos, i);
	_end.pos = buf.ulen;
}
void buffer_c::putLong(NLulong i) {
	grow(sizeof(NLulong));
	writeLong(buf.data, pos, i);
	_end.pos = buf.ulen;
}
void buffer_c::putLongs(NLlong i) {
	grow(sizeof(NLlong));
	writeLong(buf.data, pos, i);
	_end.pos = buf.ulen;
}
void buffer_c::putFloat(NLfloat i) {
	grow(sizeof(NLfloat));
	writeFloat(buf.data, pos, i);
	_end.pos = buf.ulen;
}
void buffer_c::putDouble(NLdouble i) {
	grow(sizeof(NLdouble));
	writeDouble(buf.data, pos, i);
	_end.pos = buf.ulen;
}
void buffer_c::putBlock(char *buffer, int length) {

	if (length > 0) {

		//passing null buffer with length > 0 - already happened because of another bug!
		if (buffer == 0)
			FAILSTOP();		//crash nicely

		grow(length);
		writeBlock(buf.data, pos, buffer, length);
		_end.pos = buf.ulen;
	}
}
void buffer_c::putString(string cppstr) {
	const char *str = cppstr.c_str();
	int slen = strlen(str);
	grow(slen+1);
	writeString(buf.data, pos, str);
	_end.pos = buf.ulen;
}

//special write operation: write read data from a socket. size: maximum size expected (buffer
//will be expanded). returns code from nlRead(...)
int buffer_c::putDataFromSocket(NLsocket sock) {

	char lebuf[65536];
	int result = nlRead(sock, lebuf, 65536);
	if (result > 0)
		putBlock(lebuf, result);		// if read something, put it on the buffer
	_end.pos = buf.ulen;
	return result;
}

//special write operation: write read data from a socket. size: maximum size expected (buffer
//will be expanded). returns code from the last nlRead(...)
int buffer_c::putDataFromSocket(NLsocket sock, int size) {

#define _PUT_DATA_CHUNK_SIZE 65536

	int result;

	//how much left
	int size_left = size;

	do {

		//read in 64K chunks AT MOST
		char lebuf[ _PUT_DATA_CHUNK_SIZE ];

		// try to read all that is left
		int chunksize = size_left;
		if (chunksize > _PUT_DATA_CHUNK_SIZE)
			chunksize = _PUT_DATA_CHUNK_SIZE;
	
		// read the chunk
		result = nlRead(sock, lebuf, chunksize);

		//error!
		if (result < 1) { // WAS: result < chunksize
			//con->printf("ERROR: buffer_c::putDataFromSocket, nlRead()==%i, size was=%i\n", result, chunksize);
			return NL_INVALID;
		}

		//con->printf("   nlRead() RESULT = %i of %i\n", result, chunksize);
		
		if (result > 0)
			putBlock(lebuf, result);		// if read something, put it on the buffer

		//update size left
		size_left -= result;

		//if result less than chunk size, we're done (this was the last one)
		//if (result < _PUT_DATA_CHUNK_SIZE)
		//	break;

		//if there's nothing left to download, we're done
		if (size_left == 0)
			break;

	} while (result != NL_INVALID);

	_end.pos = buf.ulen;
	return result;
}

buffer_c& buffer_c::operator<<(NLubyte v) {
	putByte(v);
	return *this;
}

buffer_c& buffer_c::operator<<(NLbyte v) {
	putBytes(v);
	return *this;
}

buffer_c& buffer_c::operator<<(NLushort v) {
	putShort(v);
	return *this;
}

buffer_c& buffer_c::operator<<(NLshort v) {
	putShorts(v);
	return *this;
}

buffer_c& buffer_c::operator<<(NLulong v) {
	putLong(v);
	return *this;
}

buffer_c& buffer_c::operator<<(NLlong v) {
	putLongs(v);
	return *this;
}

buffer_c& buffer_c::operator<<(NLfloat v) {
	putFloat(v);
	return *this;
}

buffer_c& buffer_c::operator<<(NLdouble v) {
	putDouble(v);
	return *this;
}

buffer_c& buffer_c::operator<<(std::string v) {
	putString(v);
	return *this;
}

buffer_c& buffer_c::operator<<(const serializable_c &v) {
	putObject(v);
	return *this;
}

//make buffer grow if needed, to make room for more "amount" bytes of data at "pos"
void buffer_c::grow(long amount, bool writing) {
	
	//RECORD OLD ULEN VALUE FOR REALLOCATIONS!
	int old_ulen = buf.ulen;
	
	//if writing data, advance "ulen" accordingly
	if (writing) {

		//ulen is just the biggest value between (ulen, pos+amount)
		if (pos + amount > buf.ulen)
			buf.ulen = pos + amount;
	}

	//minimum size of buffer after call
	int minsize = pos + amount;
	
	//need to grow?
	if (minsize <= buf.alen) {
		return;		//no
	}

	//calc new size of buffer
	//
	//		//int targsize = BUFFER_CHUNK_SIZE;
	//		//while (targsize < minsize)
	//		//	targsize += BUFFER_CHUNK_SIZE;
	//
	int targsize = ((minsize-1) / BUFFER_CHUNK_SIZE) * BUFFER_CHUNK_SIZE + BUFFER_CHUNK_SIZE;

	//paranoia
	assert(targsize >= buf.ulen);

	//if buf.data == 0, just create it
	if (buf.data == 0) {

#ifndef BUFFER_NO_STATS

		//status: another block
		cbuffer_mutex.lock();
		cbuffer_blocks++;
		cbuffer_totmem += targsize;
		cbuffer_mutex.unlock();
		check_cbuffer_maxmem(targsize);

#endif

		//alloc it
		buf.data = new char[targsize];
		assert(buf.data);
	}
	//expand it
	else {

#ifndef BUFFER_NO_STATS

		//status: more memory used, block gets bigger
		cbuffer_mutex.lock();
		cbuffer_totmem -= buf.alen;				//minus previously allocated len
		cbuffer_totmem += targsize;		//plus new allocation size
		cbuffer_mutex.unlock();
		check_cbuffer_maxmem(targsize);

#endif

		//record old data ptr
		char* o_data = buf.data;

		//alloc it
		buf.data = new char[targsize];
		assert(buf.data);

		//copy contents of old to new
		memcpy(buf.data, o_data, old_ulen); // FIXED: MOVE "OLD_ULEN" !!!
		//delete old buffer
		delete[] o_data; // v1.3.2 fixed: was "delete o_data" (memory leak) fixed by Milan Mimica
	}

	//update alen to the allocated "targsize"
	buf.alen = targsize;

	_end.pos = buf.ulen;
}

// --------------------------------------------------------------
// experimental object serialization support:
// --------------------------------------------------------------

///NEW: write an object of a REGISTERED class that extends serializable_c
///THE CLASS MUST BE REGISTERED
void buffer_c::putObject(const serializable_c &obj) {

/*
	//thanks to the magic macro ZIG_SERIALIZABLE_CLASS, we can get the object's id
	//and write it to the network, so the other side can use this "code" to automagically
	//know from which class to make a "new" when rebuilding the object on the other side
	int code = obj.ZIG_GetTypeId();

	//use special functions that can write/read a value from 0 to 32k using only one
	//byte for values in the range 0..127 (optimization)
	put32K(code);
*/	
	// write this object's actual fields to the buffer
	obj.write(*this);

	_end.pos = buf.ulen;
}

///read a value in the range [0..32767] where values [0..127] use only one byte and [128..32767] use two bytes
int buffer_c::get32K() {

	//at least one byte is there for sure!
	int b0 = (int)getByte();

	if (b0 < 128)
		return b0;
	else {
		int b1 = (int)getByte();
		b1 = b1 << 7;	// "make room" for b0's 7 bits
		int i = b0 + b1 - 128;	// FIXED: must subtract "128", the high bit of b0 that signals use of 2 bytes to encode the value
        return i;
	}
}

///write a value in the range [0..32767] where values [0..127] use only one byte and [128..32767] use two bytes
///passing a negative value or a value greater than 32767 as argument produces undefined behavior
void buffer_c::put32K(int i) {

	if (i < 128)
		putByte((NLubyte)i);
	else {
		int b0 = (i & 127) + 128; //byte 0 = first 7 bits of i + "128" (set higher bit to flag)
		int b1; //byte 1
		b1 = i;
		b1 = b1 >> 7;  //"push off right" the first 7 bits of i (already on b0)
		putByte((NLubyte)b0);
		putByte((NLubyte)b1);
	}

	_end.pos = buf.ulen;
}

/// read a value written with put128Ms() (see comments below)
int buffer_c::get256Ms() {

	int b0 = (int)getByte();

	int m = (b0 & 252) >> 2; // put bits 2..7 of coded byte into bits 0..5 of m

	// more bytes?
	if (b0 & 1) {

		int b1 = (int)getByte();

		m += (b1 & 254) << 5; // put bits 1..7 of coded byte into bits 6..12 of m

		// more bytes?
		if (b1 & 1) {

			int b2 = (int)getByte();
			m += (b2 & 254) << 12; // put bits 1..7 of coded byte into bits 13..19 of m

			// more?
			if (b2 & 1) {

				int b3 = (int)getByte(); // put all 8 bits 0..7 of coded byte into bits 20..27 of m
				m += b3 << 20;
			}
		}
	}

	// if signal bit is set: negative value
	if (b0 & 2)  
		m *= -1;

	return m;
}

/// write a signed value in the range [-268435455..268435455] where values with abs() in range [0..63] use only 
/// one byte, and values with abs() in range [64..8191] use only 2 bytes, and values with abs() in range 
/// [8192..1048575] use only 3 bytes and values with abs() in range [1048576..268435455] use 4 bytes. 
/// NOTE: passing an out-of-range value (not in [-268435455..268435455] ) as argument causes undefined behavior. 
void buffer_c::put256Ms(int i) {
	
	// encoding:
	// byte 1:
	//  bit 0: 1 if value is encoded in at least 2 bytes
	//  bit 1: signal bit, 0 if positive value, 1 if negative value
	//  bits 2-7: bits 0..5 of value magnitude (+6 bits)
	// byte 2 (optional):
	//  bit 0: 1 if value is encoded in at least 3 bytes
	//  bits 1-7: bits 6..12 of value magnitude (+7 bits)
	// byte 3 (optional):
	//  bit 0: 1 if value is encoded in 4 bytes
	//  bits 1-7: bits 13..19 of value magnitude (+7 bits)
	// byte 4 (optional):
	//  bits 0-7: bits 20..27 of value magnitude (+8 bits)

	// 1048576 --> 2097152

	int ai = abs(i);	 // ai = value magnitude

	// set bit 1 (signal), clear bit 0
	int b0;
	if (i >= 0) b0 = 0; else b0 = 2;

	if (ai > 63) b0 += 1;					// if value > 63 set bit0 of b0

	b0 += ((ai & 63) << 2);				// encode bits 0..5 of ai in bits 2-7 of b0
	
	putByte((NLubyte)b0);

	// if has more bytes to send, keep going
	if (ai > 63) {

		int b1 = ((ai & 8128) >> 5);  // encode bits 6..12 of ai in bits 1-7 of b1
		if (ai > 8191) b1 += 1;       // if value > 8191 set bit0 of b1
		
		putByte((NLubyte)b1);

		// more bytes?
		if (ai > 8191) {

			//8192+16384+32768+65536+131072+262144+524288 = 1040384

			int b2 = ((ai & 1040384) >> 12);  // encode bits 13..19 of ai in bits 1-7 of b2
			if (ai > 1048575) b2 += 1;				// if value > 1048575 set bit0 of b2
			
			putByte((NLubyte)b2);

			// more?
			if (ai > 1048575) {

				// 1048756+2097152+4194304+8388608+16777216+33554432+67108864+134217728 = 267387060

				int b3 = ((ai & 267387060) >> 20); // encode bits 20..27 of ai in the 8 bits of b3

				putByte((NLubyte)b3);
			}
		}
	}

	_end.pos = buf.ulen;

	//
	// full test code for put/get256Ms() :
	//
	/*
		// testing put256Ms()/get256Ms()
		int err = 100;
		int cont = 0;
		con->printf("put256Ms()/get256Ms() test started\n");
		gameclient->render_frame();
		buffer_c buf;
		for (int i=-268435455;i<=268435455;i++) {  // full test
			buf.clear();
			buf.put256Ms(i);
			int si = buf.getpos();
			buf.seek(0);
			int j = buf.get256Ms();
			int sj = buf.getpos();

			if (i != j) {
				con->printf("FAILED!: put: %i, got: %i\n", i, j);
				if (err -- < 0) break;
			}
			if (si != sj) {
				con->printf("FAILED!: put: %i, got: %i, putsize=%i != getsize=%i\n", i, j, si, sj);
				if (err -- < 0) break;
			}

			if (cont++ > 1000000) {
				cont = 0;
				con->printf("reached i=%i\n", i);
				gameclient->render_frame();
			}
		}
		con->printf("put256Ms()/get256Ms() test ended.\n");
	*/
}

#ifndef ZIG_NO_BZIP2 // don't include BZIP2

///compress this buffer into another buffer passed as parameter. 
///be sure to seek() to the desired position in both the source and the 
///destination buffer. the optional parameter is the amount of bytes to read for compression (if not 
///specified, all remaining bytes are read and compressed into the destination). 
///RETURN VALUES: returns <0 if the compression failed for any reason; returns the number of bytes 
///written to dest otherwise.
///CURRENTLY, the library used to compress/decompress is BZIP2. 
///BZIP2 1.02 from http://sources.redhat.com/bzip2 
int buffer_c::zip(buffer_c &dest, int amount) {

	//if no amount given, then everything left from src (this)
	if (amount == -1)
		amount = size_left();

	//we don't need to record the ACTUAL uncompressed size; we just need a few bits to
	//  tell what "order of magnitude" of memory we need to pre-allocate when decompressing.
	//write 1 or 2 header bytes telling the approximate uncompressed size of the compressed
	//data that follows. (so we know approximately how many bytes to pass to buff-decompress later...)
	int dest_inipos = dest.getpos();
  dest.put32K( (amount >> 8) );  //amount is recorded in steps of 256 bytes
	int dest_headersize = dest.getpos() - dest_inipos;

//	con->printf("ZIP HEADER: AMOUNT = %i\n", amount);

	//minimum bzip2 size for dest: 1% larger than input + 600
	int minfreedest = (int)( ((double)amount) * 1.01 ) + 600;

	//extra amount needed at dest: minfreedest, minus what free amount is already available (alen500-pos300=200)
	int extraneed = minfreedest - (dest.buf.alen - dest.getpos());

	//pre-allocate enough space in the destination buffer. writing: FALSE! just increase alen, not ulen
	if (extraneed > 0)
		dest.grow(extraneed, false);
	
	//dest, source, 2nd parameter is updated with the new dest data size if everything OK
	unsigned int dest_size = dest.buf.alen - dest.getpos();
	
  int result = BZ2_bzBuffToBuffCompress(
		(char*) &(dest.buf.data[dest.getpos()]),  //dest buffer initial position ptr
		&dest_size,                     //dest buffer available size and return value = compressed size written to dest
		(char*) &(buf.data[getpos()]),       //src buffer initial position ptr
		amount,                         //src buffer amount of uncompressed data to compress
		zig_bzip2_compression_level,0,0);

	//check for any error and abort
	if (result != BZ_OK)
		return -1; // ERROR!

	//now must FIX:
	//  BUF.ULEN at DEST to match the compressed size we got (the new dest_size)
	//  BUF.POS at DEST since we wrote zipped stuff
	dest.manual_put_block(dest_size);  //this does the job and mutex'es if necessary	

	//  BUF.POS at SRC since we read stuff
	skip(amount);	// nice

	dest._end.pos = dest.buf.ulen;

	//return the amount of bytes written to dest (header(data size) + the actual data)
	return dest_headersize + dest_size;
}

///decompress this buffer into another buffer passed as parameter. 
///RETURN VALUES: returns <0 if the decompression failed for any reason; returns the number of 
///bytes written to dest otherwise. 
///CURRENTLY, the library used to compress/decompress is BZIP2. 
///BZIP2 1.02 from http://sources.redhat.com/bzip2 
int buffer_c::unzip(buffer_c &dest) {

	//we don't need to record the ACTUAL uncompressed size; we just need a few bits to
	//  tell what "order of magnitude" of memory we need to pre-allocate when decompressing.
	//minimum size for dest is the uncompressed_size
	int uncompressed_size = (get32K() + 1) << 8;   // allocate in 256-byte steps
	
//	con->printf("UNZIP HEADER: UNCOMPRESSED_SIZE (ORDER OF MAGNITUDE) = %i\n", uncompressed_size);

	//extra amount needed at dest: uncompressed_size, minus what free amount is already available (alen500-pos300=200)
	int extraneed = uncompressed_size - (dest.buf.alen - dest.getpos());

	//pre-allocate enough space in the destination buffer. writing: FALSE! just increase alen, not ulen
	if (extraneed > 0)
		dest.grow(extraneed, false);
	
	//dest, source, 2nd parameter is updated with the new dest data size if everything OK
	unsigned int dest_size = dest.buf.alen - dest.getpos();

	//compressed size: we can just guess it here; if it's greater than the actual compressed size,
	//that's not a problem. (it's a problem if it's smaller than the actual compressed size)
	//
	//  THIS IS WRONG:          unsigned int compressed_size = 4000;
	//
	// How the GUESS below is done: we are assuming that the compressed size can be GREATER than the 
	// uncompressed size as a worst-case scenario, since size can increase when compressing.
	// This "times 2 plus a few-hundred bytes" formula is from the BZip docs.
	unsigned int compressed_size = (uncompressed_size * 2) + 600;
	// I'm not sure why the "4000" thing above appeared in the code, but to avoid a 
	// revert back to that, let's settle with this:
	if (compressed_size < 4000) { compressed_size = 4000; }
  
  int result = BZ2_bzBuffToBuffDecompress(
		(char*) &(dest.buf.data[dest.getpos()]),  //dest buffer initial position ptr
		&dest_size,                     //dest buffer available size and return value = decompressed size written to dest
		(char*) &(buf.data[getpos()]),       //src buffer initial position ptr
		&compressed_size,               //src buffer amount of compressed data to uncompress
		0,0);

	// NEW: the "sourceLen" as "*" parameter was a custom addition to BZIP2 code because we needed to
	// know the actual size the compressed data had in the source buffer.

	/*  //debugging
	if (result == BZ_PARAM_ERROR)
		return -3;
	if (result == BZ_DATA_ERROR)
		return -4;
	if (result == BZ_DATA_ERROR_MAGIC)
		return -5;
	if (result == BZ_MEM_ERROR)
		return -6;
	if (result == BZ_STREAM_END)
		return -7;
	*/

	//check for any error and abort
	if (result != BZ_OK)
		return -1; // ERROR!
	
	//now must FIX:
	//  BUF.ULEN at DEST to match the DEcompressed size we got (dest_size/uncompressed_size)
	//  BUF.POS at DEST since we wrote UNzipped stuff
	//dest.manual_put_block(uncompressed_size);  //this does the job and mutex'es if necessary	
	// NEW: use dest_size, uncompressed_size is not accurate anymore
	dest.manual_put_block(dest_size);  //this does the job and mutex'es if necessary	

	//  BUF.POS at SRC since we read the ZIPPED stuff
	skip(compressed_size);	// nice

	_end.pos = buf.ulen;

	//return the amount of bytes written to dest (the actual uncompressed data size)
	return dest_size;
}

#endif // ZIG_NO_BZIP2 // don't include BZIP2

void buffer_c::print(console_c* con) const {
	
	if (!con) return;

	char *data = buf.data;
	int   size = buf.ulen;

	std::string s;
	char aux[10];

	for (int i = 0; i < size; i++) {
		sprintf(aux, "%d ", static_cast<unsigned int>( data[i] ) );
		s.append(aux);
	}

	con->printf("%s\n", s.c_str() );
}

buffer_c::iterator::iterator(const iterator& it): buf(it.buf) {
	pos = it.pos;
}

buffer_c::iterator::iterator(buffer_c& buf): buf(buf) {
	pos = 0;
}

// Wrong, wrong implementation (but usually people prefers "it++;" instead "++it;")
buffer_c::iterator& buffer_c::iterator::operator++(int) {
	pos++;
	return (*this);
}

buffer_c::iterator& buffer_c::iterator::operator++() {
	++pos;
	return (*this);
}

bool buffer_c::iterator::operator==(const iterator& it) const {
	// TODO: how test if two references points to the same object?
	if (&(buf) != &(it.buf)) return false;
	else if (pos != it.pos) return false;
	else return true;
}

void  buffer_c::iterator::skip(const long amount) {
	pos += amount;
}

NLubyte buffer_c::iterator::getByte() {

	char* data = buf.data();
	if (data == NULL) THROW(EX_NULL);
	if (pos + (int) sizeof(NLubyte) > buf.size()) THROW(EX_EOB);

	NLubyte b;
	readByte(data, pos, b);
	return b;
}

NLbyte buffer_c::iterator::getBytes() {

	char* data = buf.data();
	if (data == NULL) THROW(EX_NULL);
	if (pos + (int) sizeof(NLbyte) > buf.size()) THROW(EX_EOB);

	NLbyte b;
	readByte(data, pos, b);
	return b;
}

NLushort buffer_c::iterator::getShort() {

	char* data = buf.data();
	if (data == NULL) THROW(EX_NULL);
	if (pos + (int) sizeof(NLushort) > buf.size()) THROW(EX_EOB);

	NLushort s;
	readShort(data, pos, s);
	return s;
}

NLshort buffer_c::iterator::getShorts() {

	char* data = buf.data();
	if (data == NULL) THROW(EX_NULL);
	if (pos + (int) sizeof(NLshort) > buf.size()) THROW(EX_EOB);

	NLshort s;
	readShort(data, pos, s);
	return s;
}

NLulong buffer_c::iterator::getLong() {

	char* data = buf.data();
	if (data == NULL) THROW(EX_NULL);
	if (pos + (int) sizeof(NLulong) > buf.size()) THROW(EX_EOB);

	NLulong l;
	readLong(data, pos, l);
	return l;
}

NLlong buffer_c::iterator::getLongs() {

	char* data = buf.data();
	if (data == NULL) THROW(EX_NULL);
	if (pos + (int) sizeof(NLlong) > buf.size()) THROW(EX_EOB);

	NLlong l;
	readLong(data, pos, l);
	return l;
}

NLfloat buffer_c::iterator::getFloat() {

	char* data = buf.data();
	if (data == NULL) THROW(EX_NULL);
	if (pos + (int) sizeof(NLfloat) > buf.size()) THROW(EX_EOB);

	NLfloat f;
	readFloat(data, pos, f);
	return f;
}

NLdouble buffer_c::iterator::getDouble() {

	char* data = buf.data();
	if (data == NULL) THROW(EX_NULL);
	if (pos + (int) sizeof(NLdouble) > buf.size()) THROW(EX_EOB);

	NLdouble d;
	readDouble(data, pos, d);
	return d;
}

void buffer_c::iterator::getBlock(char *buffer, int length) {


	char* data = buf.data();
	if (data == NULL) THROW(EX_NULL);
	if (pos + length > buf.size()) THROW(EX_EOB);

	readBlock(data, pos, buffer, length);
}

std::string buffer_c::iterator::getString() {

	char* data = buf.data();
	if (data == NULL) THROW(EX_NULL);
	//must have at least one byte left on buffer (the zero of zero terminated string)
	if (pos + (int) sizeof(NLubyte) > buf.size()) THROW(EX_EOB);

	char big_buffer[65535];
	readString(data, pos, big_buffer);
	return string(big_buffer);
}

NLushort buffer_c::iterator::get32K() {

	//at least one byte is there for sure!
	int b0 = (int) getByte();

	if (b0 < 0x80) return (NLushort) b0;
	else {

		int b1 = (int)getByte();
		b1 = b1 << 7;	// "make room" for b0's 7 bits
		int i = b0 + b1 - 128;	// FIXED: must subtract "128", the high bit of b0 that signals use of 2 bytes to encode the value
		return (NLushort) i;
	}
}

NLulong buffer_c::iterator::get256Ms() {

	int b0 = (int)getByte();

	int m = (b0 & 252) >> 2; // put bits 2..7 of coded byte into bits 0..5 of m

	// more bytes?
	if (b0 & 1) {

		int b1 = (int)getByte();

		m += (b1 & 254) << 5; // put bits 1..7 of coded byte into bits 6..12 of m

		// more bytes?
		if (b1 & 1) {

			int b2 = (int)getByte();
			m += (b2 & 254) << 12; // put bits 1..7 of coded byte into bits 13..19 of m

			// more?
			if (b2 & 1) {

				int b3 = (int)getByte(); // put all 8 bits 0..7 of coded byte into bits 20..27 of m
				m += b3 << 20;
			}
		}
	}

	// if signal bit is set: negative value
	if (b0 & 2)  
		m *= -1;

	return (NLulong) m;
}

const buffer_c::iterator& buffer_c::begin() const {
	return static_cast<const iterator&>(_begin);
}

const buffer_c::iterator& buffer_c::end() const {
	return static_cast<const iterator&>(_end);
}
