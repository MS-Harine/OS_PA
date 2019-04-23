#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "instagrapd.h"

#define	LISTEN_QUEUE	16

void handle_opt(int, char **, Data *);

int main(int argc, char *argv[]) {
	int serv = 0, client = 0;
	int addrlen = 0;
	struct sockaddr_in address;
	struct _thread_args *arg;
	pthread_t thread;
	Data info;

	addrlen = sizeof(address);

	handle_opt(argc, argv, &info);
	serv = open_server(&info, &address);
	//worker = connect_to_worker(&info);

	while (1) {
		if (listen(serv, LISTEN_QUEUE) < 0) {
			perror("listen failed: ");
			exit(EXIT_FAILURE);
		}

		client = accept(serv, (struct sockaddr *)&address, (socklen_t *)&addrlen);
		if (client < 0) {
			perror("accept failed: ");
			exit(EXIT_FAILURE);
		}

		arg = (struct _thread_args *)malloc(sizeof(struct _thread_args));
		arg->data = &info;
		arg->sock = client;

		pthread_create(&thread, NULL, work, (void *)arg);
	}
	free(info.worker.ip);
	return 0;
}

void handle_opt(int argc, char **argv, Data *data) {
	int c = 0;
	char *ip_n_port = NULL, *temp = NULL;
	struct stat buffer;

	while ((c = getopt(argc, argv, "p:w:h")) != -1) {
		switch(c) {
		case 'p':
			(data->this).port = atoi(optarg);
			break;
		case 'w':
			ip_n_port = strdup(optarg);
			break;
		case 'h':
			printf("-p <Port>\n");
			printf("   Open this server using port.\n");
			printf("-w <IP>:<Port>\n");
			printf("   Connect to worker using IP and port.\n");
			printf("-h\n");
			printf("   Print help message.\n");
			exit(0);
			break;
		case '?':
			break;
		}
	}

	if (argc != 6) {
		fputs("Usage -p <Port> -w <IP>:<WPort> <DIR>\n", stderr);
		fputs("See details using -h option.\n", stderr);
		exit(EXIT_FAILURE);
	}

	// Seoerate ip and port from one string
	temp = strtok(ip_n_port, ":");
	(data->worker).ip = temp;
	if ((data->worker).ip == NULL) {
		fputs("Wrong arguments with -w option. See help using -h option.\n", stderr);
		exit(EXIT_FAILURE);
	}
	(data->worker).port = atoi(ip_n_port + strlen((data->worker).ip) + 1);
	if ((data->worker).port == 0) {
		fputs("Wrong port number. See help using -h option.\n", stderr);
		exit(EXIT_FAILURE);
	}

	// Check is directory exist
	data->filepath = argv[5];
	if (stat(data->filepath, &buffer) != 0) {
		fputs("Directory doesn't exist.\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (!S_ISDIR(buffer.st_mode)) {
		fputs("DIR is not directory.\n", stderr);
		exit(EXIT_FAILURE);
	}
}
