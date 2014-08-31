#include <stdlib.h>
#include "liquid/liquid.h"

int fft(float complex* in, float complex* out, unsigned int size, int flags) {
	fftplan fft_plan = fft_create_plan(size, in, out, LIQUID_FFT_FORWARD, flags);
	fft_execute(fft_plan);
	fft_destroy_plan(fft_plan);

	return 0;
}
