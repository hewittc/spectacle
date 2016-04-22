#include "common.h"
#include "fft.h"

int fft(float complex *buffer, unsigned int size, int flags)
{
	fftplan fft_plan = fft_create_plan(size, buffer, buffer, LIQUID_FFT_FORWARD, flags);
	fft_execute(fft_plan);
	fft_destroy_plan(fft_plan);

	return EXIT_SUCCESS;
}

float window_rectangle(const float pos)
{
	return 1.0;
}

float window_hamming(const float pos)
{
	return 0.54 + 0.46 * cos(M_PI * pos);
}

float window_blackman(const float pos)
{
	return 0.42 + 0.50 * cos(M_PI * pos) + 0.08 * cos(2.0 * M_PI * pos);
}

float window_hann(const float pos)
{
	float x = cos(pos * M_PI / 2.0);
	return x * x;
}

float window_gaussian(const float pos)
{
	float a = 4.0;
	float ax = a * pos;
	return exp(-0.5 * ax * ax);
}

float window_welch(const float pos)
{
	return 1.0 - pos * pos;
}

float window_bartlett(const float pos)
{
	if (pos == 0.0) {
		return 1.0;
	} else if (pos > -1.0 && pos < 0.0) {
		return pos + 1.0;
	} else if (pos > 0.0 && pos < 1.0) {
		return 1.0 - pos;
	} else {
		return 0.0;
	}
}

int apply_window(float complex *buffer, size_t size, fft_window window)
{
	float (*window_function)(const float);
	int i;

	switch (window) {
	case HAMMING:
		window_function = window_hamming;
		break;
	case BLACKMAN:
		window_function = window_blackman;
		break;
	case HANN:
		window_function = window_hann;
		break;
	case GAUSSIAN:
		window_function = window_gaussian;
		break;
	case WELCH:
		window_function = window_welch;
		break;
	case BARTLETT:
		window_function = window_bartlett;
		break;
	case RECTANGLE:
	default:
		window_function = window_rectangle;
		break;
	}

	float pos = -1.0;
	float inc = 2.0 / size;
	for (size_t i = 0; i < size; i++) {
		buffer[i] = crealf(buffer[i]) * window_function(pos) + I * cimag(buffer[i]) * window_function(pos);
		pos += inc;
	}

	return EXIT_SUCCESS;
}
