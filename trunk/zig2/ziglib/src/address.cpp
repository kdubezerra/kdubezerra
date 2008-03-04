/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*

	address_c : wrapper to a HawkNL "NLaddress"

*/

#include "zig/address.h"
#include "zig/utils.h"

using namespace std;

//default ctor = empty address
address_c::address_c() {
	vport = 0;
	iaddress.valid = NL_FALSE;
	iaddress_changed();
}

//ctor - calls set_address(addr_spec) internally.
address_c::address_c(string addr_spec) {
	vport = 0;
	set_address(addr_spec);
}

//ctor - build from existing NLaddress object
address_c::address_c(NLaddress &addr) {
	vport = 0;
	iaddress = addr;
	iaddress_changed();
}

//ctor - bult from an NL socket's REMOTE address (good for getting NL UDP broadcast results)
address_c::address_c(NLsocket &sock) {
	vport = 0;
	// can yield an invalid address, so check with valid() afterwards
	nlGetRemoteAddr(sock, &iaddress);
	iaddress_changed();
}

//copy ctor
address_c::address_c(const address_c &addr) {
	vport = addr.vport;		// COPY vport
	iaddress = addr.iaddress;
	iaddress_changed();
}

//dtor
address_c::~address_c() {
}

//operator=
address_c &address_c::operator=(const address_c &addr) {
	if (this != &addr) {
		vport = addr.vport;		// COPY vport
		iaddress = addr.iaddress;
		iaddress_changed();
	}
	return (*this);
}

//operator==
int address_c::operator==(const address_c &addr) const { 
	return 
		(
			(nlAddrCompare(&iaddress, &(addr.iaddress)) == NL_TRUE)
						&&
			(vport == addr.vport)		// ZIGLITE 1.1
		);
}

//operator<, so it can be used as a std::map<> key value
bool address_c::operator<(const address_c &addr) const {

	if (this->ipv4_addr < addr.ipv4_addr)
		return true;

	// if equal ipv4 addr, then try comparing port numbers
	if (this->ipv4_addr == addr.ipv4_addr) {

		if ( ((address_c *)this)->get_port() < ((address_c*)&addr)->get_port())
			return true;

		// ZIGLITE 1.1: if equal port address, try comparing vport numbers
		if ( ((address_c *)this)->get_port() == ((address_c*)&addr)->get_port()) {

			if ( ((address_c *)this)->get_virtual_port() < ((address_c*)&addr)->get_virtual_port())
				return true;
		}
	}

	// either they are fully equal or (this > other) somewhere
	return false;
}

// get the "best" local IP address (searches for a "public" address first, if none available, 
// use any local address) and store it in "this". returns true if a "public" IP address is 
// found, false otherwise.
bool address_c::set_best_local_address() {

	NLint count = 0;
	NLaddress *all = nlGetAllLocalAddr(&count);
	if (count == 0)
		return false;	// weird!
	iaddress = all[0];	// set first one found by default
	iaddress_changed();
	for (int i=0;i<count;i++) {
		if (check_private_IP(all[i]) == false) {
			iaddress = all[i];		//a non-private-class IP is found - set it and quit
			iaddress_changed();
			return true;
		}
	}
	return false;		//only "private" IP addresses found
}

//returns TRUE if this address represents a valid address. the address may be invalid if, for instance, 
//you passed garbage to set_address(). 
bool address_c::valid() const {
	return (iaddress.valid == NL_TRUE);
}

//set this address to point to a given NLaddress
void address_c::set_address(NLaddress &addr) {
	iaddress = addr;
	iaddress_changed();
}

//set the internal address to the speficied string. string may be in IP:PORT, HOSTNAME:PORT, IP or HOSTNAME 
//format. if the string specifies a host name, it will be resolved (nlGetAddrFromName). 
void address_c::set_address(string addr_spec) {
	nlGetAddrFromName(addr_spec.c_str(), &iaddress);
	iaddress_changed();
}

//gets the ip address in string form (IP:PORT), if port is zero, ommits ":PORT" suffix. 
//returns an empty string ("") if an error occurred. 
string address_c::get_address() const {
	char tmp[NL_MAX_STRING_LENGTH];
	if (nlAddrToString(&iaddress, (char *)tmp))
		return (string)tmp;	
	else
		return "";
}

//gets the hostname in string form (HOSTNAME:PORT), if port is zero, ommits ":PORT" suffix. this call 
//will attempt to look-up the hostname from the internal IP address (nlGetNameFromAddr). 
//returns an empty string ("") if an error occurred. 
string address_c::get_address_name() const {
	char tmp[NL_MAX_STRING_LENGTH];
	if (nlGetNameFromAddr(&iaddress, (char *)tmp))
		return (string)tmp;
	else
		return "";
}

//set address' port
void address_c::set_port(int port) {
	nlSetAddrPort(&iaddress, (NLushort)port );
}

//get address' port
int address_c::get_port() const {
	return (int)nlGetPortFromAddr(&iaddress);
}

// set address' virtual port (vport). the vport is a way to tell identical IP socket 
// adresses (IP:PORT) apart from each other for when e.g. there is more than one logical 
// connection between the same (IP1:PORT1) and the same (IP2:PORT2). if you are using 
// address_c to denote regular IP socket addresses then just don't use get_virtual_port() 
// and set_virtual_port() and everything will keep working as usual.
void address_c::set_virtual_port(int vport) {
	this->vport = vport;
}

// get the adress' virtual port (vport). see set_virtual_port() for comments.
int address_c::get_virtual_port() const {
	return vport;
}

//get the internal NLaddress
NLaddress address_c::get_nl_address() const {
	return iaddress;
}

//update ipv4_addr
void address_c::iaddress_changed() {

	unsigned int bytes[4];

	if (valid()) {

		string s = get_address();

		// parse string x.x.x.x 
		string::size_type lastpos = 0;
		for (int i=0;i<4;i++) {

			// lastpos == last "." or ":" found (0 if none)
			// pos == will hold the position of the next "." or ":" or end of string
			string::size_type pos = s.find('.', lastpos);
			if (pos == string::npos) {
				assert(i == 3);
				//no more dots (i==3 here), seek to end or to ':', whichever comes first
				pos = s.find(':', lastpos);
				if (pos == string::npos)
					pos = s.length(); // if no : then last addr byte goes all the way
			}

			string sub = s.substr(lastpos, pos-lastpos+1);

			bytes[i] = atoi(sub);
      
			lastpos = pos + 1;
		}

		// concatenate the bytes
    ipv4_addr = bytes[0] 
								+ (bytes[1] << 8)
								+ (bytes[2] << 16)
								+ (bytes[3] << 24)
								;
	}
	else // addr not valid, set to 0
		ipv4_addr = 0;
}

