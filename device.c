#include "common.h"
#include "device.h"
#include "device_iqfile.h"

device_iface devices[] = {
	{ dev_iqfile_setup, dev_iqfile_rx, dev_iqfile_tx, dev_iqfile_destroy },	/* IQFILE */
};

int printf_cbuffer(complex float *buffer, size_t size)
{
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

