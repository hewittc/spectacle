#include "common.h"
#include "server.h"

static void* worker_ctrl_routine(void* context) {
	void* receiver = zmq_socket(context, ZMQ_REP);
	zmq_connect(receiver, WORKER_TRANSPORT);

	return 0;
}

static void* worker_data_routine(void* context) {
	void* receiver = zmq_socket(context, ZMQ_REP);
	zmq_connect(receiver, WORKER_TRANSPORT);

	return 0;
}

int start_server(const char* uri_clients) {
	void* context = zmq_ctx_new();
	void* clients = zmq_socket(context, ZMQ_ROUTER);
	zmq_bind(clients, uri_clients);

	void* workers = zmq_socket(context, ZMQ_DEALER);
	zmq_bind(workers, WORKER_TRANSPORT);

/*
	pthread_t worker_ctrl;
	pthread_t worker_data;
	pthread_create(&worker_ctrl, NULL, worker_ctrl_routine, context);
	pthread_create(&worker_data, NULL, worker_data_routine, context);

	zmq_proxy(clients, workers, NULL);
*/

	zmq_close(clients);
	zmq_close(workers);
	zmq_ctx_destroy(context);

	return 0;
}
