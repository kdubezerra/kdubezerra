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

	assignment and copy ctor makes DEEP COPY

	debug the refcounted version + copy on write later, when this version is already working OK.

	...but as of march/2007: still nobody cares that buffer copy ctor and assignment are not copy-on-write...

*/

#ifndef _ZIG_HEADER_BUFFER_H_
#define _ZIG_HEADER_BUFFER_H_

#include "zigdefs.h"
#include <nl.h>

class serializable_c;

//define this to disable statistics collection on buffers
#define BUFFER_NO_STATS

// includes
#include "zigdefs.h"
#include "console.h"
#include <string>
using namespace std;

//this is the buffer's allocation unit. if big: less alocation, more wasted memory. if small, too much
//allocation, possibly less wasted memory
#define	BUFFER_CHUNK_SIZE 128


#ifndef BUFFER_NO_STATS

extern mutex_c cbuffer_mutex;
extern volatile int cbuffer_count;
extern volatile int cbuffer_totmem;
extern volatile int cbuffer_maxmem;

void check_cbuffer_maxmem(int yeah);

#endif

// shared buffer reg - a growable buffer
class buffer_t {
public:
	char *data;		//pointer to buffer
	int	alen;			//allocated length
	int ulen;			//used length
	buffer_t() {
		data = 0;
		alen = 0;
		ulen = 0;
	}
};

/*! \brief A flexible and powerful "memory stream" or "buffer" which supports several reading and writing 
			operators.

		Main features:

		- It implements a large number of methods for putting (writing) and getting (reading) primitive (HawkNL) 
			and object (serializable_c) types, both in explicit method form ( e.g.: <i>buffer.putByte((NLubyte) b);</i> ) 
			or directly in C++ stream operator form ( e.g.: <i>buffer << b;</i> )
		- It probably implements all the methods you'll ever need for managing and navigating the buffer;
		- It can compress and decompress its contents using the BZip algorithm (see buffer_c::zip() and 
			buffer_c::unzip())
		- It implements a copy constructor and an assignment operator that perform an immediate deep copy of 
			the source object on a target object.

		How it works: each buffer_c object keeps four key variables:

		- A char* pointer ("buf") to a chunk of memory on the heap, which it uses to store data. If this pointer is 
			NULL (0), then there is no memory allocated (yet) to store data;
		- An int ("alen") which stores the size of "buf" (the chunk of memory), in bytes. The size of the internal 
			chunk of memory is managed automatically by buffer_c as you put more data into it;
		- An int ("ulen") which counts the number of bytes of valid data that is stored on the buffer, counting from 
			the beginning of the internal char array. <i>Valid data</i> means all data that was inserted on the buffer 
			by any one of the "put" operations (e.g. putByte(), putBlock(), putObject() ...) and also some other methods 
			(e.g. assignment operator, wrap(), ...);
		- An int ("pos") which points to the read/write position in the buffer. This pointer is used both by 'get' 
			(read) and 'put' (write) methods. The value of "pos" should usually not be greater than the value of "ulen", but 
			"pos == ulen" is common, which probably means that the user is putting data into the buffer.

		<b>IMPORTANT</b>: When reading the documentation, remember that there are two meanings for "size". One is the 
		amount of memory allocated for the buffer, which is tracked by the "alen" internal counter. The other is the 
		amount of data that the user has written on the buffer ("valid data") which is tracked by the "ulen" internal 
		counter. 
		
		Example: a buffer may have (alen = 1024), that is, 1 Kilobyte of allocated memory, but (ulen = 4) if, say, the 
		user has only written a single int value on the buffer (and the other 1020 bytes of memory allocated by the 
		buffer_c object are currently unused).
*/ 
class buffer_c {
public:

	/*! \brief Default constructor, creates an empty buffer.

			This constructor does not allocate any memory to the buffer.

      \see buffer_c(int)      
	*/
	buffer_c();

	/*! \brief Constructor that allocates memory to the buffer up-front.

			\param size Buffer capacity, in bytes.

      \see buffer_c()
	*/
	buffer_c(int size);

	/*! \brief Copy constructor, copies the given buffer 'b' over this buffer.

			The copy constructor does a deep copy which is fully completed when the call is over (that is, 
			there is no copy-on-write optimization). <i>Deep copy</i> of buffers means that both objects 
			have their own independent, internal byte arrays, but they have the same data (values) on them. 
			The current read/write position is also copied, and any other internal state variables. The 
			objects are identical after the copy.

			\param b Source buffer object, whose state is to be copied over the state of this buffer object.

      \see buffer_c()
	*/
	buffer_c(const buffer_c &b);

	/*! \brief Assignment operator, copies the given buffer 'b' over this buffer.

			This operator has exactly the same effect as the copy constructor.

			\param b Source buffer object, whose state is to be copied over the state of this buffer object.

      \see buffer_c(const buffer_c&)
	*/
	buffer_c& operator=(const buffer_c &b);

	/*! \brief Destructor, doesn't leak!
	*/
	virtual ~buffer_c();

	/*! \brief Sets the given byte array that was previously allocated on the heap as the internal 
			byte array of this buffer_c object, which becomes responsible for freeing the byte array.

			The 'char *buffer' parameter is a pointer to a chunk of memory on the heap (byte array), 
			and the 'int buffer_size' parameter gives the size of that memory chunk.

			What this method does, in practice, is that it gives ownership of the given memory chunk 
			to this buffer_c object. Some other code has allocated the chunk, but is this buffer_c's 
			destructor that will be responsible for freeing it, after the call to this method.

			The given memory chunk is, therefore, used as the data container of this buffer_c object. 
			Also, whatever data was on the chunk is considered as valid contents of this buffer_c 
			object. So, this object now has 'buffer_size' bytes of valid data into it. If the data 
			is garbage, the caller can get rid of it with a subsequent call to buffer_c::clear(). 

			The read/write position of this object is not affected by this call.

			\param buffer Pointer to a previously allocated piece of memory on the heap.
			\param buffer_size The size of the 'buffer' parameter, in bytes.
	*/
	void wrap(char *buffer, int buffer_size);

	/*! \brief Deallocate all memory previously allocated by this buffer.

			This method deletes the internal byte array which this object uses to store the data.

			\see clear()
	*/
	void purge();

	/*! \brief Ensure minimum memory allocation to the internal data buffer.

			If this buffer_c currently has no internal data buffer (because e.g. purge() was just called) then 
			a new data buffer is allocated with at least 'capacity' bytes. If this buffer has an internal buffer 
			with more than 'capacity' bytes in size, this method does nothing. If this buffer has an internal buffer 
			with less than 'capacity' bytes in size, then a new internal buffer is allocated with at least 'capacity' 
			bytes, the contents of the old (smaller) buffer are copied into the new (bigger) buffer and, finally, the 
			old buffer is deallocated.

			\param capacity Minimum desired size of the internal data buffer, in bytes.

			\see purge()
	*/
	void ensure_capacity(int capacity);

	/*! \brief Get the current allocated capacity of the internal data buffer.

			\return Size of the internal data buffer, in bytes.

			\see ensure_capacity(int)
	*/
	int get_capacity() const;

	/*! \brief Reset contents of this buffer.

			This method simply resets the 'valid data counter' of this buffer_c object to zero (0). That is, this 
			method doesn't actually write zeroes or anything on the internal data buffer.

			This method also resets the read/write position to zero (0) since the buffer has no more valid data.

			\see purge()
	*/
	void clear();

	public: static buffer_c& getEmptyBuffer() {
		static buffer_c emptyBuffer;
		emptyBuffer.clear();
		return emptyBuffer;
	}
	
	// NEW ZIG2: new way to get data from a buffer!
	// It permits the const-correctness of buffer parameters in callbacks!
	class iterator {

		friend class buffer_c;

		public:            iterator(const iterator& it);
		private:           iterator(buffer_c& buf);

		public:  iterator& operator++(int);
		public:  iterator& operator++();
		public:  bool      operator==(const iterator& it) const;
		public:  bool      operator!=(const iterator& it) const { return !(*this == it); }

		/*! \brief Advances the read/write pointer's position by the given amount of bytes.
		*/
		public:  void      skip(const long amount);

		/*! \brief Reads an NLubyte (8-bit unsigned value in range [0..255]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next unsigned byte of valid data at the current read/write position.
		*/
		public: NLubyte    getByte();

		/*! \brief Reads an NLbyte (8-bit signed value in range [-128..127]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next signed byte of valid data at the current read/write position.
		*/
		public: NLbyte     getBytes();

		/*! \brief Reads an NLushort (16-bit unsigned value in range [0..65535]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.

			\return The next unsigned short of valid data at the current read/write position.
		*/
		public: NLushort   getShort();

		/*! \brief Reads an NLshort (16-bit signed value in range [-32768..32767]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next signed short of valid data at the current read/write position.
		*/
		public: NLshort    getShorts();

		/*! \brief Reads an NLulong (32-bit unsigned value in range [0..4294967295]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next unsigned long of valid data at the current read/write position.
		*/
		public: NLulong    getLong();

		/*! \brief Reads an NLlong (32-bit signed value in range [-2147483648..2147483647]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next signed long of valid data at the current read/write position.
		*/
		public: NLlong     getLongs();

		/*! \brief Reads an NLfloat (32-bit floating point value).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next float of valid data at the current read/write position.
		*/
		public: NLfloat    getFloat();

		/*! \brief Reads an NLdouble (64-bit floating point value).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next double of valid data at the current read/write position.
		*/
		public: NLdouble   getDouble();

		/*! \brief Reads a block of bytes of the given length and copies it on the specified memory location.

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.

			\param buffer Pointer to the memory area which will receive the bytes being read from this buffer.
			\param length Amount of bytes to be read from this buffer.
		*/
		public: void       getBlock(char *buffer, int length);

		/*! \brief Reads a null-terminated string from the buffer.

			This method reads chars (bytes) from the buffer until it finds a char with value zero (0). This 
			method is basically just a wrapper to HawkNL's <i>readString()</i> macro.

			\throws EX_EOB If the buffer has zero bytes left to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.

			\return STL string object containing the string that was read from the buffer.
		*/
		public: std::string getString();

		/*! \brief Reads an object whose class extends serializable_c. <b>IMPORTANT: The object's class MUST 
			have been previously registered by including the appropriate macro calls on the header and implementation 
			sections of the class (see the documentation of class serializable_c for details)</b>.

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
			\throws EX_INVALID_DATA No registered C++ class found for the Class ID that was read from the buffer.

			\return An object of the same class and with the same state that was previously written on this 
			buffer object with a call to putObject(), but cast to the generic supertype serializable_c.

			\see serializable_c
			\see serializable_c::read(buffer_c &)
		*/
		public: template <typename T> T* getObject()
		{
			T* obj = new T;
			obj->read(*this);
			return obj;
		}

		public: template <typename T> T& getObject(T& obj)
		{
			obj.read(*this);
			return obj;
		}

		/*! \brief Reads a "32K" object, which can fit into either 1 or 2 bytes, depending on the 
			actual value that is coded on the buffer.

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.

			\returns An int, set to the value previously written with put32K(int)

			\see put32K(int)
		*/
		public: NLushort get32K();

		/*! \brief Reads a "256Ms" object, which can fit into 1, 2, 3 or 4 bytes, depending on the
			actual value that is coded on the buffer.

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.

			\returns An int, set to the value previously written with put256Ms(int)

			\see put256Ms(int)
		*/
		public: NLulong get256Ms();

		private: int pos;
		private: buffer_c& buf;
	};

	public: const iterator& begin() const;

	public: const iterator& end() const;

	private: iterator _begin;

	private: iterator _end;

public:
	/*! \brief Set the read/write position to the specified value.

			The only validation applied to the 'ptarg' parameter is that if (ptarg < 0), then the read/write pointer 
			is set to zero (0) instead. Know what you are doing when using seek() with a parameter other than zero (0).

			Calling <i>seek(0);</i> is useful when you want to parse the contents of a buffer from its beginning, and 
			you want to be sure that you will be reading it from the beginning.

			\param ptarg The new read/write position, specified by a positive (or zero) distance value, in bytes, 
			from the beginning of the internal char/byte buffer.

			\see getpos()
			\see seek_end()
			\see skip(int)
			\see size()
	*/
	void seek(int ptarg);

	/*! \brief Set the read/write position to point to the next byte just after the last byte of valid data. In other 
			words, this method places the read/write pointer at the correct position for appending more data to the buffer.

      This method ensures that a subsequent "put" call won't overwrite any valid data already on the buffer, and 
			also ensures that a subsequent "put" call will put data at the end of the buffer (append).

			This method is just a shortcut of <i>seek( size() );</i>

			\see seek(int)
			\see size()
	*/
	void seek_end();

	/*! \brief Advances the read/write pointer's position by the given amount of bytes.

			This method is just a shortcut of <i>seek( getpos() + amount );</i>
	*/
	void skip(int amount);

	/*! \brief Returns the current read/write position of the buffer.

			\return The current read/write position, specified by a positive (or zero) distance value, in bytes, 
			from the beginning of the internal char/byte buffer.
	*/
	int getpos() const;

	/*! \brief Returns the current amount of valid data on the buffer.

			<i>Valid data</i> means all data that was inserted on the buffer by any one of the "put" operations 
			(e.g. putByte(), putBlock(), putObject() ...) and also some other methods (e.g. assignment operator, 
			wrap(), ...).

			The size of valid data is not the same as the actual amount of memory allocated internally by the 
			buffer object, which is returned by a call to get_capacity() instead.
	*/
	int size() const;

	/*! \brief Returns the current amount of valid data, in bytes, still available to be read by subsequent 
			"get" operations.

			This method returns the amount of bytes between the current read/write position and the last byte 
			of valid data on the buffer.

			This method is useful if, for instance, you want to call getLong() (which reads an NLulong from the 
			buffer) and you want to be sure that the call will succeed. So, in this case, you would call this 
			method and ensure that it returns a value of at least sizeof(NLulong) before calling getLong().

			This method is just a shortcut for the expression <i>( size() - getpos() )</i>.

			\see size()
	*/
	int size_left() const;

	/*! \brief Get a pointer to the internal byte array of this buffer.

			\return Pointer to the internal buffer which is allocated on the heap.
	*/
	char *data() const;

	/*! \brief Gets a pointer to the position, in the heap, of the byte, in the buffer, that is referenced by 
			the current read/write position pointer of this buffer_c object.

			This method is equivalent to <i>(char*)( data() + getpos() )</i>

			\return Memory pointer to the byte referenced by the read/write position pointer of this buffer_c object.
	*/
	char *data_cur() const;

	/*! \brief Advances the current read/write position and the current size (of valid data) on the buffer by 
			the given amount, in bytes.

			<b>WARNING #1</b>: Using this method is probably never a good idea.

			<b>WARNING #2</b>: If you try to advance the valid data counter past the size of allocated memory of 
			the internal buffer (returned by get_capacity()), the behavior is undefined.

			This method can be used if you have written data on the internal byte array of this buffer_c object 
			(by, for example, writing to the area of memory pointed by data_cur(), directly).

			\param amount Byte count to be added to the read/write pointer and the valid data counter.
	*/
	void manual_put_block(int amount);

	/*! \brief Reads an NLubyte (8-bit unsigned value in range [0..255]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next unsigned byte of valid data at the current read/write position.
	*/
	NLubyte getByte();

	/*! \brief Reads an NLbyte (8-bit signed value in range [-128..127]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next signed byte of valid data at the current read/write position.
	*/
	NLbyte getBytes();

	/*! \brief Reads an NLushort (16-bit unsigned value in range [0..65535]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.

			\return The next unsigned short of valid data at the current read/write position.
	*/
	NLushort getShort();

	/*! \brief Reads an NLshort (16-bit signed value in range [-32768..32767]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next signed short of valid data at the current read/write position.
	*/
	NLshort getShorts();

	/*! \brief Reads an NLulong (32-bit unsigned value in range [0..4294967295]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next unsigned long of valid data at the current read/write position.
	*/
	NLulong getLong();

	/*! \brief Reads an NLlong (32-bit signed value in range [-2147483648..2147483647]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next signed long of valid data at the current read/write position.
	*/
	NLlong getLongs();

	/*! \brief Reads an NLfloat (32-bit floating point value).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next float of valid data at the current read/write position.
	*/
	NLfloat getFloat();

	/*! \brief Reads an NLdouble (64-bit floating point value).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\return The next double of valid data at the current read/write position.
	*/
	NLdouble getDouble();

	/*! \brief Reads a block of bytes of the given length and copies it on the specified memory location.

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.

			\param buffer Pointer to the memory area which will receive the bytes being read from this buffer.
			\param length Amount of bytes to be read from this buffer.
	*/
	void getBlock(char *buffer, int length);

	/*! \brief Reads a null-terminated string from the buffer.

			This method reads chars (bytes) from the buffer until it finds a char with value zero (0). This 
			method is basically just a wrapper to HawkNL's <i>readString()</i> macro.

			\throws EX_EOB If the buffer has zero bytes left to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.

			\return STL string object containing the string that was read from the buffer.
	*/
	std::string getString();

	/*! \brief Reads an object whose class extends serializable_c. <b>IMPORTANT: The object's class MUST 
			have been previously registered by including the appropriate macro calls on the header and implementation 
			sections of the class (see the documentation of class serializable_c for details)</b>.

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
			\throws EX_INVALID_DATA No registered C++ class found for the Class ID that was read from the buffer.

			\return An object of the same class and with the same state that was previously written on this 
			buffer object with a call to putObject(), but cast to the generic supertype serializable_c.

			\see serializable_c
			\see serializable_c::read(buffer_c &)
	*/
	template <typename T> T* getObject()
	{
		T* obj = new T;
		obj->read(*this);
		return obj;
	}

	template <typename T> T& getObject(T& obj)
	{
		obj.read(*this);
		return obj;
	}

	/*! \brief Reads a "32K" object, which can fit into either 1 or 2 bytes, depending on the 
			actual value that is coded on the buffer.

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.

			\returns An int, set to the value previously written with put32K(int)

			\see put32K(int)
	*/
	int get32K();

	/*! \brief Reads a "256Ms" object, which can fit into 1, 2, 3 or 4 bytes, depending on the
			actual value that is coded on the buffer.

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.

			\returns An int, set to the value previously written with put256Ms(int)

			\see put256Ms(int)
	*/
	int get256Ms();

	/*! \brief Convenient operator version of getByte().

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
	*/
	buffer_c& operator>>(NLubyte &v);

	/*! \brief Convenient operator version of getBytes().

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
	*/
	buffer_c& operator>>(NLbyte &v);

	/*! \brief Convenient operator version of getShort().

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
	*/
	buffer_c& operator>>(NLushort &v);

	/*! \brief Convenient operator version of getShorts().

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
	*/
	buffer_c& operator>>(NLshort &v);

	/*! \brief Convenient operator version of getLong().

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
	*/
	buffer_c& operator>>(NLulong &v);

	/*! \brief Convenient operator version of getLongs().

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
	*/
	buffer_c& operator>>(NLlong &v);

	/*! \brief Convenient operator version of getFloat().

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
	*/
	buffer_c& operator>>(NLfloat &v);

	/*! \brief Convenient operator version of getDouble().

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
	*/
	buffer_c& operator>>(NLdouble &v);

	/*! \brief Convenient operator version of getString().

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
	*/
	buffer_c& operator>>(std::string &v);

	/*! \brief Convenient operator version of getObject().

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
	*/
	template <typename T> friend buffer_c& operator>>(buffer_c& buff, T& v);

	/*! \brief Reads a block of bytes from the buffer and sends it through the given HawkNL socket.

			This method is used internally by the library. ZIG users shouldn't need this.

			\throws EX_EOB If the buffer has zero bytes left to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.

			\return The result of the HawkNL nlWrite() call (please check with the HawkNL documentation):
			- Zero or greater than zero: the amount of bytes read from the buffer and sent through the socket
			- Less than zero: an error occurred
	*/
	int getDataToSocket(NLsocket sock, int length);

	/*! \brief Writes an NLubyte (8-bit unsigned value in range [0..255]).

			\throws EX_EOB Not enough data left on the buffer to be read (exception type: int).
			\throws EX_NULL No memory is allocated for this buffer object yet.
		
			\param i The unsigned byte to be written at the current read/write position.
	*/
	void putByte(NLubyte i);

	/*! \brief Writes an NLbyte (8-bit signed value in range [-128..127]).

			\param i The signed byte to be written at the current read/write position.
	*/
	void putBytes(NLbyte i);

	/*! \brief Writes an NLushort (16-bit unsigned value in range [0..65535]).

			\param i The unsigned short to be written at the current read/write position.
	*/
	void putShort(NLushort i);

	/*! \brief Writes an NLshort (16-bit signed value in range [-32768..32767]).

			\param i The signed short to be written at the current read/write position.
	*/
	void putShorts(NLshort i);

	/*! \brief Writes an NLulong (32-bit unsigned value in range [0..4294967295]).

			\param i The unsigned long to be written at the current read/write position.
	*/
	void putLong(NLulong i);

	/*! \brief Writes an NLlong (32-bit signed value in range [-2147483648..2147483647]).

			\param i The signed long to be written at the current read/write position.
	*/
	void putLongs(NLlong i);

	/*! \brief Writes an NLfloat (32-bit floating point value).

			\param i The float to be written at the current read/write position.
	*/
	void putFloat(NLfloat i);

	/*! \brief Writes an NLdouble (64-bit floating point value).

			\param i The double to be written at the current read/write position.
	*/
	void putDouble(NLdouble i);

	/*! \brief Writes the block of bytes specified by the parameters.

			\param buffer Pointer to the memory area which has the bytes to be written on this buffer.
			\param length Amount of bytes to be written into this buffer.
	*/
	void putBlock(char *buffer, int length);

	/*! \brief Writes an STL string to the buffer, encoded as a null-terminated string.

			This method is basically just a wrapper to HawkNL's <i>writeString()</i> macro.

			\param str STL string object to be written on the buffer.
	*/
	void putString(std::string str);

	/*! \brief Writes an object whose class extends serializable_c. <b>IMPORTANT: The object's class MUST 
			have been previously registered by including the appropriate macro calls on the header and implementation 
			sections of the class (see the documentation of class serializable_c for details)</b>.

			\param obj An object of a class that extends serializable_c and, ideally, overrides serializable_c::read() 
			and serializable_c::write().

			\see serializable_c
			\see serializable_c::write()
	*/
	void putObject(const serializable_c &obj);

	/*!	\brief Writes a "32K" object which can be later read by get32K(). The value must be in the range [0..32767].

			The advantage of using put32K() is that it uses only one byte to codify values in the range [0..127]. 
			The price to pay is that, using two bytes, the maximum value that can be encoded is in the range 
			[0..32767], which is half of what putShort() has, since put32K() uses one bit to determine if the 
			encoding will use one byte or two.

			<b>WARNING</b>: Passing a negative value or a value greater than 32767 as argument produces undefined behavior.

			\param i Integer value in range [0..32767]

			\see get32K()
	*/
	void put32K(int i);

	/*! \brief Writes a "256Ms" object which can be later read by get256Ms(). The value must be in the range 
			[-268435455..268435455].

			The advantage of using put256Ms is that it may use less than 4 bytes to write an int value:
			- Values in range [-63..0) or range [0..63] use only one byte;
			- Values in range [-64..-8191] or range [64..8191] use only two bytes;
			- Values in range [-1048575..-8192] or range [8192..1048575] use only three bytes;
			- Values in range [-268435455..-1048576] [1048576..268435455] use four bytes.

			The range of allowed values is smaller than that of an int because two bits are used to determine how 
			many bytes will be used in the encoding.

			<b>WARNING</b>: Passing an out-of-range value (not in [-268435455..268435455] ) as argument causes 
			undefined behavior. 

			\param i Integer value in range [-268435455..268435455]

			\see get256Ms()
	*/
	void put256Ms(int i);

	/*! \brief Reads a block of bytes from the given HawkNL UDP socket and writes it into the buffer. 
			The block must be at most 64Kb in size.

			This method is used internally by the library. ZIG users shouldn't need this.

			\return The result of the HawkNL nlRead() call (please check with the HawkNL documentation):
			- Greater than zero: the amount of bytes received from the socket and written on the buffer
			- Zero: please check with the HawkNL documentation, this value is kind of overloaded...
			- Less than zero: an error occurred
	*/
	int putDataFromSocket(NLsocket sock);

	/*! \brief Reads 'size' bytes from the given HawkNL TCP socket and writes it into the buffer.

			This method is used internally by the library. ZIG users shouldn't need this.

			\return The result of the HawkNL nlRead() call (please check with the HawkNL documentation):
			- Greater than zero: the amount of bytes received from the socket and written on the buffer
			- Zero: please check with the HawkNL documentation, this value is kind of overloaded...
			- Less than zero: an error occurred
	*/
	int putDataFromSocket(NLsocket sock, int size);

	/*! \brief Convenient operator version of putByte().
	*/
	buffer_c& operator<<(NLubyte v);

	/*! \brief Convenient operator version of putBytes().
	*/
	buffer_c& operator<<(NLbyte v);

	/*! \brief Convenient operator version of putShort().
	*/
	buffer_c& operator<<(NLushort v);

	/*! \brief Convenient operator version of putShorts().
	*/
	buffer_c& operator<<(NLshort v);

	/*! \brief Convenient operator version of putLong().
	*/
	buffer_c& operator<<(NLulong v);

	/*! \brief Convenient operator version of putLongs().
	*/
	buffer_c& operator<<(NLlong v);

	/*! \brief Convenient operator version of putFloat().
	*/
	buffer_c& operator<<(NLfloat v);

	/*! \brief Convenient operator version of putDouble().
	*/
	buffer_c& operator<<(NLdouble v);

	/*! \brief Convenient operator version of putString().
	*/
	buffer_c& operator<<(std::string v);

	/*! \brief Convenient operator version of putObject().
	*/
	buffer_c& operator<<(const serializable_c &v);

#ifndef ZIG_NO_BZIP2

	/*! \brief Compress a portion of this buffer, beginning at its current read/write position, into the 
			current read/write position of the buffer passed as parameter.

			<b>Currently, the library used to compress/decompress is BZIP2. ZIG is using BZIP2 version 1.02 
			from http://sources.redhat.com/bzip2 </b>
	
			Make sure you seek() to the desired position in both the source and the destination buffer. 

			\param dest The target buffer, which will receive the data from this buffer, in compressed form.
			\param amount Amount of bytes to read for compression. If this parameter is not specified (or if -1 
			is specified) then all bytes from the current read/write position, up to the last byte of valid data 
			on the buffer, are read and compressed into the destination buffer.

			\return An int value:
			- Zero or greater than zero: success; the return value is the number of bytes written to <i>dest</i>;
			- Less than zero: compression failed for any reason.
	*/
	int zip(buffer_c &dest, int amount = -1);

	/*! \brief Decompress a portion of this buffer, at its current read/write position, that was previously 
			compressed with zip(). The decompressed data is written into the current read/write position of the 
			buffer passed as parameter.

			<b>Currently, the library used to compress/decompress is BZIP2. ZIG is using BZIP2 version 1.02 
			from http://sources.redhat.com/bzip2 </b>

			Make sure that this buffer's read/write position is set to the beginning of a zip()'ed block, and 
			make sure that you seek() to the desired position at the destination buffer.

			\param dest The target buffer, which will receive the data from this buffer, in decompressed form.

			\return An int value:
			- Zero or greater than zero: success; the return value is the number of bytes written to <i>dest</i>;
			- Less than zero: decompression failed for any reason.
	*/
	int unzip(buffer_c &dest);
	
#endif // #ifndef ZIG_NO_BZIP2

	/*! \brief Prints the value of each byte of valid data of this buffer on a console_c object.

			\param con The console_c object that will receive the buffer dump in text form.
	*/
	void print(console_c* con) const;

	/*! \brief Public field used mainly as extra return value, when methods return a buffer_c object. */
	int code;

	/*! \brief Public field used mainly as extra return value, when methods return a buffer_c object. */
	int id;

	//not needed anymore
	// int internal;

private:

	//make buffer grow if needed, to make room for more "amount" bytes of data at "pos"
	void grow(long amount, bool writing = true);

	//makecopy (common part of copy ctor & operator=)
	void makecopy(const buffer_c &b);

	//buffer struct: NOT a handle (speeeed!!)
	buffer_t buf;

	//seek cursor in buffer
	long pos;
};

template <typename T> buffer_c& operator>>(buffer_c& buff, T& v)
{
	buff.getObject<T>(v);
	return buff;
}

/*! \brief Abstract class of all objects that can be written (serialized) to buffer_c objects, and also read 
		(de-serialized) from buffer_c objects.

		This class must be the ancestor class of all objects that are readable/writeable from/to buffer_c buffers. 

		The idea here is that a serializable object (an object whose class is a child of serializable_c) is first 
		written to a buffer_c buffer object. Since buffer_c objects are "THE" way to pack data for sending and 
		receiving through ZIG's communication primitives, then, with serializable_c and buffer_c working together 
		you can send and receive C++ objects through a ZIG connection!

		<hr>
		<b>VITAL INFORMATION BEGINS HERE: HOW TO REGISTER A CLASS FOR AUTOMAGIC SERIALIZATION</b>
		<hr>
		
		When subclassing from serializable_c (that is, when you do: <i>class MyClass : public serializable_c</i>) 
		you <B>MUST</B> include two pieces of code on your class. The first one on the definition (.H) of your 
		class, and the second one on the implementation module (.CPP) of your class.

		For instance, suppose that you are declaring a serializable class called MyClass.

		FIRST, on the header (.H) file, you must call the macro ZIG_SERIALIZABLE_CLASS_H() like this:
		
		<pre>
			class MyClass : public serializable_c {
			
			public:

				ZIG_SERIALIZABLE_CLASS_H ( MyClass );

				/ / ... rest of class definition here ...

			};
		</pre>

		SECOND, on the implementation (.CPP) file, you must call the macro ZIG_SERIALIZABLE_CLASS_CPP() like this
		(note that this call must be placed anywhere on the .CPP file that implements your class -- just make sure 
		that it is called in the global scope, and not included inside some method implementation, for instance):

		<pre>
			ZIG_SERIALIZABLE_CLASS_CPP ( MyClass );

			/ / ... rest of class implementation here ... 
		</pre>

		Without those macros, ZIG cannot automatically register your serializable class, and thus ZIG would 
		never be able to call the C++ primitive <i><b>new</b></i> on the correct class when de-serializing an 
		object of MyClass. See the documentation of read() for more details.

		\see buffer_c::putObject(const serializable_c &)
		\see buffer_c::getObject()
*/ 
class serializable_c {
public:
	
	/*! \brief Reads the contents of the data members of this object from the given buffer_c data stream.

			<b>WARNING #1:</b> <i>You should always override this method! serializable_c provides a default 
			implementation that does a simple bitwise memory copy. This isn't very safe and especially not 
			very portable!</i>
			
			<b>WARNING #2:</b> <i>The default implementation only supports objects that are under 32K in size. 
			If the object is larger, the result is undefined behavior!</i>

			This virtual method is called by the implementation of buffer_c::getObject(). What buffer_c::getObject() 
			does is this:

			- First, it reads a class identifier (CID) from the buffer_c stream and, using this CID, it calls 
				the C++ primitive <b><i>new</i></b> to create the C++ Class that is registered to this CID. This 
				is done automatically for you by ZIG so you don't have to worry about that. 
			
			The object that is <b><i>new</i></b>'ed by ZIG is an "empty", newly-created object whose data 
			members have the default values set by the default constructor of the class. But, what we want 
			to do is to read the object that is in the buffer_c stream, that is, an object whose state is 
			probably not the default one. 
			
			So, the second thing that is done by buffer_c::getObject() is:

			- Second, it calls serializable_c::read(buffer_c &) <b>(this method!)</b> to read from the 
				buffer the values that are to be assigned to the data members. Effectively, what happens is 
				that a clone of the C++ object that was sent is re-created on the receiving end.

			And so, this is how ZIG "sends" a C++ object through a network connection: 

			- Call seriarizable_c::write(buffer_c &) on the object so it writes its state, that is, the values 
				of its data members to a buffer_c, as a stream of bytes.
			- Send the stream of bytes (buffer) through the network
			- Receive the stream of bytes on the other end of the connection
			- Use C++ <b><i>new</i></b> over the appropriate type to create a "default" object of 
				the correct type, on the receiving end
			- Call serializable_c::read() on the "default" object, which knows how to update the value 
				of its own data members using the contents of the stream of bytes present in the 
				buffer_c object that was received, effectively re-creating the C++ object that was sent

			\param in A buffer_c object which contains, in "serialized form", the values that must be 
			set to this serializable_c object's data members.
			
			\see buffer_c::getObject()
	*/ 
	virtual void read(buffer_c &in);
	virtual void read(buffer_c::iterator &in);

	/*! \brief Writes the contents of the data members of this object to the given buffer_c data stream.

			<b>WARNING #1:</b> <i>You should always override this method! serializable_c provides a default 
			implementation that does a simple bitwise memory copy. This isn't very safe and especially not 
			very portable!</i>
			
			<b>WARNING #2:</b> <i>The default implementation only supports objects that are under 32K in size. 
			If the object is larger, the result is undefined behavior!</i>

			This virtual method is called by the implementation of buffer_c::putObject(const serializable_c &) 
			so that the object being serialized is responsible for writing itself on the buffer object.

			See the documentation of serializable_c::read(buffer_c&) above for a detailed explanation.

			\param out A buffer_c object where this serializable_c will write, in "serialized form", the value 
			of its data members to.
			
			\see serializable_c::read(buffer_c&)
			\see buffer_c::putObject(const serializable_c &)
	*/ 
	virtual void write(buffer_c &out) const;

	// this method is referenced by the magic macro ZIG_SERIALIZABLE_CLASS (see typereg.h). this is
	// one of the reasons why you need to include ZIG_SERIALIZABLE_CLASS(X) in class X to be 
	// serializable.
	virtual int ZIG_GetTypeId() const = 0;

	// this method is referenced by the magic macro ZIG_SERIALIZABLE_CLASS (see typereg.h). this allows
	// us to implement the "default serialization" mechanism (which is just a bitwise memcopy).
	virtual int ZIG_GetMySizeof() const { return sizeof(serializable_c); };

	// virtual destructor
	virtual ~serializable_c() { };
};

#endif // _ZIG_HEADER_BUFFER_H_

