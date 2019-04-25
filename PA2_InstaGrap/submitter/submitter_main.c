#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include "submitter.h"
#include "protocol.h"

#define	WAIT_TIME	1

void handle_opt(int, char **, Data *);

int main(int argc, char *argv[]) {
	int status = 0;
	char *result = NULL;
	Data info;

	handle_opt(argc, argv, &info);

	// Try test
	do {
		status = work(&info, &result, 1);
		if (status == LOGIN_FAILED) {
			printf("Login failed.\n");
			exit(EXIT_FAILURE);
		}
		if (status == BUILD_FAILED) {
			printf("Build failed. See error message.\n");
			printf("%s", result);
			exit(EXIT_FAILURE);
		}

		switch (status) {
		case TEST_PROCESS:
			printf("> Testing... %s\b", result);
			sleep(WAIT_TIME);
			break;
		case TEST_TIMEOUT:
			printf("> Timeout\n");
			break;
		case TEST_FAILED:
			printf("> Wrong\n");
			printf("> %s/10\n", result);
			break;
		case TEST_SUCCESS:
			printf("> Success!\n");
			printf("> %s/10\n", result);
			break;
		}
		FREE(result);
	} while (status == TEST_PROCESS);

	return 0;
}

void handle_opt(int argc, char **argv, Data *data) {
	int c = 0, i = 0;
	char *ip_n_port = NULL, *temp = NULL;
	struct stat buffer;

	while ((c = getopt(argc, argv, "n:u:k:h")) != -1) {
		switch (c) {
		case 'n':
			ip_n_port = strdup(optarg);
			break;
		case 'u':
			(data->auth).id = optarg;
			break;
		case 'k':
			(data->auth).pw = optarg;
			break;
		case 'h':
			printf("-n <IP>:<Port>\n");
			printf("   Connect to server using IP and port.\n\n");
			printf("-u <ID>\n");
			printf("   Login to server using ID.\n");
			printf("   If you are first time to login to server,\n");
			printf("   you will sign up automatically.\n\n");
			printf("-k <PW>\n");
			printf("   Login to server using password.\n\n");
			printf("-h\n");
			printf("   Print help message.\n");
			exit(0);
			break;
		case '?':
			break;
		}
	}

	if (argc != 8) {
		fputs("Usage -n <IP>:<Port> -u <ID> -k <PW> <File>\n", stderr);
		fputs("See details using -h option.\n", stderr);
		exit(EXIT_FAILURE);
	}

	// Seperate ip and port from one string
	temp = strtok(ip_n_port, ":");
	(data->network).ip = temp;
	if ((data->network).ip == NULL) {
		fputs("Wrong arguments with -n option. See help using -h option.\n", stderr);
		exit(EXIT_FAILURE);
	}
	(data->network).port = atoi(ip_n_port + strlen((data->network).ip) + 1);
	if ((data->network).port == 0) {
		fputs("Wrong port number. See help using -h option.\n", stderr);
		exit(EXIT_FAILURE);
	}

	// Check id and pw form
	if (strlen((data->auth).id) != 8) {
		fputs("Id should be 8 digit number.\n", stderr);
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < (int)strlen((data->auth).id); i++) {
		if (!isdigit((data->auth).id[i])) {
			fputs("Id should be 8 digit number.\n", stderr);
			exit(EXIT_FAILURE);
		}
	}

	if (strlen((data->auth).pw) != 8) {
		fputs("PW should be 8 digit alphanumeric string.\n", stderr);
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < (int)strlen((data->auth).pw); i++) {
		if (!isalnum((data->auth).pw[i])) {
			fputs("PW should be 8 digit alphanumeric string.\n", stderr);
			exit(EXIT_FAILURE);
		}
	}
	
	// Check is file exist
	data->filename = argv[7];
	if (stat(data->filename, &buffer) != 0) {
		fputs("File doesn't exist.\n", stderr);
		exit(EXIT_FAILURE);
	}
}
