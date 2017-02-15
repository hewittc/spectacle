#include "common.h"
#include "device.h"

void *serve_http(void *);

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

	pthread_t thread_rx;
	pthread_t thread_http;

	pthread_create(&thread_rx, NULL, device_rx, dev);
	pthread_create(&thread_http, NULL, serve_http, dev);

	pthread_join(thread_http, NULL);
	pthread_join(thread_rx, NULL);

	dev_iface->destroy(dev);

	return EXIT_SUCCESS;
}

