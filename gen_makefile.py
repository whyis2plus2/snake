import platform
import os

if os.path.exists("makefile"):
    os.remove("makefile")

mk = open("makefile", 'w')

if platform.system() == "Windows":
    mk.write("""
MKDIR := python mkdir.py
RMDIR := python rmdir.py

CC := gcc
CFLAGS := -Wall -pedantic -std=c11 -Werror -g -O1 -Iinc
LFLAGS := -Llib/SDL2 -lmingw32 -lSDL2main -lSDL2 -mwindows

SRC_DIR := src
OBJ_DIR := build
OUT_DIR := out

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

EXE := snake.exe
TARGET := $(OUT_DIR)/$(EXE)

build: $(TARGET)

$(TARGET): $(OBJ)
\t@$(MKDIR) $(OUT_DIR)
\t$(CC) $^ $(LFLAGS) -o$@ 
\t@copy SDL2.dll $(OUT_DIR) > NUL

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
\t@$(MKDIR) $(OBJ_DIR)
\t$(CC) $(CFLAGS) -c $< -o$@ 

clean:
\t@$(RMDIR) $(OBJ_DIR) $(OUT_DIR)

run: $(TARGET)
\t.\\$(TARGET)

.PHONY:
\tmake clean run
""")
else:
    mk.write("""
CC := gcc
CFLAGS := -Wall -pedantic -std=c11 -Werror -g -O1 -Iinc
LFLAGS := -lSDL2main -lSDL2

SRC_DIR := src
OBJ_DIR := build
OUT_DIR := out

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

EXE := snake
TARGET := $(OUT_DIR)/$(EXE)

build: $(TARGET)

$(TARGET): $(OBJ)
\t@mkdir -p $(OUT_DIR)
\t$(CC) $^ $(LFLAGS) -o$@ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
\t@mkdir -p $(OBJ_DIR)
\t$(CC) $(CFLAGS) -c $< -o$@ 

clean:
\trm -rf $(OBJ_DIR) $(OUT_DIR)

run: $(TARGET)
\t./$(TARGET)

.PHONY:
\tmake clean run
""")


mk.close()
