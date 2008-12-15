#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <strings.h>

#define PORT 10000
#define BUFFER 100000 /* 100 Kbytes */

int main(int argc, char *argv[]){
	int socket_fd, tmp;
	int res, filesize;
	struct sockaddr_in server;
	struct hostent *server_info;
	
	char out_buffer[30] = "toto.avi";
	char buffer[BUFFER], *output;
	int bytes_read, bytes_write;
	FILE* arq;
	
	/* testa se os argumentos estão corretos */
	if (argc != 5){
		printf("\nUsage: <server IP> <filename> <port> <local filename>\n");
		return(1);
	}
	
	output = argv[3];
	
	/* cria o socket */
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	/* nome do servidor */
	server_info = gethostbyname(argv[1]);

	bzero((char *)&server, sizeof(server));
	server.sin_family = AF_INET;
	bcopy((char *)server_info->h_addr, (char *)&server.sin_addr.s_addr,
			server_info->h_length);
	server.sin_port = htons(PORT);

	/* conecta ao servidor */
	res = connect(socket_fd, (struct sockaddr *)&server, sizeof(server));
	if (res < 0){
		perror("ERRO: não foi possivel conectar ao servidor");
		return(1);
	}

	/* envia dados ao servidor */
	out_buffer[strlen(out_buffer)-1] = '\0';
	bytes_write = write(socket_fd, out_buffer, sizeof(out_buffer));
	if (bytes_write < 0){
		perror("\nERRO: enviando dados ao servidor\n");
	}

	/* recebe o tamanho do arquivo */
	bytes_read = read(socket_fd, &filesize, sizeof(filesize));
	if (bytes_read < 0){
		perror("\nERRO: recebendo dados do servidor\n");
	}
	printf("\nRecebi o tamanho do arquivo = %d\n",filesize);

	/* Arquivo que contem o video baixado */
	//sprintf(output, "%s%lf%s", "client", r,".avi");

	/* cria arquivo para receber dados */
	arq = fopen(output,"w");
	tmp = 0;
	while(tmp < filesize){
		/* recebendo o video do servidor */
		bytes_read = read(socket_fd, buffer, BUFFER);
		if (bytes_read < 0){
			perror("\nERRO: recebendo dados do servidor\n");
		}
		//printf("\nRecebi %d bytes\n", bytes_read);
		/* escreve no arquivo de saída */
		fwrite(buffer, 1, bytes_read, arq);
		tmp+= bytes_read;
	}
	printf("\nRecebi todo o arquivo\n");
	fclose(arq);
	return(0);
}
