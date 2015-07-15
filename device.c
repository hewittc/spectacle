#include "common.h"
#include "device.h"
#include "device_hackrf.h"
#include "device_iqfile.h"

device_iface devices[] = {
	{ dev_iqfile_setup, dev_iqfile_xfer, dev_destroy },	/* IQFILE */
	{ dev_hackrf_setup, dev_hackrf_xfer, dev_destroy },	/* HACKRF */
};

int dev_destroy(device *dev)
{
	free(dev->driver);
	free(dev->buffer);
	free(dev);

	return EXIT_SUCCESS;
}

int printf_iq(uint8_t byte, uint64_t position)
{
	if (!(position % 16)) { 
		printf("%08"PRIx64": ", position);
	}
	printf("%02x", byte);
	if (!(++position % 2)) {
		printf(" ");
	}
	if (!(position % 16)) {
		printf("\n");
	}

	return EXIT_SUCCESS;
}

