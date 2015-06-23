#include "common.h"
#include "device_iqfile.h"

float complex* iqfile_buffer = NULL;

int device_iqfile_config(device_t* dev, const uint64_t freq, const uint64_t rate)
{
	if (!dev->driver) {
		FILE* iqfile_fp = NULL;

		iqfile_buffer = malloc(sizeof(float complex) * IQFILE_BUFFER_SIZE);

		if (iqfile_buffer) {
			dev->driver = iqfile_fp;
			dev->type = IQFILE;
			dev->mode = MODE_OFF;
		} else {
			return EXIT_FAILURE;
		}
	}

	dev->freq = freq;
	dev->rate = rate;

	return EXIT_SUCCESS;
}

int device_iqfile_xfer(device_t* dev)
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

int device_iqfile_path(device_t* dev, const char* path)
{
	return EXIT_FAILURE;
}
