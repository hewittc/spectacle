#include "common.h"
#include "device_iqfile.h"

int device_iqfile_config(device_t *dev, const uint64_t freq, const uint64_t rate)
{
	if (!dev) {
		return EXIT_FAILURE;
	}

	if (!dev->driver) {
		device_file_t *driver = NULL;
		driver = malloc(sizeof(device_file_t));

		float complex *buffer = NULL;
		buffer = malloc(sizeof(float complex) * IQFILE_BUFFER_SIZE);

		if (driver && buffer) {
			driver->path = NULL;
			driver->fp = NULL;
			driver->loop = false;
			dev->driver = driver;
			dev->type = IQFILE;
			dev->mode = MODE_RX;
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
	if (!dev || !dev->driver || dev->type != IQFILE) {
		return EXIT_FAILURE;
	} 

	if (dev->mode == MODE_OFF) {
		return EXIT_SUCCESS;
	} else if (dev->mode == MODE_RX) {
		return device_iqfile_rx(dev);
	} else if (dev->mode == MODE_TX) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int device_iqfile_rx(device_t *dev)
{
	if (!dev || !dev->driver || dev->type != IQFILE) {
		return EXIT_FAILURE;
	}

	uint64_t position = 0;
	uint8_t byte;
	device_file_t *driver = dev->driver;
	while (true) {
		fread(&byte, 1, 1, driver->fp);
		if (ferror(driver->fp)) {
			return EXIT_FAILURE;
		}
		if (feof(driver->fp)) {
			if (driver->loop) {
				rewind(driver->fp);
				position = 0;
			} else {
				return EXIT_SUCCESS;
			}
		}
		printf_iq(byte, position++);
	}

	return EXIT_SUCCESS;
}

int device_iqfile_open(device_t *dev, const char *path, const bool loop)
{
	if (!dev || !dev->driver || dev->type != IQFILE) {
		return EXIT_FAILURE;
	}

	if (device_iqfile_close(dev)) {
		return EXIT_FAILURE;
	}

	FILE *fp = fopen(path, "rb");
	if (!fp) {
		return EXIT_FAILURE;
	}

	device_file_t *driver = dev->driver;
	driver->path = strdup(path);
	driver->fp = fp;
	driver->loop = loop;

	return EXIT_SUCCESS;
}

int device_iqfile_close(device_t *dev)
{
	if (!dev || !dev->driver || dev->type != IQFILE) {
		return EXIT_FAILURE;
	}

	device_file_t *driver = dev->driver;
	if (driver->path) {
		free(driver->path);
		driver->path = NULL;
	}
	if (driver->fp) {
		if (fclose(driver->fp)) {
			return EXIT_FAILURE;
		}
		driver->fp = NULL;
	}

	return EXIT_SUCCESS;
}

