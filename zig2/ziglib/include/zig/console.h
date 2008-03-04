/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*

		console_c

		basically, a manager for a list of strings, and a manager for a viewport that can locate a portion 
		of that list of strings.
        
*/

#ifndef _ZIG_HEADER_CONSOLE_H_
#define _ZIG_HEADER_CONSOLE_H_

#define CONSOLE_PRINTF_MAXPRINTMSGSIZE 4096

#include "log.h"

typedef struct { int start; int count; } tagLineIndexRecord;

// estrutura para retorno da busca de linhas no outbuffer
typedef struct { int charOfs, charCount, resultCode; } tagGetResult;

/*! \brief A generic text I/O console class. Must be subclassed to interface with input/rendering code.
	
		Class console_c is basically a manager for a list of strings. It supports the following main operations:

		- Text input: the console can receive keypresses captured by the application, and these are converted 
		into text characters which are appended to a prompt string which is managed by the console. When the 
		console receives an #13 or #10 character (CR or LF: "ENTER" or "RETURN" key pressed) the prompt string 
		is sent by the console to a command line handler defined by the application, and the prompt is emptied 
		so the user can type in a new command line.

		- Text output: the console has several "printf()"-like methods (like the LIBC version) which allow 
		the application to add strings to the list of strings managed by the console.

		- Viewport: the console has methods that can be called to control a viewport, that is, a subset of 
		the strings (for instance, 25 strings) that would show up on the screen for the user to see. For 
		instance, there are methods that allow to set the screen size (how many lines are visible) scroll up 
		and down one screen at a time, etc.

		How to use this class in your application (quick overview):

    - Extend class console_c and implement (override) the following methods (see their documentation for 
		more details):
			- virtual void draw_line(int line, char *buf)
			- virtual void clear()
			- virtual void clear_prompt()
			-	virtual void interprete_command(char *cmdstr)

		- Instantiate the class that extends console_c;

		- Call enable_log(int log_handle) if you want the console text I/O to be logged to a text file (see log.h 
			for instructions on opening a log file). This is optional but is nice.

		- Call enable_display(int cols, int rows) at least once to set the size of your viewport on the screen;

		- Put a call to the console's draw_page() method in a loop that is called frequently (for instance, in the 
		main loop of your game engine) so that the console can redraw itself to the screen, if needed. 
		
		- If the console is not supposed to be showing on the screen (for instance, it is hidden) you can either:
			- Just don't call draw_page() while the console is hidden, or
			- Call disable_display() to deactivate the draw_page() calls, remembering to call either enable_display() 
			or enable_display(int, int) so that draw_page() calls will take effect again.
		
		- To force a console redraw to the screen, call draw_page(true);

		- To enable console input (read what user is typing), find a way to capture user keypresses (depends on 
		what API you have for reading user input, e.g. Allegro, ...) and translate them into calls to, at least, 
		read_char(), and optionally, to scroll_page_up(), scroll_page_down(), scroll_all_down(), history_back() 
		and history_forward();

		- Call printf() to print messages on the console. Printing messages, like user input, forces the console 
		to redraw itself to the screen on the next call to draw_page();

		- Call xprintf() to print messages which have a priority attached to them. The console's priority can then 
		be adjusted in run-time, by a call to set_xprintf_level(), to filter (discard) more or less messages that 
		are being printed with xprintf().

		Caveats and tricks:

		- Static method get_default_console() returns a default console_c instance which can be used when you need 
		to pass a console_c instance to the constructor or method of some class and you really don't care what 
		messages this class might generate.

		Examples:

		- The "minigame" application that comes with ZIG distributions has a conalleg_c class (implemented in 
		conalleg.cpp and conalleg.h) which extends this console_c class. conalleg_c is a simple console that 
		knows how to draw itself to the screen using the Allegro multimedia library (http://alleg.sf.net). 

		- The "minigame" application itself works as a complete example on how to use the console. The "minigame"
		code is bound to Allegro, so the application uses Allegro to capture user keypresses and send them to the 
		console.
*/
class console_c
{
private: // don't show on doxygen ... subclasses won't be needing to see this

	// con: dados gerais do console
	//
	bool  conSizeEverSet;		// if true, either console_c(int,int,int) or enable_display(int,int) were called at least once
	bool	conDisplay;				// se false, significa que console nao tem onde se desenhar atualmente
	int		conCols;		      // largura do display do console, em colunas
	int		conRows;		      // altura do display do console, em linhas

	bool	conInputRedrawFlag;	// linha de input alterada, necessita redesenho (na página virtual)
	bool	conOutputRedrawFlag;	// linha de output alterada, necessita redesenho (na página virtual)
	bool	conDeveloperMode; 		// flag: imprime mensagens de debug (DPrintf)?

	int		conLogHandle;			//log handle

	// output: buffer de saída (mensagens do console)
	//
	char*	outBuffer;   // buffer de saída do console
	int		outSize;     // tamanho em bytes alocado para o buffer de saída
	int		outStart;	 // ponteiro relativo p/ o 1o caractere (valido) do buffer. -1 = vazio
	int		outEnd;		 // ponteiro relativo p/ o ultimo caractere (valido) do buffer. -1 = vazio
	int		outCount;	 // contador de caracteres usados
		
	int		outRows;		   // nº linhas reservadas para output (= conRows - 1)
	bool	outIndexValid; // TRUE=pode desenhar direto FALSE=necessita scan
	bool	outScrolled;   // TRUE=deu pageups FALSE=grudado no fim do console
	int		outPageStart;	// offset do primeiro caractere (visível)
	int		outPageEnd;	   // offset do último caractere (visível)

	tagLineIndexRecord* outLineIndex; // vetor de informações sobre as linhas visíveis, tipo um "cache"

	// input: digitação de comandos
	//
	char*	inBuffer;	// buffer de entrada
	int		inSize;		// tamanho em bytes alocado
	int		inCount;		// contador de caracteres usados

	// history: historico de comandos digitados na entrada
	//
	char**	hisList;    // lista de strings
	int			hisSize;    // numero maximo de strings (capacidade) da history
	int			hisSpaceLeft;// numero de strings ainda nao preenchidas na history
	int			hisFirst;   // proxima string a ser mostrada quando acessar history
	int			hisLast;    // posicao da ultima string (onde escreve a proxima entrada)
	int			hisCurrent; // posicao corrente do history sendo acessada

	// busca a próxima linha no buffer
	// parâmetros:
	//	  firstCharOffset: primeiro caractere da linha atual
	//   maxWidth: se -1: ignorar largura do console, senão, a funcao para quando a linha completar maxWidth colunas
	// retorno:
	//   r.charPos = se (r.resultCode) charPos = posição do 1o caractere (início) da próxima linha encontrada
	//   r.charCount = numero de caracteres da linha ATUAL (não da próxima)
	//   r.resultCode = 0:falha (fim buffer) 1:ok, parou por \n 2: ok, parou por width
	tagGetResult get_next_line(int firstCharOffset, int maxWidth);

	// busca a linha anterior no buffer (este só para em "\n"s e fim do buffer!)
	// parâmetros:
	//   firstCharOffset: caractere onde inicia a busca
	//   alreadyAtEnd: TRUE se firstCharOffset já aponta para o último caractere
	//    da linha "anterior" em questão.  FALSE se ainda aponta para o início da
	//    linha seguinte
	// retorno:
	//   r.charPos = se (r.resultCode) charPos = posição do 1o caractere (início) da linha anterior encontrada
	//   r.charCount = numero de caracteres da linha ANTERIOR ENCONTRADA
	//   r.resultCode = 0:falha (não há linha anterior) 1:ok (achou)
	tagGetResult get_previous_line(int firstCharOffset, bool alreadyAtEnd);

	// current "level" for xprintf
	int xprintf_level;

	// Realiza varredura da tela atual visível (out), atualizando a estrutura outLineIndex
	void update_output_line_index();

	// Renderiza uma linha na tela
	void render_line(char* buffer, int firstCharOffset, int lineNumber, int charCount);

	// adiciona um comando 'a history list
	void history_add(char* inputstr);

	// altera o numero de linhas/colunas visiveis em uma pagina do console
	void set_con_rows(int newConsoleDisplayRows);
	void set_con_cols(int newConsoleDisplayColumns);

protected:

	/*! \brief Callback method that should parse a command string entered by the console user.

			This method is triggered by read_char() or read_string() calls made from the application code. 
			When a read_char() method is called with a CR (#13) or LF (#10) char value, then the string 
			currently on the console prompt is sent to this callback so that the application can parse and 
			execute the command string.

			If your console is output-only, then just provide an empty implementation.

			\param cmdstr Null-terminated string containing the command-line that was typed in by the user.

			\see read_char()
	*/
	virtual void interprete_command(char* cmdstr) { }

	/*! \brief Callback method that should draw, on the screen, a string from the visible part (viewport) of 
			the console.

			The console implementation knows how many lines of text are to be visible on the screen at once - call 
			that amount 'N'. So, when it knows that the console viewport must be redrawn on the screen, it issues 
			'N' calls to draw_line(), with the first parameter varying from 0 to N-1. The application must know 
			where and how, exactly, to draw those lines on the screen.

			Note (1): Before issuing those series of calls to draw_line(), the console calls clear() first.

			Note (2): The last line is the console input prompt.

			Note (3): Methods get_width() and get_height() give the current number of visible console columns and 
			rows, respectively.
	
			\param line The line number on the viewport, in range [0, N-1] where N is the number of visible lines 
			on the viewport as given in a previous call to enable_display()

			\param buf Null-terminated string containing the actual line of text to be drawn on the screen
	*/
	virtual void draw_line(int line, char *buf) { }

	/*! \brief Callback method that should fill the console area with its background picture or colors.

			An implementation of this method must erase, from the screen, any text previously drawn by calls to 
			draw_line(). For instance, an implementation of this method can draw a filled rectangle of some 
			predefined 'console background' color that fills the portion of the screen where the draw_line() 
			callbacks draw the text.

			This method is called whenever the console text output area must be redrawn.

			Note: Methods get_width() and get_height() give the current number of visible console columns and 
			rows, respectively.

			\see clear_prompt()
	*/
	virtual void clear() { }

	/*! \brief Callback method that should fill the console 'input prompt' area with its background 
			picture or colors.

			An implementation of this method must erase, from the screen, the last visible line of text that 
			is drawn with a call to draw_line(). For instance, an implementation of this method can draw a 
			filled rectangle of some 			predefined 'console background' color that fills the portion of the 
			screen where the last console line is drawn.

			This method is called whenever the console text input area must be redrawn.

			Note: Methods get_width() and get_height() give the current number of visible console columns and 
			rows, respectively. The expression (get_console_height() - 1) gives the index of the last line, 
			which is always the index of the console input prompt.

			\see clear()
	*/
	virtual void clear_prompt() { }

public:

	/*! \brief Constructor that allocates memory and also calls enable_display().

			\param outputBufferSizeInKilobytes: int KBs of memory to allocate for the console. 64 is a good value
			\param consoleDisplayColumns: first parameter of the subsequent enable_display() call
			\param consoleDisplayRows: second parameter of the subsequent enable_display() call
	*/
	console_c(int outputBufferSizeInKilobytes, int consoleDisplayColumns, int consoleDisplayRows);

	/*! \brief Constructor that just allocates memory.

			You must call enable_display() later and provide the amount of columns and rows for your display.
	
			\param outputBufferSizeInKilobytes: int KBs of memory to allocate for the console. 64 is a good value
	*/
	console_c(int outputBufferSizeInKilobytes);

	
	/*! \brief Constructor that allocates a default amount of memory (hard-coded at 16 KB, currently).
	*/
	console_c();

	/*! \brief Destructor (frees all allocated memory).
	*/
	virtual ~console_c();

	/*! \brief Enables console display, configuring the number of available columns and rows on screen for 
			the console to draw its text output area, and also its input prompt (which is always drawn on the 
			last row).

			If you want to implement show/hide functionality on your console, you can call disable_display() 
			to hide the console, and enable_display() or enable_display(int, int) to show it again. 
			Alternatively, you can just stop calling draw_page() on your application's main loop when the 
			console is not supposed to be showing, and leave the console with the display enabled. 
			Both approaches will work.

			\param cols Number of columns of text available for the console (e.g.: 60)
			\param rows Number of rows of text available for the console (e.g.: 25)

			\see enable_display()
	*/
	void enable_display(int cols, int rows);

	/*! \brief Re-enables console display, after a call to disable_display().

			This call only works if the number of rows and columns of the console was already set up with a prior 
			call to either enable_display(int, int) or if the console object was instantiated with the 
			console_c(int, int, int) constructor. If this is not the case, then you should use 
			enable_display(int, int) instead.

			If the number of columns and rows of the console was not previously set, and this method is called, 
			then this method does nothing.

			\see enable_display(int cols, int rows)
	*/
	void enable_display();

	/*! \brief Disables console display, so that any further calls to draw_page() do nothing. 

			After a call to disable_display(), you must call enable_display() or enable_display(int,int) so that 
			draw_page() calls may again draw to the screen.

			\see enable_display()
	*/
	void disable_display();

	/*! \brief Returns the width of the console display, in characters.

			To change this value, use the console_c(int,int,int) constructor, or the enable_display(int,int) method.

			\return Current number of columns in the console viewport.
	*/
	int get_width() { return conCols; }

	/*! \brief Returns the height of the console display, in lines.

			To change this value, use the console_c(int,int,int) constructor, or the enable_display(int,int) method.

			\return Current number of rows in the console viewport.
	*/
	int get_height() { return conRows; }

	/*! \brief Enable (true) or disable (false) console dprintf() calls.

			\param devmode New value for this console's debug mode:
			- \c true: Calls to dprintf() are printed to the console.
			- \c false: Calls to dprintf() are ignored.
	*/
	void set_debug_mode(bool devmode) { conDeveloperMode = devmode; };

	/*! \brief Set the minimum "level" parameter for filtering xprintf() calls.

			Example: 
			<pre>
			set_xprintf_level(4); // level 4 is minimum
			xprintf(2, "This message (level 2) is not printed to the console.\n");
			xprintf(3, "This message (level 3) is not printed to the console.\n");
			xprintf(4, "This message (level 4) is printed.\n");
			xprintf(5, "This message (level 5) is also printed.\n");
			set_xprintf_level(3);
			xprintf(3, "Messages with level 3 are now printed!\n");
			xprintf(2, "But this is still not printed... :-(\n");
			</pre>

			\param level Any further xprintf() calls with a level value lower than this will be ignored (not printed).
	*/
	void set_xprintf_level(int level) { xprintf_level = level; }

	/*! \brief Enables logging of console input/output to the given log handle (see log.h).

			\param log_handle The log handle, given by a call to log_open()
	*/
	void enable_log(int log_handle);

	/*! \brief Disables logging of console input/output (default behavior).

			\see enable_log(int)
	*/
	void disable_log();

	/*! \brief Sends a character string to the console as output.

			Calling write_string(outstr) is the same as calling printf(outstr). This method is actually used by the 
			implementation of printf(). It works, but you should always use printf() instead.

			\param outstr Null-terminated string that is to be printed on the console output buffer.

			\see printf()
	*/
	virtual void write_string(const char* outstr);

	/*! \brief Sends a formatted character string to the console as output.

			This method works exactly as the LIBC printf method. Please check the LIBC printf documentation.
			
			Example:
			
			<pre>
			int foo = 47;
			printf("The answer is %i :-)\n", foo); // prints "The answer is 47 :-)"
			</pre>

			\param formatstr Format string.
			\param ... Values to replace the "%" marks inserted on the format string.
	*/
	void printf(const char* formatstr, ...); 

	/*! \brief If debug mode is enabled, sends a formatted character string to the console as output.

			\see set_debug_mode() and printf()
	*/
	void dprintf(const char* formatstr, ...);

	/*! \brief If the "level" parameter is greater or equal than the console's configured "level", then send 
			a formatted character string to the console as output.

			Messages printed with xprintf() have a priority attached to them, which is determined by the \c level 
			parameter. The console's priority is adjusted at run-time by calls to set_xprintf_level(). Any xprintf() 
			calls with a lower priority than the console's current configured priority are discarded.
			
      \see set_xprintf_level() and printf()
	*/
	void xprintf(int level, const char* formatstr, ...);

	/*! \brief Sends a character string to the console as input (typed by user or a script).

			This method just loops through all of the characters of <i>instr</i> and sends them
			to the console input, one at a time, through read_char(char,bool) calls.

			\param instr A string that is to be typed into the console prompt
			\param isScript Determines if the string is being typed by a human user or is being read from a script file:
			- \c true Input is from script: do not echo input on the output
			- \c false Input is from user: echo input on the output

			\see read_char()
	*/
	void read_string(const char* instr, bool isScript = false);

	/*! \brief Sends a single character to the console as input (typed by user or a script).

			This method sends the given character to the console's input prompt. 
			
			The following character values have special meanings:
			- 8: Erase last character (Backspace key)
			- 10 or 13: Execute command line and clear prompt (Enter/Return keys)
			- 27: Clear prompt (Esc key)

			Any other char values are added to the console input prompt as a text character.

			\param inchar A char that is to be typed into the console prompt
			\param isScript Determines if the char is being typed by a human user or is being read from a script file:
			- \c true Input is from script: do not echo input on the output
			- \c false Input is from user: echo input on the output
	*/
	void read_char(char inchar, bool isScript = false);

	/*! \brief Clear all console text (input and output).
	*/
	void clear_text();


	/*! \brief Conditionally redraws the console to the screen.

			This call draws the console's output viewport and input prompt to the screen, if the following 
			conditions are met:

			- Console display is currently enabled (see enable_display(int,int))
			- The input prompt is redrawn if its contents have changed since the last call to draw_page()
			- The output area is redrawn if its contents have changed since the last call to draw_page()

			Additionally, if the \c force parameter is set to \c true, then the last two conditions are 
			ignored, that is, input and output are redrawn even if their contents have not changed (but the 
			console display still must be enabled).

			This method calls the virtual methods of this class (namely clear(), clear_prompt(), and draw_line()) 
			to do the actual drawing to a screen, bitmap, or whatever 2D graphical context. Since the default 
			implementation of these methods in console_c do nothing, the console can only draw to a screen 
			if you extend class console_c and provide actual drawing code for them.

			\param force Forces (or not) a console redraw (see above).
	*/
	void draw_page(bool force = false);

	/*! \brief Scrolls the console output viewport up (by the console's display height, in rows).

			Usually, you will call this method when the PAGE UP key is pressed by the user.
	*/
	void scroll_page_up();

	/*! \brief Scroll the console output viewport down (by the console's display height, in rows).

			Usually, you will call this method when the PAGE DOWN key is pressed by the user.
	*/
	void scroll_page_down();

	/*! \brief Scroll the console output viewport all the way down to the latest message.

			Usually, you will call this method when the END key is pressed by the user.
	*/
	void scroll_all_down();

	/*! \brief Browses the history of command lines entered in the console, towards the older entries.

			Usually, you will call this method when the UP ARROW key is pressed by the user.
	*/
	void history_back();    

	/*! \brief Browses the history of command lines entered in the console, towards the newer entries.

			Usually, you will call this method when the DOWN ARROW key is pressed by the user.
	*/
	void history_forward();

	/*! \brief Returns a reference to a default (static) console instance. 

			This static method returns a default console_c instance which can be used at any time when you need to pass 
			a console_c instance to the constructor or method of some class and you really don't care what messages this 
			class might generate (think "/dev/null").

      \return A reference to a default console_c instance, for when you need one and don't have one.
	*/
	static console_c& get_default_console();

	/*! \brief Returns a pointer to a default (static) console instance. 

			This does the same as get_default_console(), but returns a pointer instead of a reference.

      \return A pointer to a default console_c instance, for when you need one and don't have one.

			\see get_default_console()
	*/
	static console_c* get_default_console_ptr() { return &get_default_console(); };

private:

	// set defaults (usado pelos ctors)
	void defaults();
};

#endif

