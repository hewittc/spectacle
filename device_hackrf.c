#include "common.h"

#include "device.h"
#include "device_hackrf.h"

int device_hackrf_init(device_t* dev) {
	int errcode;

	errcode = hackrf_init();
	printf("init [%s]\n", hackrf_error_name(errcode));

	hackrf_device* hackrf_dev;
	dev->handle = hackrf_dev;

	return errcode;
}

int device_hackrf_rx(device_t* dev) {
	int errcode;
	
	hackrf_device* hackrf_dev = dev->handle;

	errcode = hackrf_open(&hackrf_dev);
	printf("open [%s]\n", hackrf_error_name(errcode));

	uint64_t freq = 89500000;
	float freq_nice = (float) freq / 1e6;
	errcode = hackrf_set_freq(hackrf_dev, freq);
	printf("set freq: %0.2f MHz [%s]\n", freq_nice, hackrf_error_name(errcode));

	const double rate = 8e6;
	float rate_nice = (float) rate / 1e6;
	errcode = hackrf_set_sample_rate(hackrf_dev, rate);
	printf("set rate: %0.2f MS/s [%s]\n", rate_nice, hackrf_error_name(errcode));

	errcode = hackrf_close(hackrf_dev);
	printf("close [%s]\n", hackrf_error_name(errcode));

	return errcode;
}

int device_hackrf_tx(device_t* dev) {
	return 0;
}

