/* Servidor */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

/* Structures */
typedef struct{
	 int client_fd;
	 int id;
} client_data_t;

/* Defines */
#define PORT 10000
#define BANDWIDTH 10 /* 10 megabits = 1,25 megabytes = 12800 kilobytes */
#define CLIENT_BANDWIDTH 100 /* 100 Kbytes */
#define BUFFER 100000 /* 100 Kbytes */

/* Function prototype */
void* client_handler(void *client_data);

/* Globals */
int nb_clients = 0, id = 0;

/* Programa principal */
int main(int argc, char *argv[]){
	int server_fd, client_fd, client_len;
	struct sockaddr_in client;
	pthread_t *client_thread;
	client_data_t *client_data;
	
	/* cria o socket */
	server_fd = socket(AF_INET, SOCK_STREAM, 0); /* internet, orientado à conexão, nb protocolo */
	if (server_fd < 0){
		  perror("ERRO: Criação do Socket !");
		  exit(1);
	 }

	/* configuração do cliente */
	client.sin_family = AF_INET;  /* domínio internet */
	client.sin_addr.s_addr = INADDR_ANY; /* pode ser qualquer cliente */
	client.sin_port = htons(PORT);
	bzero(&(client.sin_zero),8);

	/* Liga o socket à porta */
	if(bind(server_fd, (struct sockaddr *)&client, sizeof(client)) < 0) {
		 perror("\nERRO: bindando o socket\n");
		 exit(1);
	}

	/* Escuta por conexões */
	listen(server_fd, 5);

	client_len = sizeof(client);
	printf("\nConexões ativas = %d\n", nb_clients);
	
	while(1){
		if (nb_clients != 10){
			client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);
			if(client_fd < 0){
				perror("\nERRO: aceitando conexão do ciente\n");
			}
		 	nb_clients++;
		 
		 	/* Prepara a estrutura para a thread */
		 	client_data = (client_data_t *) malloc (sizeof(client_data_t));
		 	client_data->id = id;
		 	client_data->client_fd = client_fd;
		 
		 	/* cria a thread aqui */
		 	client_thread = (pthread_t *) malloc (sizeof(pthread_t));
		 	pthread_create(client_thread+id, NULL, (void *)client_handler, (void *)client_data);
		 	id++; /* incrementa os indentificadores das threads */
		} else {
			printf("\nAlguem tentou conectar-se!\n");
		}
	}
	return(0);
}

void* client_handler(void *client_data){
	client_data_t *data;
	int bytes_read, bytes_write, filesize, fd, tmp, diff;
	char buffer[250], output[30];
	char out_buffer[BUFFER];
	FILE *arq, *arq2;
	struct timeval start, finish;
	long time = 0;
	
	data = (client_data_t *)client_data;
	printf("\nConexões ativas = %d\n", nb_clients);

	/* cliente solicita o arquivo para download */
	bytes_read = read(data->client_fd, buffer, sizeof(buffer));
	if (bytes_read < 0){
		perror("\nERRO: recebendo dados do cliente\n");
	} else {
		buffer[bytes_read] = '\0'; /* coloca finalizador no buffer */
		if(!strcmp(buffer, "toto.avi")){
			perror("\nERRO: Nome de arquivo incorreto\n");
			exit(0);
		} else {
			/* arquivo a ser enviado */
			arq = fopen("toto.avi", "r");
			if (arq == NULL) { perror("\nAbrindo video no servidor\n"); }
			/* envia informaçoes sobre o arquivo para o cliente. Tamanho. */
			fd = fileno(arq);
			fseek(arq, 0, 0);
			filesize = lseek(fd, 0, SEEK_END); /* pega o tamanho do arquivo */
			bytes_write = write(data->client_fd, &filesize, sizeof(filesize));
			if (bytes_write < 0){
				perror("\nERRO: enviando dados ao cliente\n");
			}
			
			/* Arquivo que contem a taxa de transmissão */
			sprintf(output, "%s%d%s", "client", id, ".dat");
			arq2 = fopen(output,"w");
			if (arq == NULL) { perror("\nAbrindo arquivo de taxa\n"); }
			
			fseek(arq, 0, 0);
			tmp = 0;
			int i = 1;
			/* Loop para envio do arquivo */
			while(tmp < filesize){
				/* carrega partes do arquivo */
				//bytes_read = fread(out_buffer, BUFFER, 1, arq);
				bytes_read = read(fd, out_buffer, BUFFER);
				if (bytes_read < 0){
					perror("\nERRO: lendo dados do arquivo\n");
				}
				/* tempo inicial da transmissão */
				gettimeofday(&start, NULL);
				/* envia arquivo ao cliente */
				bytes_write = write(data->client_fd, out_buffer, bytes_read);
				if (bytes_write < 0){
					perror("\nERRO: enviando dados ao cliente\n");
				} 
				/* tempo final da transmissão */
				gettimeofday(&finish, NULL);
				time = (((finish.tv_sec*1000000)+finish.tv_usec)-((start.tv_sec*1000000)+start.tv_usec));
				diff = 1000000 - time;
				
				if(diff < 0) { fprintf(arq2, "%lu %d \n", time, bytes_write); 
								//printf("\nDiff < 0: time = %lu bytes = %d\n", time, bytes_write);
				}
				else if (diff > 0) {
						fprintf(arq2, "%d %d \n", i, bytes_write);
						//printf("\nDiff > 0: time = %lu bytes = %d\n", time, bytes_write);
						usleep(diff);
						i++;
					} else if (diff == 0){ fprintf(arq2, "%lu %d \n", time, bytes_write);
							//printf("\nDiff < 0: time = %lu bytes = %d\n", time, bytes_write);
					}
				tmp+=bytes_write;
			}						
		}
	}
	/* fecha conexão com o cliente */
	int res;
	res = close(data->client_fd);
	if (res < 0){
		perror("\nFechando conexão com o cliente \n");
	} else {
		/* fecha arquivos */
		fclose(arq);
		fclose(arq2);
		printf("\nA conexão com o cliente %d foi fechada\n",data->client_fd);
	}
	nb_clients--;
	printf("\nConexões ativas = %d\n", nb_clients);
	pthread_exit(NULL);
}

