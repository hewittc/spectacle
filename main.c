#include "common.h"
#include "device.h"
#include "fft.h"

int main(int argc, char **argv)
{
	device_iface *dev_iface = &devices[IQFILE];

	device *dev = calloc(1, sizeof(*dev));
	if (dev) {
		dev_iface->setup(dev, 92e6, 20e6);
		dev_iqfile_open(dev, "./pager.iq", false);

		size_t bins = 512;

		char filename[255] = {0};
		for (size_t j = 0; j < 10; j++) {
			snprintf(filename, sizeof(filename), "results-%zu.txt", j);
			FILE *fp = fopen(filename, "w");

			dev_iface->rx(dev, bins);
			//printf_cbuffer(dev->buffer, bins);
			//printf("\n");

			complex float *out = calloc(bins, sizeof(complex float));
			apply_window(dev->buffer, out, bins, HANN);
			//printf_cbuffer(out, bins);
			//printf("\n");

			fft(out, out, bins, 0);
			//printf_cbuffer(out, bins);
			//printf("\n");

			for (size_t i = 0; i < bins; i++) {
				fprintf(fp, "%zu %f\n", i, 20.0 * log10f(cabsf(out[i])));
			}

			fclose(fp);
		}

		dev_iqfile_close(dev);
		dev_iface->destroy(dev);
	}

	return EXIT_SUCCESS;
}

