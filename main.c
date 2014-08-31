#include "common.h"
#include "device.h"

int main(int argc, char **argv) {
	device_t* dev = malloc(sizeof(device_t*));
	device_iface_t* dev_iface = &devices[HACKRF];

	dev_iface->config(dev, 89.5e6, 20e6);
	dev_iface->xfer(dev);

	return 0;
}

