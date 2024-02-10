MKDIR := python mkdir.py
RMDIR := python rmdir.py

CC := gcc
CFLAGS := -Wall -pedantic -std=c11 -Werror -g -O1 -Iinc
LFLAGS := -Llib -lmingw32 -lSDL2main -lSDL2

SRC_DIR := src
OBJ_DIR := build
OUT_DIR := out

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

EXE := snake.exe
TARGET := $(OUT_DIR)/$(EXE)

build: $(TARGET)

$(TARGET): $(OBJ)
	@$(MKDIR) $(OUT_DIR)
	$(CC) $^ $(LFLAGS) -o$@ 
	@copy SDL2.dll $(OUT_DIR) > NUL

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR) $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o$@ 

clean:
	@$(RMDIR) $(OBJ_DIR) $(OUT_DIR)

run: $(TARGET)
	.\$(TARGET)

.PHONY:
	make clean run
