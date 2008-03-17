#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"

#define MAX_Clientes 8

typedef struct {
	int px,py;
	int dx,dy;
	short vai_frente, id;
	char tipo;
	long sequencia;
} mensagem_rede;

UDPsocket sock = NULL;
IPaddress clientes[MAX_Clientes];
short vago[MAX_Clientes];

long sequencia_global = 0;

short porcentagem_de_perda = 0;

short faca_loopback = 0;

void dissemine(UDPpacket *pacote){
	UDPpacket *out_packet = NULL;
	mensagem_rede msg;
	short origem,destino;	
	
	memcpy(&msg, pacote->data, sizeof(mensagem_rede));
	origem = msg.id;
	
	msg.sequencia = sequencia_global;
	
	sequencia_global++;
	
	out_packet=SDLNet_AllocPacket(1024);
	
	memcpy(out_packet->data, &msg, sizeof(mensagem_rede));
	out_packet->len = sizeof(mensagem_rede);
	
	for (destino = 0; destino < MAX_Clientes; destino++) {
		if (vago[destino] == 0 && (destino != origem || faca_loopback == 1)  ) {
			
			printf ("mandando para o destino: \t%d\n",destino);

			out_packet->address.host = clientes[destino].host;
			out_packet->address.port = clientes[destino].port;

			//implementando a PERDA de pacotes...

			

			if (msg.tipo == 'T' || (rand() % 101) > porcentagem_de_perda) {				
				if(SDLNet_UDP_Send(sock,-1,out_packet)<0)
					fprintf(stderr,"Couldn't send packet!\n");
			}

		}
	}
	printf("\n\t***Sequencia: %i\n", sequencia_global);
}

void verificaSeCadastravel(UDPpacket *pacote) {
	UDPpacket *out_packet;
	mensagem_rede resposta;
	//IPaddress origem;
	short cliente;	
	
	resposta.dx = 0;
	resposta.dy = 0;
	resposta.id = -1;
	resposta.px = 0;
	resposta.py = 0;
	resposta.tipo = '?';
	resposta.vai_frente = 0;
	
	out_packet=SDLNet_AllocPacket(1024);
	
	out_packet->len = sizeof(mensagem_rede);
	
	for (cliente = 0; cliente < MAX_Clientes; cliente++)
		if (vago[cliente] == 1)
			break;
		
	
	if (cliente >= MAX_Clientes) { 	//responda que nao deu...
		resposta.tipo = 'N';
		
		memcpy (out_packet->data, &resposta, sizeof(mensagem_rede));
		out_packet->address.host = pacote->address.host;
		out_packet->address.port = pacote->address.port;
		
		SDLNet_UDP_Send(sock,-1,out_packet); //VAI CARAI, sem controle de erro
						
		return ;
	}

	resposta.tipo = 'C';
	resposta.id = cliente;
	clientes[cliente].host = pacote->address.host;
	clientes[cliente].port = pacote->address.port;
	vago[cliente] = 0;
		
	memcpy (out_packet->data, &resposta, sizeof(mensagem_rede));
	out_packet->address.host = pacote->address.host;
	out_packet->address.port = pacote->address.port;
		
	SDLNet_UDP_Send(sock,-1,out_packet); //VAI CARAI, sem controle de erro
	
	//responda que deu, com o id = cliente; tipo = 'C'; vago[cliente] = 0
	return;
	
}	



int main (int argc, char *argv[]) {
	int i;
	
	if (argc > 3) {
		printf ("usavel: ./gameserver [percentagem.de.perda.de.pacotes] [-loopback]\n");
		printf ("\t-loopback: devolve a um cliente seus proprios dados\n\n");
		printf ("\texemplo para 37%% de perda: ./gameserver 37\n");
		exit(1);
	}
	
	if (argc == 2 || argc == 3) {
		if ( atoi(argv[1])<0 || atoi(argv[1])>99) {
			printf ("usavel: ./gameserver [percentagem.de.perda.de.pacotes] [-loopback]\n");
			printf ("\t-loopback: devolve a um cliente seus proprios dados\n\n");
			printf ("\ta percentagem de perda deve variar de 0 a 99!!!\n");
			exit(1);			
		}
		porcentagem_de_perda = atoi(argv[1]);
		printf ("\nSERVIDOR INICIADO COM SIMULACAO DE PERDA DE %i porcento DOS PACOTES\n",porcentagem_de_perda);
	}
	
	
	
	if ( ( argc>=2 && strcmp(argv[1],"-loopback")==0 )  ||
		 ( argc==3 && strcmp(argv[2],"-loopback")==0 )  )
	{
		faca_loopback = 1;
		printf ("servidor configurado para LOOPBACK\n");		 
	}
	
		 
		
	
	for (i = 0; i < MAX_Clientes; i++)
		vago[i] = 1;

	Uint16 port = 15000;
	UDPpacket *in_packet = NULL;

   if(SDL_Init(0)==-1)
   {
      printf("SDL_Init: %s\n",SDL_GetError());
      exit(1);
   }


	if(SDLNet_Init()==-1)
   {
      printf("SDLNet_Init: %s\n",SDLNet_GetError());
      exit(2);
   }


   if(!(sock=SDLNet_UDP_Open(port)))
   {
      printf("SDLNet_UDP_Open: %s\n",SDLNet_GetError());
      exit(4);
   }
   printf("port %hd opened\n", port);


	in_packet=SDLNet_AllocPacket(1024);

	if(in_packet==NULL)
  	{
      fprintf(stderr,"Couldn't allocate packet!\n");
      SDLNet_UDP_Close(sock);
      return(1);
 	}


	while (1) 
	{
		
		mensagem_rede rc_mensagem;
	
		while(!SDLNet_UDP_Recv(sock, in_packet))
      	SDL_Delay(5); //1/200th of a second

		memcpy (&rc_mensagem, in_packet->data, sizeof(mensagem_rede));
		
		printf("UDP Packet incoming\n");
		printf("\tChan:		%d\n", in_packet->channel);
		printf("\tTipo:     %c\n", rc_mensagem.tipo);
		printf("\tId:		%d\n", rc_mensagem.id);
		printf("\tLen: 		%d\n", in_packet->len);
		printf("\tMaxlen:	%d\n", in_packet->maxlen);
		printf("\tStatus:	%d\n", in_packet->status);
		printf("\tAddress:	%x %x\n", in_packet->address.host, in_packet->address.port);			

		switch (rc_mensagem.tipo) {
			case 'L': 
				verificaSeCadastravel(in_packet);
				break;
			case 'T':
				vago[rc_mensagem.id] = 1;
				dissemine(in_packet);
				break;
			case 'M':
				dissemine(in_packet);
				break;
			default:
				break;				
		}
	}
}
