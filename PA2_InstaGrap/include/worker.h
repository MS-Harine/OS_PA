#ifndef __WORKER_H__
#define __WORKER_H__

extern int *file_count;

int open_server(int, struct sockaddr_in *);
int task(int, pthread_mutex_t *);

#endif
