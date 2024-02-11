''' A program made because mkdir exits the makefile when the directory does not exist '''

if (__name__ != "__main__"): raise Exception("Not a library")

import sys, os, shutil

dirs = sys.argv.copy(); dirs.pop(0)

for dir in dirs:
    i = os.path.normpath(dir)

    if (os.path.exists(dir)):
        shutil.rmtree(dir)
