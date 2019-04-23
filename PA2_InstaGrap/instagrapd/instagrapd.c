#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "instagrapd.h"
#include "protocol.h"

AuthData auth[AUTHDATA_LEN] = { 0, };
int authCount = 0;

int _send(int sock, char *message, int len) {
	DPRINT(printf("> _send | sock: %d\n", sock));
	DPRINT(printf("> message\n%s\n> message end\n", message));
	DPRINT(if (len != 0) printf("> message\n%d%d%d%d%s\n> message end\n", message[0], message[1], message[2], message[3], message + 4));

	int s = 0, result  = 0;
	char *orig = message;

	if (len == 0)
		len = strlen(orig);

	while (len > 0 && (s = send(sock, orig, len, 0)) > 0) {
		orig += s;
		len -= s;
		result += s;
	}

	DPRINT(printf("< _send | %d byte\n", result));
	return result;
}

int _recv(int sock, char **message) {
	DPRINT(printf("> _recv | sock: %d\n", sock));

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
		fputs("Failed to receive message.\n", stderr);
		exit(EXIT_FAILURE);
	}

	DPRINT(printf("< recv message\n%s\n< recv message end\n", *message));
	DPRINT(printf("< _recv\n"));
	return len;
}

int authenticate(char *message) {
	DPRINT(printf("> authenticate\n"));

	int status = 0, i = 0;
	char *id = NULL, *pw = NULL;

	id = (char *)malloc(sizeof(char) * 9);
	pw = (char *)malloc(sizeof(char) * 9);
	
	strncpy(id, message, 8);
	id[8] = 0x0;
	strncpy(pw, message + 9, 8);
	pw[8] = 0x0;

	status = -1;
	for (i = 0; i < authCount; i++) {
		if (!strcmp(id, auth[i].id)) {
			if (!strcmp(pw, auth[i].pw))
				status = LOGIN_SUCCESS;
			else
				status = LOGIN_FAILED;
		}
	}

	// TODO: Synchronization
	if (status == -1) {
		auth[authCount].id = strdup(id);
		auth[authCount].pw = strdup(pw);
		authCount = (authCount + 1) % AUTHDATA_LEN;
		if (auth[authCount].id != NULL) {
			FREE(auth[authCount].id);
			FREE(auth[authCount].pw);
		}
		status = LOGIN_SUCCESS;
	}

	DPRINT(printf("< authenticate\n"));
	return status;
}

int open_server(Data *data, struct sockaddr_in *address) {
	DPRINT(printf("> open_server\n"));

	int sock = 0;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket failed: ");
		exit(EXIT_FAILURE);
	}

	memset(address, 0, sizeof(*address));
	address->sin_family = AF_INET;
	address->sin_addr.s_addr = INADDR_ANY;
	address->sin_port = htons((data->this).port);
	if (bind(sock, (struct sockaddr *)address, sizeof(*address)) < 0) {
		perror("bind failed: ");
		exit(EXIT_FAILURE);
	}

	DPRINT(printf("< open_server\n"));
	return sock;
}

int connect_to_worker(Data *data) {
	DPRINT(printf("> connect_to_worker\n"));

	int sock = 0;
	struct sockaddr_in address;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket failed: ");
		exit(EXIT_FAILURE);
	}
	
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons((data->worker).port);
	if (inet_pton(AF_INET, (data->worker).ip, &address.sin_addr) <= 0) {
		perror("inet_pton failed: ");
		exit(EXIT_FAILURE);
	}

	if (connect(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("connect failed: ");
		exit(EXIT_FAILURE);
	}

	DPRINT(printf("< connect_to_worker\n"));
	return sock;
}

char * concat(const char *p, int n, const char *str, int add_slash) {
	DPRINT(printf("> concat\n"));

	char *r = (char *)malloc(sizeof(char) * 20);
	if (p == NULL)
		sprintf(r, "%d%s", n, str);
	else {
		if (add_slash)
			sprintf(r, "%s/%d%s", p, n, str);
		else
			sprintf(r, "%s%d%s", p, n, str);
	}
	r = (char *)realloc(r, sizeof(char) * strlen(r));

	DPRINT(printf("< concat\n"));
	return r;
}

char * trim(char *string) {
	char *temp = NULL, *start = string, *end = string + strlen(string) - 1;
	
	while (*start != 0x0 && isspace(*start))
		++start;
	
	while (end != string && isspace(*end))
		--end;

	temp = (char *)malloc(sizeof(char) * (strlen(start) - strlen(end) + 2));
	strncpy(temp, start, strlen(start) - strlen(end) + 1);
	temp[strlen(start) - strlen(end) + 1] = 0x0;
	free(string);
	return temp;
}

int check_answer(char *result, char *answer_file) {
	DPRINT(printf("> check_answer\n"));

	int len = 0, status = 0, i = 0;
	char *answer = NULL, buf[BUF_SIZE] = { 0, };
	FILE *fp = NULL;

	fp = fopen(answer_file, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open file %s\n", answer_file);
		exit(EXIT_FAILURE);
	}

	len = 0;
	answer = (char *)malloc(sizeof(char));
	answer[0] = 0;
	while (fgets(buf, BUF_SIZE - 1, fp)) {
		len += strlen(buf);
		answer = (char *)realloc(answer, sizeof(char) * (strlen(answer) + len + 1));
		strcat(answer, buf);
	}
	fclose(fp);

	result = trim(result);
	answer = trim(answer);
	if (!strcmp(result, answer))
		status = TEST_SUCCESS;
	else
		status = TEST_FAILED;
	FREE(answer);

	DPRINT(printf("< check_answer %d\n", status));
	return status;
}

void * work(void *data) {
	DPRINT(printf("> work\n"));

	struct _thread_args *arg = (struct _thread_args *)data;
	int client = arg->sock;
	Data *info = arg->data;
	char *test_dir = info->filepath;

	int status = 0, i = 0, sum = 0, worker = 0;
	char *message = NULL, *content = NULL, *result = NULL, *testcase = NULL, *label = NULL;
	
	_recv(client, &message);
	shutdown(client, SHUT_RD);
	status = authenticate(message);

	if (status == LOGIN_FAILED) {
		result = (char *)malloc(sizeof(char) * 2);
		result[0] = status;
		result[1] = 0x0;
		
		_send(client, result, 0);
		shutdown(client, SHUT_WR);
		FREE(result);
		FREE(message);
		FREE(data);
		return NULL;
	}

	content = strdup(message + 17);
	FREE(message);
	
	// TODO: Check for testing


	// Test
	for (i = 1; i <= 10; i++) {
		worker = connect_to_worker(info);
		testcase = concat(test_dir, i, ".in", 1);
		label = concat(test_dir, i, ".out", 1);
		status = try_process(worker, content, testcase, label, &result);
		
		if (status == BUILD_FAILED) {
			message = (char *)malloc(sizeof(char) * (strlen(result) + 2));
			message[0] = status;
			strcpy(message + 1, result);
			message[strlen(message + 1)] = 0x0;

			_send(client, message, 0);
			shutdown(client, SHUT_WR);
			FREE(message);
			return NULL;
		}

		if (status == TEST_TIMEOUT) {
			message= (char *)malloc(sizeof(char) * 2);
			sprintf(message, "%d", status);
			_send(client, message, 0);
			shutdown(client, SHUT_WR);
			FREE(message);
			break;
		}

		status = check_answer(result, label);

		if (status == TEST_SUCCESS)
			sum++;
	}
	
	if (sum != 10)
		status = TEST_FAILED;

	message = (char *)malloc(sizeof(char) * 4);
	sprintf(message, "%d%d", status, sum);
	message[0] -= '0';
	_send(client, message, 0);
	shutdown(client, SHUT_WR);
	FREE(message);

	DPRINT(printf("< work\n"));
	return NULL;
}

int try_process(int worker, const char *c_content, const char *testcase, const char *label, char **result) {
	DPRINT(printf("> try_process\n"));

	char *message = NULL, *content = NULL, buf[BUF_SIZE] = { 0, };
	int len = 0, status = 0, i = 0;
	FILE *fp = NULL;
	union _type_caster caster;

	if (testcase == NULL)
		message = (char *)malloc(sizeof(char) * (strlen(c_content) + sizeof(caster) + 2));
	else {
		fp = fopen(testcase, "r");
		if (fp == NULL) {
			fputs("Failed to open testcase!\n", stderr);
			exit(EXIT_FAILURE);
		}

		content = (char *)malloc(sizeof(char));
		content[0] = 0x0;
		while(fgets(buf, BUF_SIZE - 1, fp)) {
			len += strlen(buf);
			content = (char *)realloc(content, sizeof(char) * (len + 1));
			strcat(content, buf);
		}
		fclose(fp);

		message = (char *)malloc(sizeof(char) * (strlen(c_content) + strlen(content) + sizeof(caster) + 1));
	}

	caster.length = strlen(c_content);
	for (i = 0; i < 4; i++)
		message[i] = caster.data[i];
	strcpy(message + 4, c_content);

	if (testcase == NULL) {
		message[strlen(c_content) + 4] = 0;
		message[strlen(c_content) + 5] = 0x0;
	}
	else {
		strcpy(message + strlen(c_content) + 4, content);
		FREE(content);
	}

	_send(worker, message, strlen(message + 4) + 4);
	shutdown(worker, SHUT_WR);
	FREE(message);
	
	_recv(worker, &message);
	shutdown(worker, SHUT_RD);
	status = message[0];
	
	*result = strdup(message + 1);
	FREE(message);

	DPRINT(printf("< try_process %d %s\n", status, *result));
	return status;
}

