CC     = clang
CFLAGS = -std=c11 -o0 -g -DDEBUG
LIBS   = -lm -lpthread -levent -lzmq -lliquid 
SRCS   = device.c device_iqfile.c http.c main.c
DEPS   = common.h device.h device_iqfile.h 

EXE    = spectacle
OBJDIR = .
OBJ    = $(patsubst %,$(OBJDIR)/%,$(SRCS:.c=.o))

all: $(EXE)

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

clean:
	rm -f $(OBJ) $(EXE) core

