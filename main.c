#include "common.h"
#include "device.h"

int main(int argc, char **argv) {
	device_t* dev = malloc(sizeof(device_t*));
	device_iface_t* dev_iface = &devices[HACKRF];

	dev_iface->config(dev);
	dev_iface->xfer(dev, 89500000, 8e6);

	return 0;
}

