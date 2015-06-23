CC     = clang
CFLAGS = -I/usr/local/include -std=c11 -g
LIBS   = -lhackrf -lliquid -lm -lpthread -lzmq
DEPS   = common.h device.h device_hackrf.h fft.h server.h
SRC    = device.c device_hackrf.c fft.c main.c server.c

EXE    = spectacle
OBJDIR = .
OBJ    = $(patsubst %,$(OBJDIR)/%,$(SRC:.c=.o))

all: $(EXE)

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

clean:
	rm -f $(OBJ) $(EXE) core

