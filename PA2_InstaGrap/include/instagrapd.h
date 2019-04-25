#ifndef	__INSTAGRAPD_H__
#define __INSTAGRAPD_H__

#define	AUTHDATA_LEN	30

struct _network_data {
	char *ip;
	int port;
};

struct _dataset {
	struct _network_data this;
	struct _network_data worker;
	char *filepath;
};

struct _authdata {
	char *id;
	char *pw;
};

struct _thread_args {
	struct _dataset *data;
	int sock;
};

typedef struct _authdata AuthData;
typedef struct _dataset Data;

int open_server(Data *, struct sockaddr_in *);
int connect_to_worker(Data *);
void * work(void *);
int try_process(int, const char *, const char *, char **);

#endif
