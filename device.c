#include "common.h"
#include "device.h"
#include "device_iqfile.h"

device_iface devices[] = {
	{ dev_iqfile_setup, dev_iqfile_rx, dev_iqfile_tx, dev_iqfile_destroy },	/* IQFILE */
};

int printf_buffer(int8_t *buffer, size_t size)
{
	for (size_t i = 0; i < size;) {
		if (!(i % 16)) { 
			printf("%08"PRIx64": ", i);
		}
		printf("%02x", (uint8_t) buffer[i]);
		if (!(++i % 2)) {
			printf(" ");
		}
		if (!(i % 16) || i == size) {
			printf("\n");
		}
	}

	return EXIT_SUCCESS;
}

int printf_cbuffer(complex float *buffer, size_t size)
{
	for (size_t i = 0; i < size;) {
		if (!(i % 8)) { 
			printf("%08"PRIx64": ", i);
		}
		printf("%02x%02x", (uint8_t) crealf(buffer[i]), (uint8_t) cimagf(buffer[i]));
		if (!(++i % 8) || i == size) {
			printf("\n");
		} else {
			printf(" ");
		}
	}

	return EXIT_SUCCESS;
}

