#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "submitter.h"
#include "protocol.h"

int _send(int sock, char *message) {
	DPRINT(printf("> _send\n"));
	DPRINT(printf("> message\n%s\n> message end\n", message));

	int len = 0, s = 0, result  = 0;
	char *orig = message;

	len = strlen(orig);
	while (len > 0 && (s = send(sock, orig, len, 0)) > 0) {
		orig += s;
		len -= s;
		result += s;
	}

	DPRINT(printf("< _send\n"));
	return result;
}

int _recv(int sock, char *message[]) {
	DPRINT(printf("> _recv\n"));
	
	int len = 0, s = 0;
	char buf[BUF_SIZE];
	
	if (*message != NULL) {
		FREE(*message);
	}

	while ((s = recv(sock, buf, BUF_SIZE - 1, 0)) > 0) {
		buf[s] = 0x0;
		if (*message == NULL) {
			*message = strdup(buf);
			len = s;
		}
		else {
			*message = realloc(*message, len + s + 1);
			strncpy(*message + len, buf, s);
			*message[len + s] = 0x0;
			len += s;
		}
	}

	if (len <= 0) {
		fputs("Failed to receive message from server.\n", stderr);
		exit(EXIT_FAILURE);
	}

	DPRINT(printf("< recv message\n%s\n< recv message end\n", *message));
	DPRINT(printf("< _recv\n"));
	return len;
}

char * get_auth_data(const char *id, const char *pw) {
	DPRINT(printf("> get_auth_data\n"));

	char *message = NULL;

	message = (char *)malloc(sizeof(char) * (strlen(id) + strlen(pw) + 2));
	strcpy(message, id);
	strcat(message, ":");
	strcat(message, pw);
	
	DPRINT(printf("< get_auth_data\n"));
	return message;
}

int work(Data *info, char *result[], int is_build) {
	DPRINT(printf("> work\n"));

	char *message = NULL, buf[BUF_SIZE] = { 0, };
	FILE *fp = NULL;
	int len = 0, status = 0, sock = 0;

	sock = connect_to_server(info);

	message = get_auth_data((info->auth).id, (info->auth).pw);
	if (is_build) {
		fp = fopen(info->filename, "r");
		if (fp == NULL) {
			fprintf(stderr, "Failed to open file %s\n", info->filename);
			exit(EXIT_FAILURE);
		}

		len = 0;
		while(fgets(buf, BUF_SIZE - 1, fp)) {
			len += strlen(buf);
			message = (char *)realloc(message, sizeof(char) * (strlen(message) + len + 1));
			strcat(message, buf);
		}
		fclose(fp);
	}
	else {
		message = (char *)realloc(message, sizeof(char) * (strlen(message) + 2));
		message[strlen(message) + 1] = 0x0;
	}

	_send(sock, message);
	shutdown(sock, SHUT_WR);
	FREE(message);

	_recv(sock, &message);
	shutdown(sock, SHUT_RD);
	status = message[0];

	*result = strdup(message + 1);
	FREE(message);

	DPRINT(printf("< work\n"));
	return status;
}

int connect_to_server(Data *information) {
	DPRINT(printf("> connect_to_server\n"));

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

	DPRINT(printf("< connect_to_server\n"));
	return sock;
}
