/*

	conalleg - helper class for the minigame

	a console_c derivate for rendering on allegro bitmaps using the default allegro font

	you can tell that a variable or function call is a console method by the "this->" prefix (this
	is to help to tell what is Allegro (multimedia-lib specific) code and what is console_c or
	conalleg_c class code;

*/

#ifdef GAME_ALLEGRO

//allegro includes
#include <allegro.h>

#endif // GAME_ALLEGRO

//header include
#include "conalleg.h"

//your global console handle (probably you won't need more than one)
#ifdef GAME_ALLEGRO
conalleg_c *con = 0;
#else
console_c *con = 0;
#endif

//function for creating the global console handle. returns true of ok, false if console already created
bool create_console(console_cmdfunc_t cmdf, int size) {
	if (con)
		return false;
	else {
#ifdef GAME_ALLEGRO
		con = new conalleg_c(size, cmdf);
#else
		con = new console_c(size);
#endif
		return true;
	}
}

//function for deleting the global console handle. if console already deleted, does nothing
void delete_console() {
	if (con) {
		delete con;
		con = 0;
	}
}

#ifdef GAME_ALLEGRO

// ************** conalleg_c class methods implementation: ******************************


//no bitmap constructor. must pass bitmap later (call set_bitmap())
//cmdf is a pointer to a function that returns void and takes a single char* argument.
conalleg_c::conalleg_c(int size, console_cmdfunc_t cmdf) : console_c(size) {
	
	this->cmdfunc = cmdf;

	this->set_bitmap(0,0,0,0,0,0);	//see below
}

	//changes the drawing position, size, and bordersize of the console on the Allegro screen
void conalleg_c::set_draw_pos(int x, int y, int w, int h, int border) {
	
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->border = border;

	this->enable_display((w + 1 - border*2) / 8, (h + 1 - border*2) / 10);		//console_c method
}

//changes the target Allegro BITMAP* for rendering and also changes the drawing position, etc.
void conalleg_c::set_bitmap(BITMAP *b, int x, int y, int w, int h, int border) {
	
	bmp = b;
	if (bmp) {

		this->set_draw_pos(x,y,w,h,border);
		
		// *** CHANGE THIS TO ANY COLORS YOU LIKE BETTER ***
		bcol = makecol(50,50,50);			//makecol(R,G,B) = Allegro color maker function..
		tcol = makecol(255,255,255);
		pcol = makecol(0,255,0);

		text_mode(-1);		//Allegro

		this->draw_page(true);				//console_c method
	}
	else
		this->disable_display();			//console_c method
}

// render a console line. line: line offset onscreen. buf: line of text.
void conalleg_c::draw_line(int line, char *buf) {

	int col = tcol;
	if (line >= this->get_height() - 1) 
		col = pcol;
	textprintf(bmp, font, x + border, y + border + line * 10, col, buf);
}

// called when console must be redrawn.
void conalleg_c::clear() { 

	//Allegro code
	rectfill(bmp, x, y, x + w - 1, y + h - 1, bcol);
}

// called when console input must be redrawn.
void conalleg_c::clear_prompt() { 

	//Allegro code
	rectfill(bmp, 
		x + border, 
		y + border + (this->get_height() - 1) * 10, 
		x + border + this->get_width() * 8, 
		y + border + (this->get_height() - 1) * 10 + 8, bcol);
}

#endif // GAME_ALLEGRO

