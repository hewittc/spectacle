#include "common.h"
#include "device_iqfile.h"

int dev_iqfile_setup(device *dev, const uint64_t freq, const uint64_t rate)
{
	if (!dev) {
		return EXIT_FAILURE;
	}

	dev->driver = calloc(1, sizeof(dev_iqfile));
	dev->buffer = calloc(IQFILE_BUFFER_SIZE, sizeof(float complex));

	dev->type = IQFILE;
	dev->mode = MODE_RX;
	dev->freq = freq;
	dev->rate = rate;

	return EXIT_SUCCESS;
}

int dev_iqfile_xfer(device *dev)
{
	if (!dev || !dev->driver || dev->type != IQFILE) {
		return EXIT_FAILURE;
	}

	if (dev->mode == MODE_OFF) {
		return EXIT_SUCCESS;
	} else if (dev->mode == MODE_RX) {
		return dev_iqfile_rx(dev);
	} else if (dev->mode == MODE_TX) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int dev_iqfile_rx(device *dev)
{
	if (!dev || !dev->driver || dev->type != IQFILE) {
		return EXIT_FAILURE;
	}

	dev_iqfile *driver = (dev_iqfile *) dev->driver;

	uint64_t position = 0;
	uint8_t byte;

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

int dev_iqfile_open(device *dev, const char *path, const bool loop)
{
	if (!dev || !dev->driver || dev->type != IQFILE) {
		return EXIT_FAILURE;
	}
	printf("open: 0x%p\n", dev->driver);

	if (dev_iqfile_close(dev)) {
		return EXIT_FAILURE;
	}

	FILE *fp = fopen(path, "rb");
	if (!fp) {
		return EXIT_FAILURE;
	}

	dev_iqfile *driver = (dev_iqfile *) dev->driver;

	driver->path = strdup(path);
	driver->fp = fp;
	driver->loop = loop;

	return EXIT_SUCCESS;
}

int dev_iqfile_close(device *dev)
{
	if (!dev || !dev->driver) {
		return EXIT_FAILURE;
	}
	printf("close: 0x%p\n", dev->driver);

	dev_iqfile *driver = (dev_iqfile *) dev->driver;

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

