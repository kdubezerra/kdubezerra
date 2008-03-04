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

	plus an optional 'virtual port' value.

	This class also supports the optional 'virtual ports' feature, which allows an application 
	to tell apart two address_c objects which point to the same IPv4 address. This is done by 
	way of an additional int 'vport' value (virtual port), which just acts as an additional 
	application-level multiplexing device. It works the same way as IPv4 port values allow the 
	operating system to route incoming network traffic to the machine to the appropriate socket 
	(which is basically an incoming-message buffer for some process). See set_virtual_port(int) 
	for more details.
*/

#ifndef _ZIG_HEADER_ADDRESS_H_
#define _ZIG_HEADER_ADDRESS_H_

#include <nl.h>
#include <string>
#include "zigdefs.h"

/*! \brief A class that stores IPv4 socket addresses.

		This class is a wrapper which adds some convenience methods around the NLaddress type 
		defined by the HawkNL library.
*/

class buffer_c;

class address_c {
public:

  /*! \brief Creates an empty (invalid) address. */ 
	address_c();

  /*! \brief Creates the address object and then calls address_c::set_address(addr_spec) to 
			set the address represented by this object.

			\param addr_spec A string representing an IPv4 address in ether 'HOST', 'HOST:PORT', 'IP' or 'IP:PORT' format.
 
			\see set_address(std::string)
  */ 
	address_c(std::string addr_spec);

  /*! \brief Creates the address object, using the value of the NLaddress (HawkNL address) 'addr' to 
			set the address represented by this object.

			\param addr An IPv4 NLaddress object.
 
			\see set_address(NLaddress &)
  */ 
	address_c(NLaddress &addr);

  /*! \brief Creates the address object and sets it to the value of the given NLsocket's ('sock') 
			remote address.

			NLsocket objects, from the HawkNL library, have a property called 'remote address'. It points to 
			the network address of the last remote socket which has sent data to it. This constructor is useful 
			if, for instance, you have an UDP NLsocket which has received a message, and you want to find out the 
			socket address of the process that has sent you the message (assuming that in your application you 
			intend to communicate with several remote hosts through a single local UDP socket).

			\param sock An NLsocket, whose 'remote address' value will be used to set this address object's value.
  */
	address_c(NLsocket &sock);

  /*! \brief Copy constructor (construction plus assignment operation). Makes deep copy of parameter to 'this' object.

			\param addr The right-hand operand of type address_c on the assignment operation.
  */
	address_c(const address_c &addr);

  /*! \brief Destructor.
  */
	virtual ~address_c();

  /*! \brief Assignment operator. Makes deep copy of parameter to 'this' object.

			\param addr The right-hand operand of type address_c on the assignment operation.

			\return address_c The value of the expression, which is the same as the right-hand operand.
  */
	address_c &operator=(const address_c &addr);

  /*! \brief Assignment operator. Makes deep copy of parameter to 'this' object.

			\param addr The right-hand operand of type address_c on the assignment operation.

			\return address_c The value of the expression, which is either zero (false, not equal) or non-zero (true, equal)
  */
	int operator==(const address_c &addr) const;

  /*! \brief Less-than comparison operator. Allows the use of address_c objects as keys in std::map, for instance.

			\param addr The right-hand operand of type address_c on the less-than comparison.

			\return address_c The standard return values for operator<:
      - less than zero: 'this' (left-hand operator) is less than the parameter value (right-hand operator)
      - zero: both address_c objects have the same address value
      - greater than zero: 'this' (left-hand operator) is greater than the parameter value (right-hand operator)
  */
	bool operator<(const address_c &addr) const;

  /*! \brief Tries to find the most useful local IP address and assigns it to this.
 
	    Searches for a "public" (global) IP address first. If none is found, it uses any 
	    local address available. Returns true if a "public" IP address is found, false otherwise.

      \return 
      - \c true: Global IP address found (address routable on Internet at large).
      - \c false: Local IP address found (routable only on local network).
  */ 
	bool set_best_local_address();

  /*! \brief Checks if this address represents a valid socket address (returns 'true') or not (returns 'false').
 
      \return 
      - \c true: This is a valid address (usable).
      - \c false: This is an invalid address (unusable).
  */ 
	bool valid() const;

  /*! \brief Sets this address to the value of the NLaddress (HawkNL address) 'addr'.

			\param addr An IPv4 NLaddress object.
  */ 
	void set_address(NLaddress &addr);

  /*! \brief Evaluates the given IPv4 address specification (addr_spec) given as a string parameter and 
			stores the evaluated IP address value into this address object.

			If the string parameter specifies a host name, it is resolved by a call to HawkNL's function 
			nlGetAddrFromName()

			\param addr_spec A string representing an IPv4 address in ether 'HOST', 'HOST:PORT', 'IP' or 'IP:PORT' format.
  */ 
	void set_address(std::string addr_spec);

  /*! \brief Returns the IP address value as a string, in the format 'IP:PORT'. If the port value is zero, the 
			':PORT' suffix of the string is ommited.

			\return A socket address as a string, formatted as 'IP:PORT' or just 'IP' if the port value is zero.
  */ 
	std::string get_address() const;

  /*! \brief Returns the hostname associated with the IP address value, as a string in 'HOSTNAME:PORT' format. If the 
			port value is zero, the ':PORT' suffix of the string is ommited.

			This call will attempt to look-up, through a call to HawkNL's nlGetNameFromAddr(), a valid hostname for the 
			address value of this address_c object.

			\return 
			- A socket address as a string, formatted as either 'HOSTNAME:PORT' or 'HOSTNAME' (if port is zero)
			- An empty string ("") if any error occurs
  */ 
	std::string get_address_name() const;

  /*! \brief Sets the port value of this address.

			\param port Valid IPv4 port value between 0 and 65535
  */ 
	void set_port(int port);

  /*! \brief Gets the port value of this address.

			\return Valid IPv4 port value of this address, between 0 and 65535
  */ 
	int get_port() const;

// internal details -- do not document
// ignore set vport / get vport for the ZIG 2.x doxygen documentation -- this isn't supported nor is it 
// used. zigclient and zigserver will just filter out any vport values set to address_c instances.
#ifndef NO_DOXYGEN

  /*! \brief Sets the 'virtual port' (vport) value of this address, which by default is zero. 

			<B>IMPORTANT!</B>: zigclient_c and zigserver_c don't, at present, support vports. Neither 
			is need to, since there won't be multiple client connections coming out of the same client 
			socket.
	
			The vport is a way to tell identical IP socket adresses (IP:PORT) apart from each other for 
			when e.g. there is more than one logical connection between the same (IP1:PORT1) and the 
			same (IP2:PORT2). If you are using address_c to store regular IPv4 socket addresses then 
			just don't use get_virtual_port() and set_virtual_port(), and everything will keep working 
			as usual.

			Virtual port values are considered for all operators (==, =, <) implemented by this class. 
			For instance, identical IPv4 addresses A and B may fail an (A == B) test if objects A and B 
			have different vport values assigned to them.

			\param vport Virtual port value.
  */ 
	void set_virtual_port(int vport);

  /*! \brief Gets the 'virtual port' (vport) value of this address, which by default is zero. 

			<B>IMPORTANT!</B>: zigclient_c and zigserver_c don't, at present, support vports. Neither 
			is need to, since there won't be multiple client connections coming out of the same client 
			socket.

			\return vport Virtual port value.

			\see set_virtual_port(int)
  */ 
	int get_virtual_port() const;

#endif
  
  /*! \brief Gets the internal NLaddress of this address_c object.

			An address_c object uses an NLaddress object to store an address value internally.

			\return The internal NLaddress object, as-is.
  */ 
	NLaddress get_nl_address() const;

private: // ZIG 2.0: take out of doxygen docs

	// cached copy of the IPv4 address in unsigned int (32-bit) format for a quick "operator<".
	// if (iaddress.valid == NL_FALSE), this value is undefined/invalid also
  unsigned int ipv4_addr;

	// update ipv4_addr
	void iaddress_changed();

	// internal HawkNL NLaddress
	NLaddress iaddress;

	// virtual port, if any
	int vport;
};

#endif

