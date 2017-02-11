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

	int bins = 1024;
	int flags = 0;

	complex float *buffer;
	buffer = calloc(bins, sizeof(complex float));

	complex float *transform;
	transform = calloc(bins, sizeof(complex float));

	float window[bins];
	for (int i = 0; i < bins; i++) {
		window[i] = hann(i, bins);
	}

	while (1) {
		zmq_recv(socket, buffer, bins * sizeof(complex float), 0);

		for (int i = 0; i < bins; i++) {
			buffer[i] = crealf(buffer[i]) * window[i] + I * cimagf(buffer[i]) * window[i];
		}

		fftplan q = fft_create_plan(bins, buffer, transform, LIQUID_FFT_FORWARD, flags);
		fft_execute(q);
		fft_destroy_plan(q);

		//printf_cbuffer(transform, bins);

		float mag;
		for (int i = bins / 2; i < bins; i++) {
			mag = 20.0 * clog10f(cabsf(transform[i]));
			printf("%f\n", mag);
		}
		for (int i = 0; i < bins / 2; i++) {
			mag = 20.0 * clog10f(cabsf(transform[i]));
			printf("%f\n", mag);
		}
	}

	free(transform);
	free(buffer);
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

