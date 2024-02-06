MKDIR := py mkdir.py

CC := gcc
CFLAGS := -Wall -Wdeclaration-after-statement -pedantic -std=c99 -Werror -g -O0

INCLUDE := -ID:/Coding/.devel/MSYS2/ucrt64/include
LINK_PATH := -LD:/Coding/.devel/MSYS2/ucrt64/lib

COMP_FLAGS := $(CFLAGS) $(INCLUDE) -c
LINK_FLAGS := $(CFLAGS) $(LINK_PATH)

SRC_DIR := src
OBJ_DIR := build
OUT_DIR := out

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

EXE := test
TARGET := $(OUT_DIR)/$(EXE).exe

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(MKDIR) $(OUT_DIR)
	$(CC) $(LINK_FLAGS) $^ -o$@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR) $(OBJ_DIR)
	$(CC) $(COMP_FLAGS) $< -o$@ 

clean:
	rmdir /s /q $(OBJ_DIR) $(OUT_DIR)

run: $(TARGET)
	./$(TARGET)

.PHONY:
	make clean run
