/*This source code copyrighted by Lazy Foo' Productions (2006) and may not be redestributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"
#include "SDL/SDL_net.h"
#include <math.h>
#include <string>

#define PI 3.14
#define MAX_OUTRAS_NAVES 8

typedef struct {
	int pos_x, pos_y, pant_x, pant_y; //posicao da nave, vai e (0,0) a (639,479)
	double alpha, girando_alpha, dir_x, dir_y; //direcao da nave, vai de (-1.0f , -1.0f) a (1.0f , 1.0f)
	short vai_frente;
	short id;
} dados_nave;

typedef struct {
	bool presente;
	int px, py;
	int dx, dy;
	short vai_frente;
	short id;
} nave_rede;

typedef struct {
	int px,py;
	int dx,dy;
	short vai_frente, id;
	char tipo;
	long sequencia;
} mensagem_rede;

long sequencia_msgs;

nave_rede grupo[MAX_OUTRAS_NAVES];

//The attributes of the screen
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 24;

//The surfaces that will be used
SDL_Surface *outras_naves[MAX_OUTRAS_NAVES];
SDL_Surface *local_nave = NULL;
SDL_Surface *nave1 = NULL;
SDL_Surface *nave2 = NULL;
SDL_Surface *nave3 = NULL;
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *message = NULL;

SDL_Event keyevent;

SDL_Thread *thread_pegaKeyPressed = NULL;
SDL_Thread *thread_mandeDados = NULL;
SDL_Thread *thread_pegaDadosDosOutros = NULL;
SDL_Thread *thread_desenhaNaves = NULL;
SDL_Thread *thread_executeDeadReckoning = NULL;

SDL_sem *sem_pos_naves;


dados_nave voyager;


IPaddress server_ip;
UDPsocket sock = NULL;

bool quit = false;
bool use_dr = false;

int pegaDadosDosOutros (void *tem_que_ter_porque_essa_porra_de_sdl_thread_exige) {
	UDPpacket *in_packet;
	mensagem_rede msg;
	
	in_packet = SDLNet_AllocPacket(1024);
	
	while (1) {
		while(!SDLNet_UDP_Recv(sock, in_packet))
      		SDL_Delay(10); //1/100th of a second
      		
		memcpy (&msg, in_packet->data, sizeof(mensagem_rede));
		
		if (msg.sequencia < sequencia_msgs)
			//ignore essa msg
			continue;
		else
			sequencia_msgs = msg.sequencia;

		if (msg.tipo == 'M') {
			
			SDL_SemWait(sem_pos_naves);
			grupo[msg.id].presente = true;
			grupo[msg.id].px = msg.px;
			grupo[msg.id].py = msg.py;
			grupo[msg.id].vai_frente = msg.vai_frente;
			grupo[msg.id].dx = msg.dx;
			grupo[msg.id].dy = msg.dy;
			SDL_SemPost(sem_pos_naves);
		}
		
		if (msg.tipo == 'T') {
			grupo[msg.id].presente = false;
		}		
		
	}
}

int mandeDados(void *bli) {
	mensagem_rede msg;
	UDPpacket *out_packet;
	
	out_packet = SDLNet_AllocPacket(1024);
	
	out_packet->len = sizeof(mensagem_rede);
	out_packet->address.host = server_ip.host;
	out_packet->address.port = server_ip.port;
	
	while (1) {
	
		msg.id = voyager.id;
		msg.px = voyager.pos_x;
		msg.py = voyager.pos_y;
		msg.tipo = 'M'; // [M]ensagem normal de comunicacao durante o "jogo"
		msg.dx = (int) voyager.dir_x;
		msg.dy = (int) voyager.dir_y;
		msg.vai_frente = voyager.vai_frente;
		
		memcpy (out_packet->data, &msg, sizeof(mensagem_rede));
		
		SDLNet_UDP_Send(sock,-1,out_packet); //VAI CARAI, sem controle de erro
		
		SDL_Delay(100);
	
	}
}

void facaLogin (std::string endereco_ip, int porta) {
	
	mensagem_rede msg_login, msg_resposta;
	
	msg_login.dx = 0;
	msg_login.dy = 0;
	msg_login.id = -1; //a nave ainda nao tem ID
	msg_login.px = 0;
	msg_login.py = 0;
	msg_login.tipo = 'L'; //msg do tipo LOGIN
	msg_login.vai_frente = 0;
	
	UDPpacket *out_packet = NULL;	
	UDPpacket *in_packet = NULL;

	if(!(sock=SDLNet_UDP_Open(0)))
    {
    	printf("SDLNet_UDP_Open: %s\n",SDLNet_GetError());
    	exit(-1);
    }
	printf("port %hd opened\n", sock);


	SDLNet_ResolveHost(&server_ip, endereco_ip.c_str(), 15000);


	fprintf(stderr,"Recieving port opened!\n");

	out_packet=SDLNet_AllocPacket(1024);

	if(out_packet==NULL)
	{
    	fprintf(stderr,"Couldn't allocate packet!\n");
    	SDLNet_UDP_Close(sock);
    	exit(-1);
	}

	//MERDAS INICIAIS QUE TEM QUE FAZER: END

 	memcpy(out_packet->data, &msg_login, sizeof(mensagem_rede));
	out_packet->address.host = server_ip.host;
	out_packet->address.port = server_ip.port;
	out_packet->len = sizeof(mensagem_rede);

    fprintf(stderr,"Sending packet of %d bytes\n",out_packet->len);
    if(SDLNet_UDP_Send(sock,-1,out_packet)<0) {
   	fprintf(stderr,"Couldn't send packet!\n");
      SDLNet_UDP_Close(sock);
      SDLNet_FreePacket(out_packet);
      exit(-1);
	}
	
	//ACABOU DE FAZER O PEDIDO DE LOGIN AO SERVIDOR
	
	in_packet=SDLNet_AllocPacket(1024);
	
	if(in_packet==NULL)
    {
  		fprintf(stderr,"Couldn't allocate packet!\n");
    	SDLNet_UDP_Close(sock);
    	exit(1);
 	}
 	
 	while(!SDLNet_UDP_Recv(sock, in_packet))
      SDL_Delay(10); //1/100th of a second

	memcpy (&msg_resposta, in_packet->data, in_packet->len);
		
	printf("UDP Packet incoming\n");
	printf("\tChan:    %d\n", in_packet->channel);
	printf("\tLen:     %d\n", in_packet->len);
	printf("\tMaxlen:  %d\n", in_packet->maxlen);
	printf("\tStatus:  %d\n", in_packet->status);
	printf("\tAddress: %x %x\n\t------------\n", in_packet->address.host, in_packet->address.port);
	
	printf("\tTipo:    %c\n", msg_resposta.tipo);
	printf("\tID:    %d\n\n\n", msg_resposta.id);
	
	if (msg_resposta.tipo == 'C') {//pedido concedido
		printf ("pedido de login concedido pelo servidor\nID desta nave: %d\n\n", msg_resposta.id);		
		voyager.id = msg_resposta.id;
	}
		
	if (msg_resposta.tipo == 'N') {//pedido negado
		printf ("pedido de login negado pelo servidor\n");
		exit(1);
	}
}

void digaTchau() {
	UDPpacket *tchau_packet=SDLNet_AllocPacket(1024);
	mensagem_rede tchau;
	
	tchau.dx = 0;
	tchau.dy = 0;
	tchau.id = voyager.id;
	tchau.px = 0;
	tchau.py = 0;
	tchau.tipo = 'T'; //T de Tchau
	tchau.vai_frente = 0;
	
	tchau_packet->len = sizeof(mensagem_rede);
	tchau_packet->address.host = server_ip.host;
	tchau_packet->address.port = server_ip.port;	
	memcpy(tchau_packet->data, &tchau, sizeof(mensagem_rede));
	
	SDLNet_UDP_Send(sock,-1,tchau_packet); //VAI CARAI, sem controle de erro
}

double valorMinimo (double a, double b) {
	if (a <= b) return a;
	else return b;	
}

float posicaoValida (double c, char coord) {
	switch (coord) {
		case 'x':
			if (c < 0) return 0;
			if (c > 610) return 610;
			return c;
			break;
		case 'y':
			if (c < 0) return 0;
			if (c > 450) return 450;
			return c;
			break;		
	}
		
}

int pegaKeyPressed (void *bli) {
	
	while( true )	
    {    	
    	SDL_Delay( 10 );
        //If there's an event to handle
        if (SDL_PollEvent(&keyevent))   //Poll our SDL key event for any keystrokes.
		{
		  if (keyevent.type == SDL_KEYDOWN) {		    
		      switch(keyevent.key.keysym.sym){
		        case SDLK_LEFT:
		          //voyager.dir_x = -1;
		          
		          voyager.girando_alpha = -PI / 100;
		          		          	
		          break;
		        case SDLK_RIGHT:
		          //voyager.dir_x = 1;
		          
		          voyager.girando_alpha =  PI / 100;
		          
		          break;
		        case SDLK_UP:
		          voyager.vai_frente = 1;
		          break;
		        case SDLK_DOWN:
		          voyager.vai_frente = -1;
		          break;
		        case SDLK_q:
		          quit = true;
		        default:
		          break;
		       }
		  }
		  if (keyevent.type == SDL_KEYUP) {
		      switch(keyevent.key.keysym.sym){
		        case SDLK_LEFT:
		          voyager.girando_alpha = 0;
		          break;
		        case SDLK_RIGHT:
		          voyager.girando_alpha = 0;
		          break;
		        case SDLK_UP:
		          voyager.vai_frente = 0;
		          break;
		        case SDLK_DOWN:
		          voyager.vai_frente = 0;
		          break;
		        default:
		          break;
		      }
		  }
		}
		
		voyager.alpha += voyager.girando_alpha;
		if (voyager.alpha < -PI)
			voyager.alpha += 2*PI;
		
		if (voyager.alpha > PI)
			voyager.alpha -= 2*PI;
		
		voyager.dir_x = 5 * cos(voyager.alpha)/* / valorMinimo( sin(voyager.alpha) , cos(voyager.alpha) )*/;
		voyager.dir_y = 5 * sin(voyager.alpha)/* / valorMinimo( sin(voyager.alpha) , cos(voyager.alpha) )*/;

		voyager.pos_x = (int) posicaoValida(voyager.pos_x + voyager.vai_frente * voyager.dir_x, 'x');
		voyager.pos_y = (int) posicaoValida(voyager.pos_y + voyager.vai_frente * voyager.dir_y, 'y');  
    }	
    return 0;
}

SDL_Surface *load_image( std::string filename ) 
{
    //Temporary storage for the image that's loaded
    SDL_Surface* loadedImage = NULL;
    
    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;
    
    //Load the image
    loadedImage = SDL_LoadBMP( filename.c_str() );
    
    //If nothing went wrong in loading the image
    if( loadedImage != NULL )
    {
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );
        
        //Free the old image
        SDL_FreeSurface( loadedImage );
    }
    
    //Return the optimized image
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
    //Make a temporary rectangle to hold the offsets
    SDL_Rect offset;
    
    //Give the offsets to the rectangle
    offset.x = x;
    offset.y = y;
    
    //Blit the surface
    SDL_BlitSurface( source, NULL, destination, &offset );
}

int executeDeadReckoning(void *bli) {
	
	while (1) {
		for (short i = 0; i < MAX_OUTRAS_NAVES; i++)
			if (grupo[i].presente) {
				SDL_SemWait(sem_pos_naves);
				grupo[i].px = (int) posicaoValida(grupo[i].px + grupo[i].vai_frente * grupo[i].dx, 'x');
				grupo[i].py = (int) posicaoValida(grupo[i].py + grupo[i].vai_frente * grupo[i].dy, 'y');
				SDL_SemPost(sem_pos_naves);
			}
		SDL_Delay(10);
	}
}

int desenhaNaves (void *bli){
	while (1) {
  		apply_surface( 0, 0, background, screen );
    
    	for (short i = 0; i < MAX_OUTRAS_NAVES; i++)
    		if (grupo[i].presente)
    			apply_surface( grupo[i].px, grupo[i].py, outras_naves[i], screen );
    		
		apply_surface( voyager.pos_x, voyager.pos_y, local_nave, screen );
    
    	if( SDL_Flip( screen ) == -1 )
    	{
        	return 1;    
    	}
    	
  		SDL_Delay( 10 );
  		
  		if (quit) break;
    }
}

int main( int argc, char* args[] )
{
	
	voyager.pos_x = 200;
	voyager.pos_y = 200;
	voyager.dir_x = 0;
	voyager.dir_y = 0;
	voyager.alpha = 0;
	voyager.vai_frente = 0;
	std::string endereco_servidor;
	
	if (argc != 2 && argc != 3) {
		printf ("Usável: ./gameclient.bin endereco.do.servidor [dr]\n");
		printf ("\tdr: utilizar o algoritmo de DEAD RECKONING\n");
		exit (1);
	}
	
	endereco_servidor = args[1];

	if (argc == 3 && std::string("dr").compare(args[2]) == 0) {
		use_dr = true;
		sem_pos_naves = SDL_CreateSemaphore(1);
		if (sem_pos_naves == NULL) 
        	printf ("\n\t***ERRO: nao conseguiu criar o semaforo\n");
        else
        	printf ("\n\n\t*** EXECUTANDO DEAD RECKONING - PREDICAO LINEAR ***\n\n");
	}

	for (short i; i < MAX_OUTRAS_NAVES; i++)
		grupo[i].presente = false;
	
	
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        printf("SDL_Init: %s\n",SDL_GetError());
    	exit(1);    
    }
    
    if(SDLNet_Init()==-1)
    {
    	printf("SDLNet_Init: %s\n",SDLNet_GetError());
    	exit(2);
    }
    
    facaLogin(endereco_servidor, 15000);
    
   
    
    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
    
    //If there was in error in setting up the screen
    if( screen == NULL )
    {
        return 1;    
    }
    
    //Set the window caption
    SDL_WM_SetCaption( "Garou no Kaisen", NULL );
    
    //Load the images
    
	printf ("voyager.id: %d\n",voyager.id);	
    
    switch (voyager.id) {
    	case 0: local_nave = load_image( "nave0.bmp" ); break;
    	case 1: local_nave = load_image( "nave1.bmp" ); break;
    	case 2: local_nave = load_image( "nave2.bmp" ); break;
    	case 3: local_nave = load_image( "nave3.bmp" ); break;
    	case 4: local_nave = load_image( "nave4.bmp" ); break;
    	case 5: local_nave = load_image( "nave5.bmp" ); break;
    	case 6: local_nave = load_image( "nave6.bmp" ); break;
    	case 7: local_nave = load_image( "nave7.bmp" ); break;    
    }
    
    for (short i; i < MAX_OUTRAS_NAVES; i++) {
    	switch (i) {
    		case 0: outras_naves[i] = load_image( "nave0.bmp" ); break;
	    	case 1: outras_naves[i] = load_image( "nave1.bmp" ); break;
    		case 2: outras_naves[i] = load_image( "nave2.bmp" ); break;
    		case 3: outras_naves[i] = load_image( "nave3.bmp" ); break;
	    	case 4: outras_naves[i] = load_image( "nave4.bmp" ); break;
    		case 5: outras_naves[i] = load_image( "nave5.bmp" ); break;
    		case 6: outras_naves[i] = load_image( "nave6.bmp" ); break;
	    	case 7: outras_naves[i] = load_image( "nave7.bmp" ); break;    
    	}		
    }
    
    nave1 = load_image( "nave1.bmp" );
    nave2 = load_image( "nave2.bmp" );
    nave3 = load_image( "nave3.bmp" );
    
    background = load_image( "garoubg.bmp" );
    
    thread_pegaKeyPressed = SDL_CreateThread( pegaKeyPressed, NULL );
    thread_desenhaNaves = SDL_CreateThread( desenhaNaves, NULL );
    thread_mandeDados = SDL_CreateThread( mandeDados, NULL);
	thread_pegaDadosDosOutros = SDL_CreateThread( pegaDadosDosOutros, NULL );
	if (use_dr)
		thread_executeDeadReckoning = SDL_CreateThread( executeDeadReckoning, NULL );
    
    //Apply the message to the screen
    while (!quit) {
    	SDL_Delay(500);
    }
    
    digaTchau();
    
    //Update the screen
    
    
    //Wait 2 seconds
    
    SDL_KillThread( thread_pegaKeyPressed );
    SDL_KillThread( thread_desenhaNaves );
    SDL_KillThread( thread_mandeDados );
    SDL_KillThread( thread_pegaDadosDosOutros );
    if (use_dr)
    	SDL_KillThread( thread_executeDeadReckoning );
    
    
    SDL_Delay(200);
       
    //Free the surfaces
    SDL_FreeSurface( local_nave );
    SDL_FreeSurface( nave1 );
    SDL_FreeSurface( nave2 );
    SDL_FreeSurface( nave3 );
    
    SDL_FreeSurface( background );
    
        
    
    //Quit SDL
    SDL_Quit();
    
    return 0;    
}
