#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "submitter.h"
#include "protocol.h"

#define	FREE(x)	free(x); x = NULL;

int _send(int sock, char *message) {
	int len = 0, s = 0, result  = 0;
	char *orig = message;

	len = strlen(orig);
	while (len > 0 && (s = send(sock, orig, len, 0)) > 0) {
		orig += s;
		len -= s;
		result += s;
	}

	return result;
}

int _recv(int sock, char *message) {
	int len = 0, s = 0;
	char buf[BUF_SIZE];
	
	if (message != NULL) {
		free(message);
		message = NULL;
	}

	while ((s = recv(sock, buf, BUF_SIZE - 1, 0)) > 0) {
		buf[s] = 0x0;
		if (message == NULL) {
			message = strdup(buf);
			len = s;
		}
		else {
			message = realloc(message, len + s + 1);
			strncpy(message + len, buf, s);
			message[len + s] = 0x0;
			len += s;
		}
	}

	if (len <= 0) {
		fputs("Failed to receive message from server.\n", stderr);
		exit(EXIT_FAILURE);
	}

	return len;
}

int try_login(int sock, const char *id, const char *pw) {
	char *message = NULL;
	int status = 0;

	message = (char *)malloc(sizeof(char) * (strlen(id) + strlen(pw) + 2));
	strcpy(message, id);
	strcat(message, ":");
	strcat(message, pw);

	// Send ID/PW
	_send(sock, message);
	FREE(message);
	
	_recv(sock, message);
	status = message[0];
	FREE(message);
	
	return status;
}

int try_build(int sock, const char *file, char *result) {
	char *message = NULL, buf[BUF_SIZE] = { 0, };
	FILE *fp = NULL;
	int len = 0, status = 0;

	fp = fopen(file, "r");
	if (fp == NULL) {
		fputs("Failed to open file!\n", stderr);
		exit(EXIT_FAILURE);
	}

	message = (char *)malloc(sizeof(char));
	message[0] = 0x0;
	while(fgets(buf, BUF_SIZE - 1, fp)) {
		len += strlen(buf);
		message = (char *)realloc(message, sizeof(char) * (len + 1));
		strcat(message, buf);
	}

	// Send file
	_send(sock, message);
	FREE(message);

	_recv(sock, message);
	status = message[0];
	
	if (result != NULL) {
		FREE(result);
	}
	result = strdup(message + 1);
	FREE(message);
	return status;
}

int try_test(Data *info, char *result) {
	char *message = NULL;
	int sock = 0, status = 0;

	sock = connect_to_server(info);
	_recv(sock, message);
	
	status = message[0];
	if (result != NULL) {
		FREE(result);
	}
	result = strdup(message + 1);
	FREE(message);

	shutdown(sock, SHUT_RDWR);
	return status;
}

int connect_to_server(Data *information) {
	int sock = 0, status = 0;
	struct sockaddr_in serv_addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock <= 0) {
		perror("socket failed: ");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons((information->network).port);
	if (inet_pton(AF_INET, (information->network).ip, &serv_addr.sin_addr) <= 0) {
		perror("inet_pton failed: ");
		exit(EXIT_FAILURE);
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect failed: ");
		exit(EXIT_FAILURE);
	}

	status = try_login(sock, (information->auth).id, (information->auth).pw);
	if (status == LOGIN_FAILED) {
		printf("Login failed. Check your ID and PW.\n");
		exit(EXIT_FAILURE);
	}

	return sock;
}
