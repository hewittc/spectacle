#ifndef SERVER_H
#define SERVER_H

#include <zmq.h>

#define WORKER_TRANSPORT "inproc://workers"

static void* worker_ctrl_routine(void*);
static void* worker_data_routine(void*);
int start_server(const char*);

#endif /* SERVER_H */

