#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

uid_t getUserIdByName(const char *name) {
	struct passwd *pwd = calloc(1, sizeof(struct passwd));
	if (pwd == NULL) {
		fprintf(stderr, "Failed to allocate struct passwd for getpwnam_r.\n");
		exit(1);
	}

	size_t buffer_len = sysconf(_SC_GETPW_R_SIZE_MAX) * sizeof(char);
	char *buffer = malloc(buffer_len);
	if (buffer == NULL) {
		fprintf(stderr, "Failed to allocate buffer for getpwnam_r.\n");
		exit(2);
	}

	getpwnam_r(name, pwd, buffer, buffer_len, &pwd);
	if (pwd == NULL) {
		fprintf(stderr, "getpwnam_r failed to find requests entry.\n");
		exit(3);
	}
	
	uid_t uid = pwd->pw_uid;
	free(pwd);
	free(buffer);
	
	return uid;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage <excutable file> <case> <param>\n");
		exit(-1);
	}

	FILE *fp = fopen("/proc/dogdoor", "w");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open /proc/dogdoor file!\n");
		exit(-1);
	}

	char buffer[128];
	int detector = atoi(argv[1]);
	int param = atoi(argv[2]);
	
	sprintf(buffer, "%d %d", detector, param);
	fputs(buffer, fp);
	return 0;
}
