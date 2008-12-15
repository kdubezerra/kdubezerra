/* Copyright (C) 2003 PRAV - Pesquisa em Redes de Alta Velocidade
 *				 http://prav.unisinos.br
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Receptor 
 * Esqueleto de um receptor UDP. 
 * Desenvolvido para sistemas UNIX Like (Linux, FreeBSD, NetBSD...) e Windows
 * Written by:
 *		Maiko de Andrade
 *		Valter Roesler
*/


#include <stdio.h>
#include <string.h>

#ifdef _WIN32
	#include <winsock2.h>
#else
	#define SOCKET int
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
#endif

int main(int argc, char **argv){
	 struct  sockaddr_in peer;
	 SOCKET  s;
	 int porta, peerlen, rc, i;
	 char buffer[100];
	 
#ifdef _WIN32
	 WSADATA wsaData;
  

	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		printf("Erro no startup do socket\n");
		exit(1);
	}
#endif

	 if(argc < 2) {
		  printf("Falta parametros.\n");
		  printf("Parametros validos:\n");
		  printf("-p <porta>\n");
		  exit(1);
	 }

	 for(i=1; i < argc; i++) {
		  if(argv[i][0]=='-'){
				switch(argv[i][1]){
					 case 'p':
						  i++;
						  porta = atoi(argv[i]);
						  if(porta < 1024) {
								printf("Porta invalida\n");
								exit(1);
						  }
						  break;
					 default:
						  printf("Parametro %d: %s invalido\n",i,&argv[i]);
						  exit(1);
				}
		  }else{
				printf("Parametro %d: %s invalido\n",i,&argv[i]);
				exit(1);
		  }
	 }
	 // Zera a variavel peer e define seus parametros
	 memset((void *) &peer,0,sizeof(struct sockaddr_in));
	 peer.sin_family = AF_INET;
	 peer.sin_port = htons(porta);
	 peer.sin_addr.s_addr = htonl(INADDR_ANY); // Receve de qualquer IP 

	 s = socket(AF_INET, SOCK_DGRAM, 0);
	 if( s < 0) {
		  printf("Falha na criacao do socket\n");
		  exit(1);
 	 }

	 if(bind(s,(struct sockaddr *) &peer, sizeof(peer))) {
		  printf("Erro no bind\n");
		  exit(1);
	 }
	 
	 peerlen = sizeof(peer);
	
	 // Recebe um pacote com SIN
#ifdef _WIN32
	 rc = recvfrom(s,buffer,sizeof(buffer),0,(struct sockaddr *) &peer,&peerlen);
#else
	 rc = recvfrom(s,buffer,sizeof(buffer),0,(struct sockaddr *) &peer,(socklen_t *)&peerlen);
#endif
	 printf("Recebido %s de tamanho %d\n",&buffer,rc);
	
	 // Envia um pacote com SIN+ACK
	 strcpy(buffer,"SIN+ACK");
	 sendto(s,buffer,sizeof(buffer),0,(struct sockaddr *) &peer,peerlen); 
	 printf("Enviado SIN+ACK\n");
	
}
