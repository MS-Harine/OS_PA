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
char * get_auth_data(const char *, const char *);
int work(Data *, char **, int);

#endif
