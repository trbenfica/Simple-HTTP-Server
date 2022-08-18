#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define CONNMAX 20
#define BYTES 1024
int slot=0;

char *ROOT;
int listenfd, clients[CONNMAX], contador=0;
void error(char *);
void startServer(char *);
void respond(int*);
long GetFileSize(const char* filename);

int main(int argc, char* argv[])
{
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	char c;    
	
	char PORT[6];
	ROOT = getenv("PWD");
	strcpy(PORT,"9010");

	printf("### SERVIDOR ONLINE. %s%s%s PATH: %s%s%s\n","\033[92m",PORT,"\033[0m","\033[92m",ROOT,"\033[0m");
	int i;
	for (i=0; i<CONNMAX; i++)
		clients[i] = -1;
	startServer(PORT);
	pthread_t thread;
	void (*respond_pointer)(int*) = &respond;

	while(1){
		addrlen = sizeof(clientaddr);
		clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

		if (clients[slot]<0)
			error ("Erro ao aceitar a conexao");
		else {
			int addr = slot;

			printf("\n### Criando thread para o cliente %d\n", slot);
			fflush(stdin);
			pthread_create(&thread, NULL, (void*)(respond_pointer), &addr);

			slot = (slot + 1) % CONNMAX;
		}
	} 
	
	return 0;
}

//client connection
void respond(int* addr)
{
	int n = *addr;
	char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999], *file_type;
	int rcvd, file, bytes_read;

	memset( (void*)mesg, (int)'\0', 99999 );

	while((rcvd = recv(clients[n], mesg, 99999, 0)) > 0) {
		printf("### Requisição do cliente %d:\n\n%s", n, mesg);
		fflush(stdin);
		reqline[0] = strtok (mesg, " \r\n");
		if (strncmp(reqline[0], "GET\0", 4) == 0) {
			reqline[1] = strtok (NULL, " \r\n");
			reqline[2] = strtok (NULL, " \r\n");
			if (strncmp(reqline[1], "/\0", 2) == 0)
				reqline[1] = "/index.html"; // Se nenhum arquivo for específicado, envia o index

			strcpy(path, ROOT);
			strcpy(&path[strlen(ROOT)], reqline[1]);
			printf("file: %s\n", path);

			int fileSize;
			if ((file = open(path, O_RDONLY)) != -1) {
				char *start_req;
				struct stat info;
				fstat(file, &info);
				fileSize = info.st_size;
				if(strstr(reqline[1], "html") != NULL){
					char string[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=5, max=1000\r\nContent-Length: ";
					start_req = malloc(sizeof(string));
					strcpy(start_req, string);
				}
				else if(strstr(reqline[1], "css") != NULL){
					char string[] = "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=5, max=1000\r\nContent-Length: ";
					start_req = malloc(sizeof(string));
					strcpy(start_req, string);
				}
				else {
					char string[] = "HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=5, max=1000\r\nContent-Length: ";
					start_req = malloc(sizeof(string));
					strcpy(start_req, string);
				}
				char final_req[] = "\r\n\r\n";
				char content_length[15];
				sprintf (content_length, "%d", fileSize);
				char header[strlen(start_req) + strlen(content_length) + strlen(final_req)];
				strcpy(header, start_req);
				strcat(header, content_length);
				strcat(header, final_req);
				write(clients[n], header, sizeof(header));

				printf("\nCabeçalho enviado para o cliente: \n%s", header);

				while ((bytes_read = read(file, data_to_send, BYTES)) > 0){
					write (clients[n], data_to_send, sizeof(data_to_send));
				}
				printf("\nArquivo enviado para o cliente\n");
				close(file);
			}
			else
				write(clients[n], "HTTP/1.1 404 Not Found\n", 23); 
		}
	}

	printf("\n### Socket do cliente %d está sendo encerrado\n", n);
}

//inicia o server
void startServer(char *port)
{
	struct sockaddr_in my_addr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	int enabled = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled));
	setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &enabled, sizeof(int)); // habilita keep alive
	setsockopt(listenfd, IPPROTO_TCP, TCP_KEEPIDLE, &enabled, sizeof(int)); // tempo ocioso até começar a enviar sondas keep alive
	setsockopt(listenfd, IPPROTO_TCP, TCP_KEEPINTVL, &enabled, sizeof(int)); // intervalo entre o envio das sondas keep alive
	int maxpkt = 10;
	setsockopt(listenfd, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)); // quantidade máxima de sondas keep alive, depois fecha a conexão

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(9010);
	my_addr.sin_addr.s_addr = INADDR_ANY; // localhost
	if (bind(listenfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) { // vincula socket ao end e num porta especificados acima
		puts("Binding failed");
	}

	listen(listenfd, CONNMAX);
}

long GetFileSize(const char* filename)
{
    long size;
    FILE *f;

    f = fopen(filename, "rb");
    if (f == NULL) return -1;
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fclose(f);

    return size;
}

