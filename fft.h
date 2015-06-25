#ifndef FFT_H
#define FFT_H

#include <math.h>
#include <liquid/liquid.h>

typedef enum {
	RECTANGLE = 0,
	HAMMING = 1,
	BLACKMAN = 2,
	HANN = 3,
	GAUSSIAN = 4,
	WELCH = 5,
	BARTLETT = 6
} fft_window;

int fft(float complex *, float complex *, unsigned int, int);

float window_rectangle(const float);
float window_hamming(const float);
float window_blackman(const float);
float window_hann(const float);
float window_gaussian(const float);
float window_welch(const float);
float window_bartlett(const float);

int apply_window(float complex *, float complex *, unsigned int, fft_window);

#endif /* FFT_H */

