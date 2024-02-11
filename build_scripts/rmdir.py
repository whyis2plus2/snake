''' A program made because mkdir exits the makefile when the directory does not exist '''

if (__name__ != "__main__"): raise Exception("Not a library")

from sys import argv
import sys
import os
import shutil

if (len(argv) < 2): print("Please input at least one argument"); sys.exit(-1)

dirs = argv.copy(); dirs.pop(0)

for dir in dirs:
    i = os.path.normpath(dir)

    if (os.path.exists(dir)):
        shutil.rmtree(dir)
