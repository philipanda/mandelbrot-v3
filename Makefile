CC:=gcc
COMMON_FLAGS:= -g
C_FLAGS:= $(COMMON_FLAGS)
L_FLAGS:= -lSDL3

MODULES:= main mandelbrot complex camera text
OBJS := $(MODULES:%=%.o)
SRCS := $(MODULES:%=%.c)

all: $(OBJS)
	$(CC) $(OBJS) $(L_FLAGS) -o mandelbrot

%.o: %.c %.h
	$(CC) $(C_FLAGS) -c $< -o $@

clean:
	rm -f *.o mandelbrot