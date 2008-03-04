/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*

	LOG

	simple logging facility

*/

#ifndef _ZIG_HEADER_LOG_H_
#define _ZIG_HEADER_LOG_H_

/*! \file log.h
    
		\brief Offers logging of text strings to text files.

		How to use this class in your application:

		- Call log_set_prefix() to set where all log files from now on will be opened and closed;

		- Call log_open() to open a log file. This method returns and int value which is a handle to the log file.

		- Call log() to write text to the log file. The log() method takes as its first argument the handle of the 
		log file which will receive the message. Pass, into this parameter, the value you got from log_open().

		- Call log_close() to close the log file.
		
		After opening a logfile you can set a console_c instance to log it's output to it.

		\see console_c::enable_log(int log_handle) which routes console output to a log file that is open

		\see file_c class which can help on specifying log file names and paths
*/

/*! \brief The default size limit for log files (512 KB). */
#define LOG_DEFAULT_SIZE 0x80000

/*! \brief Sets the root path for all log file names.

		The given path will be appended (as-is) to the log names passed to log_open(). So, it is important that the 
		path is correct and that it ends with a slash, so that a file name can be appended. You might want to use 
		class file_c to help you ensure this property in a portable way.

		\param logpath Null-terminated string containing the path prefix that is appended to all log names given 
		to log_open().
*/
void log_set_prefix(const char *logpath);

/*! \brief Opens a log file.

		The log.h logfiles have a maximum size which works as follows:
		- First, a logfile with logname1 is created
		- All log() messages go to the file logname1
		- When logname1 exceeds \c maxsize bytes, then a logfile with logname2 is created
		- All log() messages go to the file logname2
		- When logname2 exceeds \c maxsize bytes, then a logfile with logname1 is recreated again
		- All log() messages go to the file logname1 ... and so forth

    This makes sure that logs don't eat an arbitrary amount of disk space.

    If the opening of the log is successful, log_open() returns and int value which is a handle to the opened log. 
		This handle (Log ID) is given to log() as its first parameter, when you want to actually write to the log.

		\param logname1 First name of the logfile (starts with this).
		\param logname2 Second name used by the logfile (alternative name; first used when \c maxsize is first exceeded).
		\param maxsize Maximum size of one log file name (when exceeded, the log file name is switched).
		\param autolinefeed If \c true then a linefeed (<a>\n character) will be added after each call to log().
		\param rectime If \c true then the current time will be recorded in the file with every call to log().

		\return Returns 0 (zero) if the opening of the logfile fails. Returns a positive value (the <b>Log ID</b>, which 
		is then given to log() as its first parameter) if successful.
*/
int log_open(const char *logname1, const char *logname2, int maxsize = LOG_DEFAULT_SIZE, bool autolinefeed = true, bool rectime = true);

/*! \brief Write text to the specified log file.

		Besides the \c logid parameter, this method works exactly as the LIBC printf method. Please check the 
		LIBC printf documentation.
		
		\param logid The Log ID of the opened log (returned by a previous call to log_open()).
		\param fs Format string.
		\param ... Values to replace the "%" marks inserted on the format string.
*/
void log(int logid, const char *fs, ...);

/*! \brief Close an open log file.

		Does nothing if you pass an invalid logid.

		\param logid The Log ID of the log that is to be closed.
*/
void log_close(int logid);

#endif

