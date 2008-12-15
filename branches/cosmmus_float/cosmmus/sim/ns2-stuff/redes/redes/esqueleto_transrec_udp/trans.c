/* Copyright (C) 2003 PRAV - Pesquisa em Redes de Alta Velocidade
 *               http://prav.unisinos.br
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
 * Transmissor 
 * Esqueleto de transmicao UDP. 
 * Desenvolvido para sistemas UNIX Like (Linux, FreeBSD, NetBSD...) e Windows
 * Written by:
 *      Maiko de Andrade
 *      Valter Roesler
*/

#include <stdio.h>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #define SOCKET  int
#endif

int main(int argc, char **argv){
     struct sockaddr_in peer;
     SOCKET s;
     int porta, peerlen, rc, i;
     char ip[15], buffer[29];//tamanho do pacote
     
#ifdef _WIN32
     WSADATA wsaData;
  

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("Erro no startup do socket\n");
        exit(1);
    }
#endif

     if(argc < 5) {
          printf("Falta parametros\n");
          printf("Parametros:\n");
          printf("-h <numero_ip>\n");
          printf("-p <porta>\n");
          exit(1);
     }

     // Pega parametros
     for(i=1;i < argc; i++){
          if(argv[i][0]=='-') {
                switch(argv[i][1]) {
                     case 'h': // Perametro IP
                          i++;
                          strcpy(ip,argv[i]);
                          break;

                     // Pega o parametro porta
                     case 'p':
                          i++;
                          porta = atoi(argv[i]);
                          if(porta < 1024) {
                                printf("Valor da porta invalido\n");
                                exit(1);
                          }
                          break;
                          
                     default:
                          printf("Parametro recebido invalido %d:%s\n",i,argv[i]);
                          exit(1);
                }            

          } else {
                printf("Parametro %d %s invalido\n",i,&argv[i]); 
                exit(1);
          }
     }
     
     peer.sin_family = AF_INET;
     peer.sin_port = htons(porta);
     peer.sin_addr.s_addr = inet_addr(ip); 

     s = socket(AF_INET, SOCK_DGRAM,0);
     if( s < 0) {
          printf("Falha na criacao do socket\n");
          exit(1);
     }
     
     peerlen = sizeof(peer);
    
     // Envia um pacote com SIN
     strcpy(buffer,"SIN");
     sendto(s,buffer,sizeof(buffer),0,(struct sockaddr *) &peer,peerlen);
     printf("Enviado SIN\n");
    
     // Recebe um pacote com SIN+ACK
#ifdef _WIN32
     rc = recvfrom(s,buffer,sizeof(buffer),0,(struct sockaddr *) &peer,&peerlen); 
#else
     rc = recvfrom(s,buffer,sizeof(buffer),0,(struct sockaddr *) &peer,(socklen_t *) &peerlen); 
#endif
     printf("Recebido %s\n",&buffer);
    
     // Envia um pacote com ACK
     strcpy(buffer,"ACK");
     sendto(s,buffer,sizeof(buffer),0,(struct sockaddr *) &peer,peerlen);
     printf("Enviado ACK\n");
}
