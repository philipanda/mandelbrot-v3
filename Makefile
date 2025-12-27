CC:=gcc
COMMON_FLAGS:= -g
C_FLAGS:= $(COMMON_FLAGS)
L_FLAGS:= -lSDL3 $(COMMON_FLAGS)

MODULES:= main mandelbrot complex camera
OBJS := $(MODULES:%=%.o)
SRCS := $(MODULES:%=%.c)

all: $(OBJS)
	$(CC) $(OBJS) $(L_FLAGS) -o mandelbrot

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o mandelbrot