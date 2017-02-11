#ifndef COMMON_H
#define COMMON_H

#define _DEFAULT_SOURCE

#include <sys/time.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <complex.h>
#include <pthread.h>

#include <zmq.h>
#include <liquid/liquid.h>

float complex clog10f(float complex z);

#endif /* COMMON_H */

