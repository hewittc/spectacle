#include "common.h"
#include "device.h"
#include "device_hackrf.h"
#include "device_iqfile.h"

device_iface_t devices[] = {
	{ NULL, NULL },					/* DUMMY */
	{ device_iqfile_config, device_iqfile_xfer },	/* IQFILE */
	{ device_hackrf_config, device_hackrf_xfer },	/* HACKRF */
};

int printf_iq(uint8_t byte, uint64_t position)
{
	if (!(position % 16)) { 
		printf("%" PRIu64 ": ", position);
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

