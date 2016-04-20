#include "common.h"
#include "device.h"
#include "fft.h"

static void *rx(void *request)
{
	device_iface *dev_iface = &devices[IQFILE];
	device *dev = calloc(1, sizeof(*dev));
	if (dev) {
		dev_iface->setup(dev, 92e6, 20e6);
		dev_iqfile_open(dev, "./pager.iq2", false);

		size_t bins = 512;
		complex float *out = calloc(bins, sizeof(complex float));

		size_t i = 0;
		while (1) {
			dev_iface->rx(dev, bins);

			apply_window(dev->buffer, out, bins, HANN);
			fft(out, out, bins, 0);

			for (size_t j = 0; j < bins; j++) {
				printf("%zu %zu %f\n", i, j, 20.0 * log10f(cabsf(out[j])));
			}

			i++;
		}

		dev_iqfile_close(dev);
		dev_iface->destroy(dev);
	}

	return NULL;
}

int main(int argc, char **argv)
{
	void *zmq_ctx = zmq_ctx_new();
	void *push = zmq_socket(zmq_ctx, ZMQ_PUSH);
	zmq_bind(push, "inproc://data-rx");

	pthread_t thread_rx;
	pthread_create(&thread_rx, NULL, rx, zmq_ctx);
	pthread_join(thread_rx, NULL);

	return EXIT_SUCCESS;
}

