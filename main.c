#include "common.h"
#include "device.h"

void *do_fft(void *args) {
	device *dev = (device *) args;

	void *zmq_ctx;
	zmq_ctx = zmq_ctx_new();

	void *socket;
	socket = zmq_socket(zmq_ctx, ZMQ_SUB);
	
	zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0);

	while (!dev->buffer_size);

	zmq_connect(socket, "tcp://127.0.0.1:5555");

	complex float *buffer;
	buffer = calloc(dev->buffer_size, sizeof(complex float));

	while (1) {
		zmq_recv(socket, buffer, dev->buffer_size * sizeof(complex float), 0);
		printf_cbuffer(buffer, dev->buffer_size);
	}
}

int main(int argc, char **argv)
{
	device *dev; 
	dev = calloc(1, sizeof(device));

	if (!dev) {
		printf("error: failed to allocate device memory\n");
		exit(EXIT_FAILURE);
	}

	dev->type = IQFILE;

	device_iface *dev_iface = &devices[dev->type];

	dev_iface->setup(dev, 92e6, 20e6);

	pthread_t rx;
	pthread_t fft;

	pthread_create(&rx, NULL, device_rx, dev);
	pthread_create(&fft, NULL, do_fft, dev);

	pthread_join(fft, NULL);
	pthread_join(rx, NULL);

	dev_iface->destroy(dev);

	return EXIT_SUCCESS;
}

