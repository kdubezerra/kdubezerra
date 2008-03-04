/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*

	PFILESYS : portable file system access functions
	- findfirst/findnext
	- file device/path/name/extension set/get/modify routines

*/

#include "zig/pfilesys.h"

#include <assert.h>
#include <stdio.h>

	//temporary code - for debugging
	//#include "../include/zig/console.h"
	//#include "../../zgun/conalleg.h"
	//extern conalleg_c *con;

using namespace std;

// ======== UTILITY FUNCTIONS ==============

//replacement for Allegro's put_backslash()
void allegro_put_backslash(char *s) {

	int l = strlen(s);

	//if zero-length, don't put nothing
	if (l == 0)
		return;

	//if last char is already some slash, return
	if ((s[l-1] == '/') || (s[l-1] == '\\'))
		return;

	//if windows, put \, else put /
#if defined WINDOWS || WIN32 || WIN64
	strcat(s, "\\");
#else
	strcat(s, "/");
#endif
}

//replacement for Allegro's get_filename()
char *allegro_get_filename(char *s) {

	int len = strlen(s);

	int i = len-1;

	//while pointing at a char (starts at last one)
	while (i > 0) {

		if ((s[i] == '\\') || (s[i] == '/') || (s[i] == ':')) {
			//found a stopper. name starts at the previously tested char (i+1)
			return (char *)(& (s[i+1]) );
		}

		i--;  //next char
	}

	//everything is the file name, we suppose!
	return s;
}

//replacement for Allegro's get_extension()
char *allegro_get_extension(char *s) {

	int len = strlen(s);

	int i = len-1;

	//while pointing at a char (starts at last one)
	while (i > 0) {

		if ((s[i] == '\\') || (s[i] == '/') || (s[i] == ':')) {
			//found dir separator. so, no extension!
			return (char *)(& (s[len]) );		//pointer to the "0" of the string
		}

		if (s[i] == '.') {
			//found first dot. extension starts at the previously tested char (i+1)
			return (char *)(& (s[i+1]) );
		}

		i--;  //next char
	}

	//didn't find any dots, so there is no extension!
	return (char *)(& (s[len]) );		//pointer to the "0" of the string
}

// ======== FILE_C ==============

//default ctor
file_c::file_c() {
	path = name = ext = "";
	//attrib = 0;
	size = 0;
}
	
//ctor
file_c::file_c(string spec, bool force_path_only) { set(spec, force_path_only); }

//set new path/file/ext
void file_c::set(string spec, bool force_path_only) {

	// parse "spec" into path/name/ext/ etc.
	char cspec[4096];
	strcpy(cspec, spec.c_str());		//c-str of full spec

	// if force_path, then be sure to add a path-separator to the end of the spec. that
	// will make the rest of the parser understand that the last component is part of the
	// path and we have an empty file name.
	if (force_path_only)
		allegro_put_backslash(cspec);

//	con->printf("SET: cspec = (%s)\n", cspec);

	char *cfn = allegro_get_filename(cspec);	//the filename 

//	con->printf("SET: cfn = (%s)\n", cfn);

	filename = cfn;

//	con->printf("SET: filename = (%s)\n", filename.c_str());

	char *cext = allegro_get_extension(cspec);	//the extension
	ext = cext;

	//find just the name (no extension). ASSUMING a 1-char separator (that's pretty reasonable, I guess...)
	//if ext == "", 
	// then name = filename
	// else we put a zero on the character before the extension pointer
	if (cext[0] == '\0')
		name = cfn;
	else {
		cext[-1] = '\0';
		name = cfn;
	}

	//find just the path (no filenames)
	//if filename == ""
	// then path = cspec
	// else we put a zero on the first character of the filename
	// BE SURE THAT the "path" string has a trailing separator, so we can concatenate names.
	if (cfn[0] == '\0') {
		allegro_put_backslash(cspec);
		path = cspec;
	}
	else {
		cfn[0] = '\0';
		path = cspec;
	}
}

//dtor
file_c::~file_c() {
	// REVIEW : nada aqui?
}

//copy ctor
file_c::file_c(const file_c &f) {

	path = f.path;
	filename = f.filename;
	name = f.name;
	ext = f.ext;
	//attrib = f.attrib;
	size = f.size;
}

//assignment operator
file_c& file_c::operator=(const file_c &f) {
	
	if (this != &f) {
		path = f.path;
		filename = f.filename;
		name = f.name;
		ext = f.ext;
		//attrib = f.attrib;
		size = f.size;
	}
	return (*this);
}


//create a sub-directory of the current path. example, if this get_path() equals
//"c:\somedir" then make_dir("subdir") makes the directory "c:\somedir\subdir"
//returns true on success.
bool file_c::make_dir(string dirname) {
	
	string all = path + dirname;

#if defined WINDOWS || WIN32 || WIN64
	int res = _mkdir(all.c_str());
#else
	int res = mkdir(all.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

	if (res == 0)
		return true;
	else
		return false;
}

//remove directory (works on windows and UNIX standard v3)
bool file_c::remove_dir(string dirname) {

	string all = path + dirname;

#if defined WINDOWS || WIN32 || WIN64
	int res = _rmdir(all.c_str());
#else
	int res = rmdir(all.c_str());
#endif

	if (res == 0)
		return true;
	else
		return false;
}

//change file name/ext over the same path
void file_c::replace_filename(string filename) {

	this->filename = filename;

	//change name and ext
	char cfn[4096];
	strcpy(cfn, filename.c_str());		//c-str of full spec
	
	char *cext = allegro_get_extension(cfn);
	ext = cext;

	if (cext[0] == '\0')
		name = cfn;
	else {
		cext[-1] = '\0';
		name = cfn;
	}
}

//change the path over the same filename
void file_c::replace_path(string newpath) {

	path = newpath;

	//add the slash if not there yet
	char cpath[4096];
	strcpy(cpath, path.c_str());		//c-str of path

	allegro_put_backslash(cpath);

	path = cpath;
}


//append another directory to the path
void file_c::cd(string dir) {

	char cpath[4096];
	strcpy(cpath, path.c_str());		//c-str of full spec

	allegro_put_backslash(cpath);

	strcat(cpath, dir.c_str());

	allegro_put_backslash(cpath);

	path = cpath;
}

//delete file; see make_dir(). returns true on success.
bool file_c::delete_file(string fn) {

	if (fn == "")
		fn = path + filename;
	else
		fn = path + fn;

	int res = remove( fn.c_str() );

	if (res != 0)
		return false;	//failed

	return true; // ok!
}

// ======== FILE_FINDER_C ==============

file_finder_c::file_finder_c() {

	//just giving nice defaults - paranoia
	//r = 0;
	//mask = "";
	exclude_non_directories = false;
	exclude_weirdos = false;
	exclude_directories = false;
	//exclude_hidden = false;
	valid_result = false;
	first_find = false;

#if defined WINDOWS || WIN32 || WIN64

	hFind = INVALID_HANDLE_VALUE;

#else

	dir = 0;

#endif
}

file_finder_c::~file_finder_c() {

#if defined WINDOWS || WIN32 || WIN64

	if (hFind != INVALID_HANDLE_VALUE) {
		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

#else

	if (dir != 0) {
		closedir(dir);
		dir = 0;
	}

#endif
}

//(re-)starts the search using some criteria (path / file name mask). if "files" is set
//to false, then files (non-directories) are excluded. if "dirs" is set to false,
//then directories are excluded. "weirdos" are the weird filesystem entries (like the "."
//and ".." directory references)
void file_finder_c::start_search(string spec, bool files, bool dirs, bool weirdos) {

	//keep non-dirs?
	exclude_non_directories = !files;

	//keep weirdos?
	exclude_weirdos = !weirdos;

	//keep directories?
	exclude_directories = !dirs;

	//for the first has_next() call
	first_find = true;

	//save the search dir
	file_c  tempf(spec);
	search_dir.set( tempf.get_path(), true );		//save only the path of spec, and search_dir's filename is empty

#if defined WINDOWS || WIN32 || WIN64

	// free previous search if any
	if (hFind != INVALID_HANDLE_VALUE) {
		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	// init internal search structures
	bContinue = TRUE;
	hFind = FindFirstFile(spec.c_str(), &data);

#else

	//UNIX:
	//closedir() - free previous search if any
	if (dir != 0) {
		closedir(dir);
		dir = 0;
	}

	// ****************************************
	//
	//  file_finder_c at least using UNIX implementation doesn't support
	//  a file mask - all files are searched!
	// 
	// ****************************************
	//opendir() - init internal search structures 
	// ******************************
	// get only the PATH part of spec to pass to opendir!
	// ******************************
	dir = opendir( search_dir.get_absolute().c_str() );

	// DO AN INITIAL readdir() call so we stay symmetrical to the Windows solution
	// do this only if opened dir sucessfully
	if (dir)
		dp = readdir(dir);

#endif

	//process whatever result from findfirst
	process_find_result();
}

//call this before next() to know if next() has anything to return. also,
//this is the function that actually iterates, so if you call next() 1000 times,
//you will get 1000 of the same result.
bool file_finder_c::has_next() {

	//if first find, then return the result from start_search
	if (first_find) {
		first_find = false;
		return valid_result;
	}

#if defined WINDOWS || WIN32 || WIN64

	// search next, set result & valid_result accordingly
	if (hFind != INVALID_HANDLE_VALUE)
		bContinue = FindNextFile(hFind, &data);

#else

	//UNIX - readdir()
	if (dir)
		dp = readdir(dir);

#endif

	//process find result
	process_find_result();
	
	return valid_result;
}

//returns the next result fetched from has_next(), from the started search. will return 
//garbage when has_next() returns false.
file_c file_finder_c::next() { return result; }

//PROTECTED -- process find result and search again if skipping directories and just found one.
void file_finder_c::process_find_result() {

#if defined WINDOWS || WIN32 || WIN64

	//if the search didn't come to a halt because of next attempt at iteration:
	if ((hFind != INVALID_HANDLE_VALUE) && bContinue) {

		//found a directory and skipping?
		// or 
		//found a non-directory and skipping?
		// or
		//found a "."/".." and skipping?

		if (
			   ((exclude_non_directories == true) && (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
				 ||
				 ((exclude_weirdos == true) && ((!strcmp(data.cFileName, ".")) || (!strcmp(data.cFileName, ".."))))
				 ||
				 ((exclude_directories == true) && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			 )
		{
			//find again, process... (recursive call)
			has_next();
		}
		//normal find
		else {
			
			valid_result = true;
			result = file_c(data.cFileName);
			//result.attrib = data.dwFileAttributes;
			result.size = data.nFileSizeLow;
		}
	}
	//no more files
	else {
		
		valid_result = false;
		result = file_c();
		//result.attrib = 0;
		result.size = 0;
	}

#else

	//if the search didn't come to a halt because of next attempt at iteration:
	if ((dir != 0) && (dp != 0)) {

		//resolve the filename now because...:
		result = file_c( (char *)dp->d_name );

		//...the target absolute filename:
		file_c targetf = search_dir; //the target file name for fopen()
		targetf.replace_filename( result.get_absolute() ); // result has only filename, no dirs, so get_absolute() works.

		//get file stats 
		struct stat     statbuf;
    
		//found a directory and skipping?
		// or 
		//found a non-directory and skipping?
		// or
		//found a "."/".." and skipping?
		if (
					(stat(targetf.get_absolute().c_str(), &statbuf) == -1)				//will fail and proceed to next entry if can't call stat() on it!
					||
					((exclude_non_directories == true) && (! (S_ISDIR(statbuf.st_mode)) )   )
					||
					((exclude_directories == true) && (S_ISDIR(statbuf.st_mode))  )
					||
					((exclude_weirdos == true) && ((!strcmp(result.get_absolute().c_str(), ".")) || (!strcmp(result.get_absolute().c_str(), ".."))))
			 )
		{
			has_next();
		}
		//normal find
		else {

			valid_result = true;
			//result = file_c( (char *)dp->d_name );  //moved up there
			//result.attrib = 0;
			result.size = statbuf.st_size;
			//result.size = -1;		//if anything goes wrong below, this is the default value

			// -- find out file size, important -- (this code works, tested)
			//FILE *f = fopen(targetf.get_absolute().c_str(), "rb"); //open the file
			//if (f != 0) {
			//	fseek(f, 0, SEEK_END);	//seek end
			//	result.size = ftell(f); //get current pos: done
			//}
		}
	}
	//no more files
	else {
		
		valid_result = false;
		result = file_c();
		//result.attrib = 0;
		result.size = 0;
	}

#endif
}


