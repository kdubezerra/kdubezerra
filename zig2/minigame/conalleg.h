/*

	conalleg - helper class for the minigame

	a console_c derivate for rendering on allegro bitmaps using the default allegro font

*/

#ifndef _CONALLEG_H_
#define _CONALLEG_H_

#include "main.h"		//#defines GAME_ALLEGRO

//zig engine base console
#include <zig/console.h>

// console command parser callback type. argument is the command to be parsed
typedef void (*console_cmdfunc_t)(char*);

#ifdef GAME_ALLEGRO

//your global console handle (probably you won't need more than one)
class conalleg_c;
extern conalleg_c *con;

#else

extern console_c *con;

#endif

//function for creating the global console handle. returns true of ok, false if console already created
//size: size of console, in kilobytes
bool create_console(console_cmdfunc_t cmdf, int size = 64);

//function for deleting the global console handle. if console already deleted, does nothing
void delete_console();

#ifdef GAME_ALLEGRO

//allegro console class
class conalleg_c : public console_c {
public:

	//no bitmap constructor. must pass bitmap later (call set_bitmap())
	//cmdf is a pointer to a function that returns void and takes a single char* argument.
	conalleg_c(int size, console_cmdfunc_t cmdf);

	//interprete command: just call back user function supplied in the constructor.
	virtual void interprete_command(char *cmdstr) {	cmdfunc(cmdstr); }

	//changes the drawing position, size, and bordersize of the console on the Allegro screen
	void set_draw_pos(int x, int y, int w, int h, int border);

	//changes the target Allegro BITMAP* for rendering and also changes the drawing position, etc.
	void set_bitmap(BITMAP *b, int x, int y, int w, int h, int border);

	//changes only the target Allegro BITMAP* (call this if you are using something like "page 
	//flipping" where the current page for drawing is always changing)
	void just_set_bitmap(BITMAP *b) {	bmp = b; }

protected:

	// bitmap to draw
	BITMAP *bmp;

	//bcol = Allegro background color for the console
	//tcol = Allegro text color for the console output
	//pcol = Allegro text color for the console input prompt
	int bcol, tcol, pcol;	

	// x, y, width, height - for drawing into "bmp"
	int x, y, w, h, border;

	//callback for command processing
	console_cmdfunc_t cmdfunc;

	// render a console line. line: line offset onscreen. buf: line of text.
	virtual void draw_line(int line, char *buf);

  // called when console must be redrawn.
	virtual void clear();

  // called when console input must be redrawn.
	virtual void clear_prompt();
};

#endif // GAME_ALLEGRO

#endif // include guard


