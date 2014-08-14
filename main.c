#include "common.h"
#include "device.h"

int main(int argc, char **argv) {
	device_t* dev = malloc(sizeof(device_t*));
	device_iface_t* dev_iface = &devices[HACKRF];

	dev_iface->init(dev);
	dev_iface->rx(dev);

	return 0;
}

