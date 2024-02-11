import platform
import os

# general consts
CC = "gcc"
CFLAGS = "-Wall -pedantic -std=c11 -Werror -g -O1 -Iinc"
SRC_DIR = "src"
OBJ_DIR = "build"
OUT_DIR = "out"
EXE = "snake"
SRC = f"$(wildcard {SRC_DIR}/*.c)"
OBJ = f"$(patsubst {SRC_DIR}/%.c,{OBJ_DIR}/%.o,{SRC})"

# platform specific consts, to be modified
MKDIR = ""
RMDIR = ""
RUN = ""
LFLAGS = ""
EXE_EXT = ""
TARGET = ""

if platform.system() == "Windows":
    MKDIR = "python build_scripts/mkdir.py"
    RMDIR = "python build_scripts/rmdir.py"
    RUN = ".\\"
    LFLAGS = "-Llib/SDL2 -lmingw32 -lSDL2main -lSDL2 -mwindows"
    EXE_EXT = ".exe"
else:
    MKDIR = "mkdir -p"
    RMDIR = "rm -rf"
    RUN = "./"
    LFLAGS = "-lSDL2main -lSDL2"
    EXE_EXT = ""

TARGET = f"{OUT_DIR}/{EXE}{EXE_EXT}"

if os.path.exists("makefile"):
    os.remove("makefile")

mk = open("makefile", 'w')

mk.write(f"""
build: {TARGET}

{TARGET}: {OBJ}
\t@{MKDIR} {OUT_DIR}
\t{CC} $^ {LFLAGS} -o$@ 
\t{f"@copy SDL2.dll {OUT_DIR} > NUL" if platform.system() == "Windows" else ""}

{OBJ_DIR}/%.o: {SRC_DIR}/%.c
\t@{MKDIR} {OBJ_DIR}
\t{CC} {CFLAGS} -c $< -o$@ 

clean:
\t{RMDIR} {OBJ_DIR} {OUT_DIR}

run: {TARGET}
\t{RUN}{TARGET}

.PHONY:
\tmake clean run build
""")

mk.close()
