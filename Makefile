BIN=c2048
CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wfloat-conversion -Wimplicit-fallthrough -pedantic -g `pkg-config --cflags freetype2` -I3rdparty/glad/include -I3rdparty/fmod/include -I3rdparty/stb/
OBJ=main.o game.o core.o shader.o text.o audio.o texture.o timer.o ui.o zephr.o zephr_math.o 3rdparty/glad/src/gl.o 3rdparty/glad/src/glx.o 3rdparty/stb/stb.o
LDFLAGS=`pkg-config --libs x11 xcursor freetype2` -lm -L3rdparty/fmod/lib -Wl,-rpath=3rdparty/fmod/lib -lfmod
DEPS=3rdparty/glad/include/glad/gl.h 3rdparty/glad/include/glad/glx.h 3rdparty/fmod/include/fmod.h 3rdparty/stb/stb_image.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm $(OBJ) $(BIN)

