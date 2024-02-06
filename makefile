MKDIR := python mkdir.py
RMDIR := python rmdir.py

CC := gcc
CFLAGS := -Wall -pedantic -std=gnu2x -fms-extensions -Werror -g -O0 -Iinc -c
LFLAGS := -Llib -lmingw32 -lSDL2main -lSDL2

SRC_DIR := src
OBJ_DIR := build
OUT_DIR := out

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

EXE := test
TARGET := $(OUT_DIR)/$(EXE).exe

build: $(TARGET)

$(TARGET): $(OBJ)
	@$(MKDIR) $(OUT_DIR)
	$(CC) $^ $(LFLAGS) -o$@ 
	@copy SDL2.dll $(OUT_DIR) > NUL

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR) $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o$@ 

clean:
	@$(RMDIR) /s /q $(OBJ_DIR) $(OUT_DIR)

run: $(TARGET)
	.\$(TARGET)

.PHONY:
	make clean run
