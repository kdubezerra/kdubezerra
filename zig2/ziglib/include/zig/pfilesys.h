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

/*! \file pfilesys.h
    \brief Somewhat portable functions for performing a few filesystem tasks.
*/

#ifndef _ZIG_HEADER_PFILESYS_H_
#define _ZIG_HEADER_PFILESYS_H_

//some internal #define's that control the implementation.
#include "zigdefs.h"

#include <string>
//using namespace std;

//includes for implementation
#if defined WINDOWS || WIN32 || WIN64
// WINDOWS includes
#include <windows.h>
#include <direct.h>
#else
//UNIX standard v3 includes
#include <sys/types.h>		//mdir()
#include <sys/stat.h>			//mdir()/stat()/fstat()
#include <unistd.h>				//rmdir()
#include <dirent.h>				//opendir()/closedir()/readdir()
#endif

/*! \brief Manages a string which represents a filesystem path and/or filename, with several methods for 
		manipulating the string or performing some filesystem operations on it.

		Ported for Windows and POSIX filesystem APIs.

		Example - Creating a log filename on the same directory where the application executable was launched:

		<pre>
		<a>// argv[] is from main() ...
		file_c exefile;
		exefile.set(argv[0]); // example: "c:\\mygame.exe"
		exefile.replace_filename("mylogfile.txt");
		<a>// exefile.get_absolute() now gives the absolute filesystem address for creation of the logfile.
		<a>// if argv[0] was "c:\\mygame.exe" then exefile.get_absolute() now returns "c:\\mylogfile.txt"
		</pre>

		@see file_finder_c
		@see log.h
*/
class file_c {
public:

	/*! \brief Default constructor: empty string.
	*/
	file_c();
	
	/*! \brief Constructor: file path/name is taken from the given string.

			\param spec A well-formed string which points to some filesystem path and/or filename
			\param force_path_only If the last component, if any is to be interpreted a directory or file name
			- \c true: a \c spec parameter of "/usr/bin" results in a path of "/usr/bin" and an empty file name
			- \c false: a \c spec parameter of "/usr/bin" results in a path of "/usr/" and a filename "bin"
	*/
	file_c(std::string spec, bool force_path_only = false);

	/*! \brief Destructor (empty).
	*/
	virtual ~file_c();

	/*! \brief Copy constructor.

			\param f Reference to source file_c object which is to be copied over \c this
	*/
	file_c(const file_c &f);

	/*! \brief Assignment operator.

			\param f Reference to source file_c object which is to be copied over \c this

			\return Value of the assignment expression (same as the value of the given parameter)
	*/
	file_c& operator=(const file_c &f);

	/*! \brief Creates a sub-directory from the current stored path.

			Example: if get_path() equals "c:\somedir", then make_dir("subdir") makes the directory "c:\somedir\subdir".

			\param dirname name of subdirectory to be created
			
			\return returns Result of mkdir operation
			- \c true: directory created.
			- \c false: directory not created.
	*/
	bool make_dir(std::string dirname);

	/*! \brief Removes a sub-directory from the current stored path. 

			Example: if get_path() equals "c:\somedir", then remove_dir("subdir") deletes the directory "c:\somedir\subdir".

			\param dirname name of subdirectory to be deleted

			\return returns Result of rmdir operation
			- \c true: directory deleted.
			- \c false: directory not deleted (not found, not empty, ...).
	*/
	bool remove_dir(std::string dirname);

	/*! \brief Deletes a file from the current stored path. 

			Example: if get_path() equals "c:\somedir", then delete_file("some.file") deletes the file "c:\somedir\some.file".

			\param filename name of file to be deleted
      
			\return returns Result of file remove operation
			- \c true: file deleted.
			- \c false: file not deleted (not found, no permission to delete, ...).
	*/
	bool delete_file(std::string filename = "");

	/*! \brief Replaces the filesystem path/name string.

			\param spec A well-formed string which points to some filesystem path and/or filename
			\param force_path_only If the last component, if any is to be interpreted a directory or file name
			- \c true: a \c spec parameter of "/usr/bin" results in a path of "/usr/bin" and an empty file name
			- \c false: a \c spec parameter of "/usr/bin" results in a path of "/usr/" and a filename "bin"
	*/
	void set(std::string spec, bool force_path_only = false);

	/*! \brief Replaces the filename (name and extension) of the string.

			The path remains unchanged.

			\param newfilename A new filename (examples: "newfile", "a_file.txt")
	*/
	void replace_filename(std::string newfilename);

	/*! \brief Replaces the path of the string.

			The filename (name and extension) remains unchanged.

			\param newpath A new path (examples: "c:\\", "/usr/bin")
	*/
	void replace_path(std::string newpath);

	/*! \brief Appends another directory to the current path.

			Example: if get_path() returns "/usr/", then cd("bin") will make get_path() now return "/usr/bin/".

			\param dir Directory name to be appended to the end of the current path string.
	*/
	void cd(std::string dir);
	
	/*! \brief Gets the full filesystem path and filename string.

			NOTE: This method name is actually a bit misleading. If, for instance, get_path() returns "../" and 
			get_filename() returns "somefile", then get_absolute() will return "../somefile", which is clearly 
			not an "absolute" filesystem path, but a relative one.

			\return Concatenation of internal path and filename strings.
	*/
	std::string get_absolute() { return (path + filename); }

	/*! \brief Gets the full filename (name of file with extension).

			\return Internal string with name and extension of the file.
	*/
	std::string get_filename() { return (filename); }

	/*! \brief Gets the name of the file, without extension.

			\return Internal string with name of file, without extension.
	*/
	std::string get_name() { return name; }

	/*! \brief Gets the path from the filesystem string.

			\return Internal string with just the path (device and/or directory names).
	*/ 
	std::string get_path() { return path; }

	/*! \brief 	Gets the extension of the file, without name.

			\return Internal string with extension of file, without name.
	*/
	std::string get_extension() { return ext; }

	/// The file size, for when file_c is used to return a result from file_finder_c
	int size;

private:

	//file attributes, when file_c is used to return a result from file_finder_c
	//int attrib;

	//root path, filename(name+'.'+ext), name, extension
	std::string path, filename, name, ext;
};

/*! \brief Searches a directory for files and sub-directories.

		Ported for Windows and POSIX filesystem APIs.

		Example:

		<pre>
		<a>// Prints a list of *.DLL files in directory /windows/system of the current drive:
		file_finder_c finder;
		finder.start_search("\\windows\\system\\*.DLL", true, false);
		while (finder.has_next()) {
			file_c result = finder.next();
			<a>//print the "result" file on the screen here...
		}
		</pre>

		\see file_c
*/
class file_finder_c {
public:

	/*! \brief Constructor.
	*/
	file_finder_c();

	/*! \brief Destructor (closes the underlying search thingies).
	*/
	virtual ~file_finder_c();

	/*! \brief Stars (or restarts) a search for files that match the given criteria.

			With this call, a file_finder_c object starts a search for files in a directory. The search 
			mask (directory plus filename mask) is given by the parameter \c spec. 

			To actually get the list of files that match the search, you must use the methods has_next() 
			and next().

			\param spec Path and filename mask.
			\param files If \c true, files (non-directory entries) are included; if \c false they are ignored.
			\param dirs If \c true, directories are included; if \c false they are ignored.
			\param weirdos If \c true, "." and ".." entries are included (possibly other exceptions too); 
			if \c false they are ignored.
	*/
	void start_search(std::string spec, bool files, bool dirs, bool weirdos = false);

	/*! \brief Checks if there is a next result for the current search.

			This method does the actual iteration, that is, it fetches the next entry in the result. However, 
			you can call it several times in a row because it only iterates if a call to next() has already 
			been made to fetch the next result.

			In other words, the sequence "has_next(); has_next(); next();" does exactly the same thing as the 
			sequence "has_next(); next();". The second has_next() call on the first example won't iterate.

      \return \c true if a subsequent call to next() will return the valid, next filename in the search
			results; or \c false if a subsequent call to next() would return garbage (no more search results).
	*/
	bool has_next();

	/*! \brief Gets the next search result from the current search, fetched by a previous has_next() call.

			When creating your loop to fetch search results, remeber to call has_next() one time and check 
			that it returns \c true before making one call to next().

			\return The valid, next filename in the search results, or garbage if the preceding has_next() call 
			returned false (in which case you shouldn't call next() anyway).
	*/
	file_c next();

private:

	//process find result. this can actually do a new search (find-next) if we're
	//skipping non-directories (exclude_non_directories == true)
	void process_find_result();

	//the search dir
	file_c search_dir;

	// Win32 internal search structure
#if defined WINDOWS || WIN32 || WIN64

	HANDLE hFind;						//a handle
	BOOL bContinue;					//a flag
	WIN32_FIND_DATA data;		//the search struct

#else

	//UNIX:
	DIR *dir;								//dir handle, if ==0 (NULL) then it's invalid
	dirent *dp;							//result of readdir()
            
#endif

	// if true, exclude non-directories from result
	bool exclude_non_directories;

	// if true, exclude directories from result
	bool exclude_directories;

	// if true, excludes weird entries like the "." and ".." directories
	bool exclude_weirdos;

	// the result fetched by "has_next()"
	file_c result;

	//bool value indicates wether the result variable is valid.
	bool valid_result;

	//if it's the first has_next() call after start_search();
	bool first_find;
};

#endif // _PFILESYS_H_

