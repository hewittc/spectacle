#include "common.h"
#include "device.h"

int dev_iqfile_setup(device *dev, const uint64_t freq, const uint64_t rate)
{
	if (!dev) {
		return EXIT_FAILURE;
	}

	dev->context = calloc(1, sizeof(dev_iqfile));
	dev->type = IQFILE;
	dev->freq = freq;
	dev->rate = rate;
	dev->buffer_size = IQFILE_BUFFER_SIZE;

	return EXIT_SUCCESS;
}

int dev_iqfile_rx(device *dev, complex float *buffer, const size_t samples)
{
	if (!dev || !dev->context || dev->type != IQFILE) {
		return EXIT_FAILURE;
	}

	dev_iqfile *context = (dev_iqfile *) dev->context;

	size_t bytes_read = 0;
	size_t bytes_need = samples * 2;

	int8_t data[bytes_need];

	while (bytes_read < bytes_need) {
		size_t bytes = fread(data + bytes_read, sizeof(int8_t), bytes_need, context->fp);
		context->pos += bytes;
		
		if (ferror(context->fp)) {
			return EXIT_FAILURE;
		}
		if (feof(context->fp)) {
			if (context->loop) {
				context->pos = 0;
				rewind(context->fp);
			} else {
				break;
			}
		}
		
		bytes_read += bytes;
	}

	int j;
	for (size_t i = 0; i < samples; i++) {
		j = (2 * i);
		buffer[i] = data[j] + I * data[j + 1];
	}
	
	return EXIT_SUCCESS;
}

int dev_iqfile_open(device *dev, const char *path, const bool loop)
{
	if (!dev || !dev->context || dev->type != IQFILE) {
		return EXIT_FAILURE;
	}
#ifdef DEBUG
	printf("open: 0x%p\n", dev->context);
#endif
	dev_iqfile *context = (dev_iqfile *) dev->context;

	if (context->fp) {
		return EXIT_FAILURE;
	}

	FILE *fp = fopen(path, "rb");
	if (!fp) {
		return EXIT_FAILURE;
	}

	context->path = strdup(path);
	context->fp = fp;
	context->pos = 0;
	context->loop = loop;

	return EXIT_SUCCESS;
}

int dev_iqfile_close(device *dev)
{
	if (!dev || !dev->context) {
		return EXIT_FAILURE;
	}
#ifdef DEBUG
	printf("close: 0x%p\n", dev->context);
#endif
	dev_iqfile *context = (dev_iqfile *) dev->context;

	if (context->path) {
		free(context->path);
		context->path = NULL;
	}
	if (context->fp) {
		if (fclose(context->fp)) {
			return EXIT_FAILURE;
		}
		context->fp = NULL;
	}

	return EXIT_SUCCESS;
}

int dev_iqfile_destroy(device *dev)
{
        free(dev->context);
        free(dev);

        return EXIT_SUCCESS;
}

