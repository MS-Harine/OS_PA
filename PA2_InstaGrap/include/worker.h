#ifndef __WORKER_H__
#define __WORKER_H__

int open_server(int, struct sockaddr_in *);
int task(int, pthread_mutex_t *);

#endif
