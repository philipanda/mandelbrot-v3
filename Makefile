CC:=gcc
COMMON_FLAGS:= -g
C_FLAGS:= $(COMMON_FLAGS)
L_FLAGS:= -lSDL3 -lGLEW -lGL

SHADERS:=mandelbrot.frag.glsl mandelbrot.vert.glsl
MODULES:= main mandelbrot_camera complex text
OBJS := $(MODULES:%=%.o)
SRCS := $(MODULES:%=%.c)

all: mandelbrot gpu

gpu: gpu.c $(SHADERS)
	$(CC) gpu.c -o gpu $(C_FLAGS) $(L_FLAGS)

mandelbrot: $(OBJS)
	$(CC) $(OBJS) $(L_FLAGS) -o mandelbrot

%.o: %.c %.h
	$(CC) $(C_FLAGS) -c $< -o $@

clean:
	rm -f *.o mandelbrot gpu *.spv

