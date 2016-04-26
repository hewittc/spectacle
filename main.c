#include "common.h"
#include "device.h"
#include "fft.h"

static void *rx(void *request)
{
	device_iface *dev_iface = &devices[IQFILE];
	device *dev = calloc(1, sizeof(*dev));
	if (dev) {
		dev_iface->setup(dev, 92e6, 20e6);
		dev_iqfile_open(dev, "./pager.iq", true);

		zmq_msg_t msg;

		size_t bins = 512;
		complex float *buffer = calloc(bins, sizeof(complex float));

		size_t i = 0;
		while (1) {
			dev_iface->rx(dev, bins);

			memcpy(buffer, dev->buffer, sizeof(complex float) * bins);
			apply_window(buffer, bins, HANN);
			fft(buffer, bins, 0);

			zmq_msg_init_size(&msg, sizeof(complex float) * bins);
			memcpy(zmq_msg_data (&msg), buffer, sizeof(complex float) * bins);
			zmq_msg_send(&msg, request, 0);
	
			i++;
		}

		free(buffer);

		dev_iqfile_close(dev);
		dev_iface->destroy(dev);
	}

	return NULL;
}

int main(int argc, char **argv)
{
	void *zmq_ctx = zmq_ctx_new();
	void *push = zmq_socket(zmq_ctx, ZMQ_PUSH);
	zmq_bind(push, "tcp://127.0.0.1:5555");

	pthread_t thread_rx;
	pthread_create(&thread_rx, NULL, rx, push);
	pthread_join(thread_rx, NULL);

	zmq_ctx_destroy(zmq_ctx);

	return EXIT_SUCCESS;
}

