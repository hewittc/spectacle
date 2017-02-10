#include "common.h"
#include "device.h"
#include "device_iqfile.h"

device_iface devices[] = {
	{ dev_iqfile_setup, dev_iqfile_rx, dev_iqfile_destroy },	/* IQFILE */
};

int printf_cbuffer(complex float *buffer, size_t size)
{
	uint8_t real;
	uint8_t imag;

        for (size_t i = 0, j = 0; i < size;) {
                if (!(i % 8)) {
                        printf("%08"PRIx64": ", 16 * j++);
                }

		real = (uint8_t) crealf(buffer[i]);
		imag = (uint8_t) cimagf(buffer[i]);

                printf("%02x%02x", real, imag);

                if (!(++i % 8) || i == size) {
                        printf("\n");
                } else {
                        printf(" ");
                }
        }

        return EXIT_SUCCESS;
}

void *device_rx(void *args)
{
	device *dev = (device *) args;
	device_iface *dev_iface = &devices[dev->type];

	if (!dev->context) {
		printf("error: missing device context\n");
		exit(EXIT_FAILURE);
	}

	void *zmq_ctx;
	zmq_ctx = zmq_ctx_new();

	void *socket; 
	socket = zmq_socket(zmq_ctx, ZMQ_PUB);

	zmq_bind(socket, "tcp://127.0.0.1:5555");

	if (dev->type == IQFILE) {
		dev_iqfile_open(dev, "pager.iq", true);
	}

	complex float *buffer;
	buffer = calloc(dev->buffer_size, sizeof(complex float));

	while (1) {
		dev_iface->rx(dev, buffer, dev->buffer_size);
		printf_cbuffer(buffer, dev->buffer_size);
		zmq_send(socket, buffer, dev->buffer_size, 0);
	}

	if (dev->type == IQFILE) {
		dev_iqfile_close(dev);
	}

	free(buffer);

	zmq_ctx_destroy(zmq_ctx);
}

