import platform
import os

if os.path.exists("makefile"):
    os.remove("makefile")

mk = open("makefile", 'w')

if platform.system() == "Windows":
    mk.write("""
        \rMKDIR := python mkdir.py
        \rRMDIR := python rmdir.py

        \rCC := gcc
        \rCFLAGS := -Wall -pedantic -std=c11 -Werror -g -O1 -Iinc
        \rLFLAGS := -Llib/SDL2 -lmingw32 -lSDL2main -lSDL2 -mwindows

        \rSRC_DIR := src
        \rOBJ_DIR := build
        \rOUT_DIR := out

        \rSRC := $(wildcard $(SRC_DIR)/*.c)
        \rOBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

        \rEXE := snake.exe
        \rTARGET := $(OUT_DIR)/$(EXE)

        \rbuild: $(TARGET)

        \r$(TARGET): $(OBJ)
        \r\t@$(MKDIR) $(OUT_DIR)
        \r\t$(CC) $^ $(LFLAGS) -o$@ 
        \r\t@copy SDL2.dll $(OUT_DIR) > NUL

        \r$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
        \r\t@$(MKDIR) $(OBJ_DIR)
        \r\t$(CC) $(CFLAGS) -c $< -o$@ 

        \rclean:
        \r\t@$(RMDIR) $(OBJ_DIR) $(OUT_DIR)

        \rrun: $(TARGET)
        \r\t.\$(TARGET)

        \r.PHONY:
        \r\tmake clean run
""")
else:
    mk.write("""
        \rCC := gcc
        \rCFLAGS := -Wall -pedantic -std=c11 -Werror -g -O1 -Iinc
        \rLFLAGS := -lSDL2main -lSDL2

        \rSRC_DIR := src
        \rOBJ_DIR := build
        \rOUT_DIR := out

        \rSRC := $(wildcard $(SRC_DIR)/*.c)
        \rOBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

        \rEXE := snake.exe
        \rTARGET := $(OUT_DIR)/$(EXE)

        \rbuild: $(TARGET)

        \r$(TARGET): $(OBJ)
        \r\t@mkdir -p $(OUT_DIR)
        \r\t$(CC) $^ $(LFLAGS) -o$@ 

        \r$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
        \r\t@mkdir -p $(OBJ_DIR)
        \r\t$(CC) $(CFLAGS) -c $< -o$@ 

        \rclean:
        \r\trm -rf $(OBJ_DIR) $(OUT_DIR)

        \rrun: $(TARGET)
        \r\t./$(TARGET)

        \r.PHONY:
        \r\tmake clean run
""")


mk.close()
