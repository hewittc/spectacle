#include "common.h"
#include "device_iqfile.h"

int device_iqfile_config(device_t *dev, const uint64_t freq, const uint64_t rate)
{
	if (!dev->driver) {
		float complex *buffer = NULL;
		buffer = malloc(sizeof(float complex) * IQFILE_BUFFER_SIZE);

		if (buffer) {
			dev->driver = NULL;
			dev->type = IQFILE;
			dev->mode = MODE_OFF;
			dev->buffer = buffer;
		} else {
			return EXIT_FAILURE;
		}
	}

	dev->freq = freq;
	dev->rate = rate;

	return EXIT_SUCCESS;
}

int device_iqfile_xfer(device_t *dev)
{
	if (!dev->driver) {
		return EXIT_FAILURE;
	} else {
		if (dev->mode == MODE_OFF) {
			return EXIT_SUCCESS;
		} else if (dev->mode == MODE_RX) {
			return EXIT_SUCCESS;
		} else if (dev->mode == MODE_TX) {
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int device_iqfile_open(device_t *dev, const char *path)
{
	return EXIT_FAILURE;
}

int device_iqfile_close(device_t *dev)
{
	return EXIT_FAILURE;
}
