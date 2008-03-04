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

	simple log facility

*/

#include "zig/zigdefs.h"
#include "zig/utils.h"
#include "zig/log.h"

#include <stdio.h>
#include <stdarg.h>

//ZIGLITE removed
//list of opened logs
//#include <map>
//using namespace std;

//our type
class logfile {
public:

	//ZIGLITE new: used log entry
	bool    used;

	char*		f1;
	char*		f2;
	int			cur;		//1=f1 2=f2
	FILE*		f;			//current logfile
	int			swapc;	//swap count
	int			s;			//the size of the current selected logfile
	int			maxs;
	bool		autolf;
	bool		rectime;
	double	itime;

	logfile() {	used = false;	}
};

//our globals!
//
//map<int, logfile> logmap;
//
//ZIGLITE: changed to static array due to user request. should not work much different if 
// we avoid allocating a large number of logs.
logfile logarray[ZIG_MAX_LOG_FILES];

//int logidgen = 0;
#define LOG_PATH_SIZE 256
char logroot[LOG_PATH_SIZE] = "";

//set root path for all logfiles 
void log_set_prefix(const char *logpath) {

	if (strlen(logpath) >= LOG_PATH_SIZE) {
		logroot[0]=0;
		return;
	}
	strcpy(logroot, logpath);
}

//open log. returns 0 if fails, returns positive log id if success
int log_open(const char *logname1, const char *logname2, int maxsize, bool autolinefeed, bool rectime) {

	// find opened log ID spot
	// ZIGLITE 1.1 FIXED: scans array slots from 1 to MAX instead of 0 to MAX. this fixes 
	//  the bug where 0 was a code for ERROR on log_open() but was also a possible log ID.
	int logidgen = -1;
	for (int lid = 1; lid < ZIG_MAX_LOG_FILES ; lid++)
		if (logarray[lid].used == false) {
			logidgen = lid;
			break;
		}
	
	// too many logs
	if (logidgen == -1)
		return 0;
	
	logfile lf;

	//ZIGLITE new
	lf.used = true;

	lf.autolf = autolinefeed;
	lf.rectime = rectime;
	lf.maxs = maxsize;
	lf.s = 0;
	lf.cur = 1;
	lf.swapc = 0;
	
	lf.f1 = new char[strlen(logname1)+1];
	strcpy(lf.f1, logname1);

	lf.f2 = new char[strlen(logname2)+1];
	strcpy(lf.f2, logname2);

	char tempz[4096];
	strcpy(tempz, logroot);
	strcat(tempz, lf.f1);
	lf.f = fopen(tempz, "w");
	if (lf.f == 0)
		return 0;			//can't open!

	//try to remove an older logfile 2nd-part
	strcpy(tempz, logroot);
	strcat(tempz, lf.f2);
	remove(tempz);
	
	//ZIGLITE removed
	//	logidgen++;
	lf.itime = get_time();

	//ZIGLITE: no map
	//logmap.insert( map<int, logfile>::value_type(logidgen, lf) );
  logarray[logidgen] = lf;
  
	log(logidgen, "LOGFILE CREATED: %.2f, MAXSIZE = %ik, SWAP# = %i\n\n", lf.itime, lf.maxs / 1024, lf.swapc);

	return logidgen;
}

//write to log
void log(int logid, const char *fs, ...)
{
	//vsprintf...
	va_list argptr;
	char msg[16384];
	va_start(argptr, fs);
	vsprintf(msg, fs, argptr);
	va_end (argptr);

	//findlog
	//ZIGLITE new
//	map<int, logfile>::iterator it = logmap.find(logid);
//	if (it == logmap.end())		//log not found
//		return;
//	logfile &lf = (*it).second;

	// ZIGLITE 1.1 FIX: logid 0 is no longer possible
	// check invalid log id
	if ((logid <= 0) || (logid > ZIG_MAX_LOG_FILES))
		return;

	logfile &lf = logarray[logid];
	if (lf.used == false)		// check unused log id
		return;

	//dolog
	if (lf.rectime) {
		double delta = get_time() - lf.itime;
		fprintf(lf.f, "%8.2f: ", delta);
	}
	fputs(msg, lf.f);
	if (lf.autolf)
		fputs("\n", lf.f);
	fflush(lf.f);

	//inc size, check logfile swap
	lf.s += (strlen(msg) + 1);
	if (lf.s > lf.maxs) {

		//close old
		fclose(lf.f);
		lf.s = 0;
		lf.swapc++;

		//open new in overwrite mode
		char tempz[4096];
		if (lf.cur == 1) {
			strcpy(tempz, logroot);
			strcat(tempz, lf.f2);
			lf.f = fopen(tempz, "w");
			lf.cur = 2;
		}
		else {
			strcpy(tempz, logroot);
			strcat(tempz, lf.f1);
			lf.f = fopen(tempz, "w");
			lf.cur = 1;
		}

		//check failure - try swapping again
		if (lf.f == 0) {

			//open new in overwrite mode
			if (lf.cur == 1) {
				strcpy(tempz, logroot);
				strcat(tempz, lf.f2);
				lf.f = fopen(tempz, "w");
				lf.cur = 2;
			}
			else {
				strcpy(tempz, logroot);
				strcat(tempz, lf.f1);
				lf.f = fopen(tempz, "w");
				lf.cur = 1;
			}
		}

		//failure again: close the log (safety net)
		if (lf.f == 0)
			log_close(logid);
		else {
			log(logid, "LOGFILE CREATED: %.2f, MAXSIZE = %ik, SWAP# = %i\n\n", lf.itime, lf.maxs / 1024, lf.swapc);
		}
	}
}

//close log
void log_close(int logid) {

	//ZIGLITE new
	//findlog
//	map<int, logfile>::iterator it = logmap.find(logid);
//	if (it == logmap.end())
//		return;
	//logfile &lf = (*it).second;

	// ZIGLITE 1.1 FIX: logid 0 is no longer possible
	// check invalid log id
	if ((logid <= 0) || (logid > ZIG_MAX_LOG_FILES))
		return;

	logfile &lf = logarray[logid];
	if (lf.used == false)		// check unused log id
		return;

	//close current
	if (lf.f != 0)
		fclose(lf.f);

	//v1.5.0 memleak fixed: delete logfile names - thanks to Milan Mimica & Valgrind
	if (lf.f1 != 0)
		delete[] lf.f1;
	if (lf.f2 != 0)
		delete[] lf.f2;

	//ZIGLITE new
	//remove logid
	//logmap.erase(it);
	lf.used = false;
}

