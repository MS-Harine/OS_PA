#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "worker.h"

#define LISTEN_QUEUE	16

int *file_count = NULL;

void handle_opt(int, char **, int *);

int main(int argc, char *argv[]) { 
	int serv = 0, client = 0, port = 0, addrlen = 0;
	struct sockaddr_in address;
	pthread_mutex_t *m;
	pthread_mutexattr_t attr;

	handle_opt(argc, argv, &port);
	serv = open_server(port, &address);
	addrlen = sizeof(address);

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	m = (pthread_mutex_t *)mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	file_count = (int *)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	pthread_mutex_init(m, &attr);

	while (1) {
		if (listen(serv, LISTEN_QUEUE) < 0) {
			perror("listen failed : ");
			exit(EXIT_FAILURE);
		}

		client = accept(serv, (struct sockaddr *)&address, (socklen_t *)&addrlen);
		if (client < 0) {
			perror("accept failed: ");
			exit(EXIT_FAILURE);
		}

		if (fork() == 0) // Parent
			close(client);
		else // Child
			task(client, m);
	}
	return 0;
}

void handle_opt(int argc, char **argv, int *port) {
	int c = 0;

	while ((c = getopt(argc, argv, "p:h")) != -1) {
		switch(c) {
			case 'p':
				*port = atoi(optarg);
				break;
			case 'h':
				printf("-p <Port>\n");
				printf("   Open this worker using port.\n");
				printf("-h \n");
				printf("   Print help message.\n");
				exit(0);
				break;
			case '?':
				break;
		}
	}

	if (argc != 3) {
		fprintf(stderr, "Usage -p <Port>\n");
		fprintf(stderr, "See details using -h option.\n");
		exit(EXIT_FAILURE);
	}

	if (port == 0) {
		fprintf(stderr, "Wrong port number. See help using -h option.\n");
		exit(EXIT_FAILURE);
	}
}
