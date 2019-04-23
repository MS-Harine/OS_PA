#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int pipes[2];

int main(int argc, char *argv[]) {
	pipe(pipes);

	if (fork() == 0) {
		// child	
		dup2(pipes[1], 1);
		printf("Change to gcc\n");
		execlp("gcc", "gcc", "-x c", "-", NULL);
	}
	else {
		// parent
		int s = strlen(argv[1]);
		int sent = 0;
		
		sleep(1);
		while (sent < s) {
			sent += write(pipes[1], argv[1] + sent, s - sent);
			printf("write ");
		}

		printf("end");
		char buf[2048] = { 0, };
		while((s = read(pipes[0], buf, 2047) > 0)) {
			buf[s + 1] = 0x0;
			printf(">%s\n", buf);
		}
	}

	return 0;
}
