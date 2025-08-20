CC = gcc
CFLAGS = -Iinclude -Iexternal/SDL2/include -Iexternal/SDL2_ttf/include
LDFLAGS = -Lexternal/SDL2/lib -Lexternal/SDL2_ttf/lib -lSDL2main -lSDL2 -lSDL2_ttf

SRC = src/*.c
OUT = kTextEditor.exe

all:
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(OUT)
	copy external\SDL2\lib\SDL2.dll .
	copy external\SDL2_ttf\lib\SDL2_ttf.dll .

clean:
	del $(OUT)
	del SDL2.dll SDL2_ttf.dll