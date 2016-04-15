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
	dev->freq = freq;
	dev->rate = rate;

	return EXIT_SUCCESS;
}

int dev_iqfile_rx(device *dev, size_t samples)
{
	if (!dev || !dev->driver || dev->type != IQFILE) {
		return EXIT_FAILURE;
	}

	dev_iqfile *driver = (dev_iqfile *) dev->driver;

	size_t bytes_read = 0;
	size_t bytes_need = samples * 2;
	int8_t buffer[bytes_need];

	while (bytes_read < bytes_need) {
		size_t bytes = fread(&buffer + bytes_read, sizeof(int8_t), bytes_need, driver->fp);
		driver->pos += bytes;

		if (ferror(driver->fp)) {
			return EXIT_FAILURE;
		}
		if (feof(driver->fp)) {
			if (driver->loop) {
				driver->pos = 0;
				rewind(driver->fp);
			} else {
				break;
			}
		}
		
		bytes_read += bytes;
	}

	for (size_t i = 0; i < samples; i++) {
		dev->buffer[i] = (complex float) buffer[(2 * i)] + buffer[(2 * i) + 1] * I;
	}
	
	return EXIT_SUCCESS;
}

int dev_iqfile_tx(device *dev)
{
	return EXIT_SUCCESS;
}

int dev_iqfile_open(device *dev, const char *path, const bool loop)
{
	if (!dev || !dev->driver || dev->type != IQFILE) {
		return EXIT_FAILURE;
	}
	printf("open: 0x%p\n", dev->driver);

	dev_iqfile *driver = (dev_iqfile *) dev->driver;

	if (driver->fp) {
		return EXIT_FAILURE;
	}

	FILE *fp = fopen(path, "rb");
	if (!fp) {
		return EXIT_FAILURE;
	}

	driver->path = strdup(path);
	driver->fp = fp;
	driver->pos = 0;
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

int dev_iqfile_destroy(device *dev)
{
        free(dev->buffer);
        free(dev->driver);
        free(dev);

        return EXIT_SUCCESS;
}

int dev_iqfile_printf(device *dev, size_t size)
{
	complex float *buffer = dev->buffer;

        for (size_t i = 0, j = 0; i < size;) {
                if (!(i % 8)) {
                        printf("%08"PRIx64": ", 16 * j++);
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

