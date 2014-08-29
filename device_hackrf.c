#include "common.h"

#include "device.h"
#include "device_hackrf.h"

volatile uint32_t byte_count = 0;

int device_hackrf_config(device_t* dev, const uint64_t freq, const uint64_t rate) {
	int result = HACKRF_SUCCESS;
	if( !dev->driver ) {
		hackrf_device* hackrf_dev = malloc(sizeof(hackrf_device*));
		if( hackrf_dev ) {
			dev->driver = hackrf_dev;
			dev->type = HACKRF;
			dev->mode = MODE_RX;
		} else {
			return EXIT_FAILURE;
		}
	}

	dev->freq = freq;
	dev->rate = rate;

	return result;
}

int device_hackrf_xfer(device_t* dev) {
	int result;
	unsigned int lna_gain=8, vga_gain=20, txvga_gain=0;

	if( !dev->driver ) {
		result = device_hackrf_config(dev, DEFAULT_FREQ, DEFAULT_RATE);
		if( result != HACKRF_SUCCESS ) {
			printf("device_hackrf_config() failed: %s (%d)\n", hackrf_error_name(result), result);
			return EXIT_FAILURE;
		}
	}

	hackrf_device* hackrf_dev = (hackrf_device*) dev->driver;

	result = hackrf_init();
	if( result != HACKRF_SUCCESS ) {
		printf("hackrf_init() failed: %s (%d)\n", hackrf_error_name(result), result);
		return EXIT_FAILURE;
	}

	result = hackrf_open(&hackrf_dev);
	if( result != HACKRF_SUCCESS ) {
		printf("hackrf_open() failed: %s (%d)\n", hackrf_error_name(result), result);
		return EXIT_FAILURE;
	}

	result = hackrf_set_sample_rate(hackrf_dev, (const double) dev->rate);
	if( result != HACKRF_SUCCESS ) {
		printf("hackrf_set_sample_rate() failed: %s (%d)\n", hackrf_error_name(result), result);
		return EXIT_FAILURE;
	} else {
		printf("set sampling rate: %0.2f MS/s\n", (float) dev->rate / 1e6);
	}

	if( dev->mode == MODE_RX ) {
		result = hackrf_set_vga_gain(hackrf_dev, vga_gain);
		result |= hackrf_set_lna_gain(hackrf_dev, lna_gain);
		result = hackrf_start_rx(hackrf_dev, rx_callback, NULL);

		if( result != HACKRF_SUCCESS ) {
			printf("hackrf_start_rx() failed: %s (%d)\n", hackrf_error_name(result), result);
			return EXIT_FAILURE;
		}
	} else {
		result = hackrf_set_txvga_gain(hackrf_dev, txvga_gain);
		result |= hackrf_start_tx(hackrf_dev, tx_callback, NULL);

		if( result != HACKRF_SUCCESS ) {
			printf("hackrf_start_tx() failed: %s (%d)\n", hackrf_error_name(result), result);
			return EXIT_FAILURE;
		}
	}

	result = hackrf_set_freq(hackrf_dev, (const uint64_t) dev->freq);
	if( result != HACKRF_SUCCESS ) {
		printf("hackrf_set_freq() failed: %s (%d)\n", hackrf_error_name(result), result);
		return EXIT_FAILURE;
	} else {
		printf("set frequency: %0.2f MHz\n", (float) dev->freq / 1e6);
	}

	printf("streaming!\n");
	while( hackrf_is_streaming(hackrf_dev) == HACKRF_TRUE ) {
		sleep(1);
	}

	result = hackrf_close(hackrf_dev);
	if( result != HACKRF_SUCCESS ) {
		printf("hackrf_close() failed: %s (%d)\n", hackrf_error_name(result), result);
		return EXIT_FAILURE;
	}

	return result;
}

int rx_callback(hackrf_transfer* transfer) {
	size_t bytes_to_write;
	int i;
	size_t bytes_written;

	byte_count += transfer->valid_length;
	bytes_to_write = transfer->valid_length;

	printf("reading %d bytes...\n", (int) bytes_to_write);

	for (i = 0; i < bytes_to_write; i++) {
		printf("%02x", transfer->buffer[i]);
		bytes_written++;
	}

	if (bytes_written != bytes_to_write) {
		printf("rx_callback() failed: read %d bytes, but expected %d bytes\n", (int) bytes_written, (int) bytes_to_write);
		return EXIT_FAILURE;
	} else {
		return HACKRF_SUCCESS;
	}
}

int tx_callback(hackrf_transfer* transfer) {
	return 0;
}

