CC=clang 
CFLAGS=-I/usr/local/include -std=c11 -g
LIBS=-lhackrf -lliquid -lm -lpthread -lzmq
DEPS=common.h device.h device_hackrf.h fft.h server.h
OBJ=device.o device_hackrf.o fft.o main.o server.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

spectacle: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJ) core
