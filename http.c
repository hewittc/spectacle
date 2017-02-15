#include "common.h"
#include "device.h"

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>

static void periodogram_request_cb(struct evhttp_request *req, void *args)
{
	struct evbuffer *evb;
	evb = evbuffer_new();

        device *dev = (device *) args;

        void *zmq_ctx;
        zmq_ctx = zmq_ctx_new();

        void *socket;
        socket = zmq_socket(zmq_ctx, ZMQ_SUB);
        
        zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0);

        while (!dev->buffer_size);

        zmq_connect(socket, "tcp://127.0.0.1:5555");

        int bins = 1024;
        int flags = 0;

        complex float *buffer;
        buffer = calloc(bins, sizeof(complex float));

        complex float *transform;
        transform = calloc(bins, sizeof(complex float));

        float window[bins];
        for (int i = 0; i < bins; i++) {
                window[i] = hann(i, bins);
        }

	zmq_recv(socket, buffer, bins * sizeof(complex float), 0);

	for (int i = 0; i < bins; i++) {
		buffer[i] = crealf(buffer[i]) * window[i] + I * cimagf(buffer[i]) * window[i];
	}

	fftplan q = fft_create_plan(bins, buffer, transform, LIQUID_FFT_FORWARD, flags);
	fft_execute(q);
	fft_destroy_plan(q);

	//printf_cbuffer(transform, bins);

	float mag;
	for (int i = bins / 2; i < bins; i++) {
		mag = 20.0 * clog10f(cabsf(transform[i]));
		evbuffer_add_printf(evb, "%f\n", mag);
	}
	for (int i = 0; i < bins / 2; i++) {
		mag = 20.0 * clog10f(cabsf(transform[i]));
		evbuffer_add_printf(evb, "%f\n", mag);
	}

	evhttp_send_reply(req, 200, "OK", evb);

        free(transform);
        free(buffer);
}

void *serve_http(void *args)
{
	struct event_base *base;
	base = event_base_new();
	if (!base) {
		fprintf(stderr, "error: could not create event base\n");
	}

	struct evhttp *http;
	http = evhttp_new(base);
	if (!http) {
		fprintf(stderr, "error: could not create request handler\n");
	}

	evhttp_set_cb(http, "/periodogram", periodogram_request_cb, args);

	struct evhttp_bound_socket *handle;
	handle = evhttp_bind_socket_with_handle(http, "0.0.0.0", 5000);
	if (!handle) {
		fprintf(stderr, "error: could not bind to port\n");
	}

	event_base_dispatch(base);

	while(1);
}
