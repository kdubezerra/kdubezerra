/*

	main

	main source file for the mini-game project

*/

//include Allegro, ZIG, and other stuff
#include "main.h"

//include gameserver definition
#include "gameserver.h"

//include gameclient definition
#include "gameclient.h"

//server instance
gameserver_c *gameserver = 0;

//client instance
gameclient_c *gameclient = 0;

//portable filesystem access functions (file_c)
#include <zig/pfilesys.h>

//helper global variable: path to executable
file_c exefile;

//helper global variable: log handle
int l;

/*
//
// just some code to test the brand-spankin new c++ object serialization!
//
#include <zig/typereg.h>	//class "registry" support
#include <zig/buffer.h>		//buffer_c & serializable_c
class blah : public serializable_c {
public:
	ZIG_SERIALIZABLE_CLASS_H(blah); // MUST add this magical tag as member of the serializable class
	int a, b;     // define some fields to be persisted
	int whatever; // define some fields that are not persisted ("transient")
	blah() { a = b = 0; whatever = 333; }
	// to test the "default serialization" implementation, comment out the two methods below
	virtual void write(buffer_c &out) const { // IMPORTANT: from ZIG v1.4.0 and up, don't forget the "const"!
		out.putLongs(a); //"Longs" = NLlong, signed long, int
		out.putLongs(b);
	}
	virtual void read(buffer_c &in) {
		a = in.getLongs();	//use same order that was used for write
		b = in.getLongs();	
	}
};
ZIG_SERIALIZABLE_CLASS_CPP(blah); // MUST add this magical tag in a .CPP module (where the class is implemented probably)
*/


//helper class: a simple command-line parser
class cmdline_parser_c {
public:
	cmdline_parser_c(char *cmdl) { 
		strcpy(cmdline, cmdl); 
		scan = (char*)cmdline; 
		thecmd = (char*)cmdbuf;
	}
	char *cmd() { return thecmd; }
	bool next() { return (getcmd(scan,thecmd,false) > 0); }		//"getcmd()" is in ZIG library's "utils.h"
	bool is(char *cmp) { return !strcmp(thecmd, cmp); }
protected:
	char *scan;
	char cmdbuf[1024], cmdline[1024];
	char *thecmd;
};

//this function is called when a command is entered in the debug console
void console_command_f(char *command_string) {
	
	//use the parser above
	cmdline_parser_c cmd(command_string);
	if (cmd.next() == false) return;    // if no commands, return

	//open server
	if (cmd.is("/os")) {

		//start the server at GAME_PORT, accepting a maximum of 256 simultaneous players.
		//NEW: running in non-blocking mode. it is somewhat analogous to run_async(), but run_non_blocking() does
		// not create a new thread to run the server loop: you have to poll the server main loop by periodically
		// calling zigserver->process_messages() (see below in main()'s loop)
		gameserver->start( GAME_PORT, 256 );
	}
	//close server
	else if (cmd.is("/cs")) {

		//stops the server (if there is not a pending run_async() or run_non_blocking() for the server, does nothing)
		gameserver->stop();

	}
	//connect to a server
	else if (cmd.is("/c")) {

		//get parameter of the "/c" command, use the localhost by default
		address_c addr;
		if (cmd.next())
			addr.set_address(cmd.cmd());
		else
			addr.set_address("localhost");

		addr.set_port(GAME_PORT);
		
		//start connection to the given IP address
		if (!gameclient->connect(addr))
			con->printf("ERROR: CAN'T CONNECT TO : '%s'\n", addr.get_address().c_str());
		else
			con->printf("CONNECTING TO : '%s'\n", addr.get_address().c_str());
	}
	//disconnect from a server
	else if (cmd.is("/d")) {

		//disconnect (if was connected)
		gameclient->disconnect();
	}
	//request server information
	else if (cmd.is("/si")) {

		//get parameter of the "/si" command, use the localhost by default
		address_c addr;
		if (cmd.next())
			addr.set_address(cmd.cmd());
		else
			addr.set_address("localhost");

		addr.set_port(GAME_PORT);

		//request server info
		if (addr.valid()) {
			gameclient->get_server_info(addr);
			con->printf("SENT SERVER INFO REQUEST TO: '%s'\n", addr.get_address().c_str());
		}
		else
			con->printf("INVALID ADDRESS: '%s'\n", cmd.cmd());
	}
	//ask whole LAN for server information
	else if (cmd.is("/li")) {

		//request server info to all LAN
		gameclient->get_local_server_info();
		con->printf("SENT LAN SERVER INFO REQUEST\n");
	}
	//change xprintf level
	else if (cmd.is("/xp")) {
		
		if (cmd.next())
			con->set_xprintf_level( atoi( cmd.cmd() ) );
	}
	//NEW: anything else is either an invalid command or a chat message (if first char is not "/")
	else {
		
		// first char is '/' ? Then it is an invalid command which wasn't recognized above!
		if (cmd.cmd()[0] == '/') {
			con->printf("invalid command: %s\n", cmd.cmd());
		}
		// it is a chat message: forward to our gameclient, which will send it to the gameserver, if connected.
		else {
			gameclient->chat_string_from_console( command_string );
		}
	}
}

//initialize all stuff
//here is where we put all the initialization crap we need
bool app_initialize(int argc, char *argv[]) {

	//random seed
	srand((unsigned)time(0));

	//--- Console and Log initialization ---

	//create console and log
	exefile.set(argv[0]);
	log_set_prefix(exefile.get_path().c_str());
	l = log_open("minilog.txt","minilog2.txt", LOG_DEFAULT_SIZE, false);
	create_console( console_command_f );
	con->set_debug_mode(false);
	con->enable_log(l);
	con->printf("-----------------------------------------------------------------\n");
	con->printf("The ZIG Library Minimalist Game Demo (zig2)\n");
	con->printf("Command line is: '%s'\n", argv[0] );
	con->printf("Log files are opened in: '%s'\n", exefile.get_path().c_str() );
	con->printf("\n");
	con->printf("Press <HOME> to toggle CONSOLE ON/OFF\n");
	con->printf("Press <ESC> to quit\n");
	con->printf("\n");
	con->printf("--- Console chat: ---\n");
	con->printf("To chat with other players, simply enter any text without '/' as\n");
	con->printf("the first character. Make sure that your client is connected.\n");
	con->printf("\n");
	con->printf("--- Console commands: ---\n");
	con->printf(" /c            : connect to local server\n");
	con->printf(" /c <address>  : connect to a server\n");
	con->printf(" /d            : disconnect from a server\n");
	con->printf(" /os           : start the local server\n");
	con->printf(" /cs           : stop the local server\n");
	con->printf(" /si <address> : get info from a server without connecting\n");
	con->printf(" /li           : get info from all servers in the local network\n");
	con->printf(" /xp <level>   : set log trace level (0-7)\n");
	con->printf("-----------------------------------------------------------------\n\n");

/*
	//
	// just some code to test the brand-spankin new c++ object serialization!
	//
	blah x;
	x.a = 10;
	x.b = 20;
	x.whatever = 666;
	buffer_c buf;
	buf.putObject(x);		// put in the buffer
	buf.seek(0);			// reset buffer pos so we can read from the buffer
	// read the object. we are actually creating a new instance of the CORRECT
	// type ("blah") and returning a reference to a serializable_c
	serializable_c *object = buf.getObject();
	// now you can cast the "object" to the correct type ("blah") if you know
	// for a fact that it is of class "blah", or you can cast to another 
	// superclass that you know for sure that is the superclass of ALL the 
	// classes you are serializing (say, "superblah")
	blah *hx = (blah *)object;
	con->printf("x.a = %i\n",hx->a);  // prints "10"
	con->printf("x.b = %i\n",hx->b);  // prints "20"
	con->printf("x.whatever = %i\n",hx->whatever);  // prints "333"
*/

/*
	//
	//   just some code to test buffer_c's compression/decompression
	//
	buffer_c src;
	buffer_c dest;
	buffer_c udest;
	src.putString("ESTE É O TEXTO QUE QUEREMOS SABER SE COMPRIME. ELE PRECISA SER MAIS OU MENOS GRANDE. NA VERDADE, O\
                 IDEAL É QUE O TEXTO TENHA MAIS DE 50 BYTES. ESPERAMOS QUE OS PACOTES DO JOGO SEJAM MENORES DO QUE\
                 1.6 KB JÁ QUE SUPOMOS UMA FREQÜÊNCIA DE 10 HZ E 16KB/S EM UM ADSL NORMAL. MAIS ALGUMAS LINHAS DE TEXTO\
                 DEVEM SER SUFICIENTES PARA UM TESTE DECENTE. POR SINAL, JÁ REPAROU QUE NÃO É POSSÍVEL ESPIRRAR DE\
                 OLHOS ABERTOS...E NÃO TENTE, PORQUE NÃO VAI GOSTAR DO QUE ACONTECERÁ SE CONSEGUIR. OUTRA COISA\
                 INTERESSANTE É PERCEBER COMO É MUITO MAIS FÁCIL ESCREVER BOBAGENS DO QUE XXXXXXXXXXXXXXX? BOM,\
                 ISSO NÃO VEM AO CASO. ESTOU TERMINANDO ESSE TEXTO POR AQUI. POISEH! APARENTEMENTE CONSEGUI COLOCAR\
								 OS FONTES DO BZIP2 NO ZIG! TAH COMPILANDO! AGORA VOU RODAR PELA PRIMEIRA VEZ UM TESTE! TOMARA QUE\
								 NAO FORMATE O MEU HD! OUTRA COISA, DEPOIS TEM QUE TESTAR COM PACOTES BINARIOS REAIS, PQ COMPRIMIR\
								 ASCII ALL-CAPS NAO DEVE SER MUITO REPRESENTATIVO!");
	con->printf("BZIP2 Compression/Decompression Test!\n");
	con->printf("  uncompressed data size = %i\n", src.size());
	src.seek(0);
	int zip_result = src.zip(dest);
	con->printf("  ZIPPING... R = %i\n", zip_result);
	con->printf("  src buffer current pos = %i , size_left = %i\n", src.getpos(), src.size_left());
	con->printf("  dest buffer current pos = %i , size = %i\n", dest.getpos(), dest.size());
	dest.seek(0);
	int unzip_result = dest.unzip(udest);
	con->printf("  UNZIPPING... R = %i, actual compressed size + buffer_c zip header: read below and guess\n", unzip_result);

	con->printf("  dest buffer_curent pos = %i\n", dest.getpos());
	con->printf("  udest buffer current pos = %i , size = %i\n", udest.getpos(), udest.size());

	if (udest.size() == src.size())
		con->printf("IT WORKED! udest.size == src.size!\n");
	else
		con->printf("IT DIDN'T WORK! udest.size != src.size!\n");
*/

	//------- ZIG initialization -------

	//init ZIG. currently, this will only initialize HawkNL (calls "nlInit()" and "nlSelectNetwork(NL_IP)")
	if (zig_init(con) == false)
		return false;

	//------- Allegro initialization -------
#ifdef GAME_ALLEGRO
	allegro_init();
	install_keyboard();
	install_mouse();
	three_finger_flag = 0;
	set_window_title("The ZIG Library Minimalist Game Demo");
#endif // GAME_ALLEGRO
	return true;
}


//shutdown stuff
void app_shutdown() {

	if (gameclient != NULL) {

		delete gameclient;
		gameclient = NULL;
	}

	if (gameserver != NULL) {

		gameserver->stop();

		delete gameserver;
		gameserver = NULL;
	}

	zig_shutdown(con);			//shutdown ZIG (network...)

	delete_console();				//delete "con" object
	log_close(l);						//close "con"'s backing logfile
}

//main program
int main(int argc, char *argv[]) {

	// GLOBAL emulated packet loss and latency parameters
	if (argc >= 2) zig_global_packet_loss_percent = atoi(argv[1]);
	else zig_global_packet_loss_percent = 0;

	if (argc >= 3) zig_global_packet_delay_base = atoi(argv[2]);
	else zig_global_packet_delay_base = 0;

	if (argc >= 4) zig_global_packet_delay_delta = atoi(argv[3]);
	else zig_global_packet_delay_delta = 0;

	//initialize stuff
	if (app_initialize(argc, argv) == true) {

		//instantiate client and server, now that we have the console instance to pass
		//to the constructor (maybe this could've been designed better...)
		gameserver = new gameserver_c( con );
		gameclient = new gameclient_c( con );

		// configures a range of local UDP ports to be used by the client
		gameclient->set_local_port_range(1300, 1499);

		//run the gameclient. the gameclient deals with starting/stopping a local
		//listen-server and stuff.
		gameclient->start();

		// our main loop. since we're running in non-blocking mode, we must deal with some extra stuff
		// that was taken care of by the zigclient.
		while (1)
		{
			// you may call these functions freely; if by chance the client (or server) in question
			// is not running, then these calls just returns false immediatelly:
			gameclient->process_nonblocking();		// run client non-blocking logic step (must call in a loop)
			gameserver->process_nonblocking();   // run server non-blocking logic step (must call in a loop)
			
			// the zigclient in non-blocking mode DOES NOT deal with input polling and frame rendering,
			// so we must do that here ourselves. we will use the same method names (render_frame() and
			// poll_input() just to keep the example pretty, but we could use any method names or just
			// paste the whole input/render code below...
			//
			// poll input and render frame; returning "false" will mean that client wants to quit the
			// process (shutdown any pending client or server connection and then quit the app)

			if (gameclient->game_read_input() == false)		//break if my poll_input() returns false
				break;
			if (gameclient->game_draw_frame() == false)		//break if my render_frame() returns false
				break;

			// wait a bit so we can actually read what's in the screen (the drawing code does not use
			// a backbuffer
			//
			// also, it is important to at least put a " sched_yield(); " call here, so we won't maybe
			// take over all the CPU with this loop. yielding at some point in a game loop is generally
			// not a bad idea, especially if your game runs in windowed mode.
			SLEEP(1);
		}
	}
	else {

		con->printf("==================================\n");
		con->printf("FATAL ERROR: CAN'T INITIALIZE APP!\n");
		con->printf(" (see the log messages above ...) \n");
		con->printf("==================================\n");
	}

	//shutdown stuff
	app_shutdown();

	//return
	return 0;

#ifdef GAME_ALLEGRO
} END_OF_MAIN()
#else
}
#endif

