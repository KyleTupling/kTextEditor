CC = gcc
CFLAGS = -Iinclude -Iexternal/SDL2/include -Iexternal/SDL2_ttf/include
LDFLAGS = -Lexternal/SDL2/lib -Lexternal/SDL2_ttf/lib -lSDL2main -lSDL2 -lSDL2_ttf

SRC = src/*.c
BUILD_DIR = build
OUT = $(BUILD_DIR)/kTextEditor.exe

all:
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(OUT)
	copy external\SDL2\lib\SDL2.dll $(BUILD_DIR)
	copy external\SDL2_ttf\lib\SDL2_ttf.dll $(BUILD_DIR)
	copy test.txt $(BUILD_DIR)
	@if not exist $(BUILD_DIR)\resources mkdir $(BUILD_DIR)\resources
	xcopy /E /I /Y resources $(BUILD_DIR)\resources

clean:
	del $(OUT)
	del $(BUILD_DIR)/SDL2.dll $(BUILD_DIR)/SDL2_ttf.dll