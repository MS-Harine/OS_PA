#ifndef	__SUBMITTER_H__
#define	__SUBMITTER_H__

struct _network_data {
	char *ip;
	int port;
};

struct _auth_data {
	char *id;
	char *pw;
};

struct _dataset {
	struct _network_data network;
	struct _auth_data auth;
	char *filename;
};

typedef struct _dataset Data;

int connect_to_server(Data *);
int try_login(int, const char *, const char *);
int try_build(int, const char *, char *);
int try_test(Data *, char *);

#endif
