CC     = clang
CFLAGS = -std=c11 -g -DDEBUG
LIBS   = -lliquid -lm -lpthread
DEPS   = common.h device.h device_iqfile.h fft.h 
SRC    = device.c device_iqfile.c fft.c main.c

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

