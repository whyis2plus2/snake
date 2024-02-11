''' A program made because mkdir exits the makefile when the directory already exists '''

import sys, os

if (len(sys.argv) < 2): print("Please input at least one argument"); sys.exit(-1)

dirs = sys.argv.copy(); dirs.pop(0)

for dir in dirs:
    i = os.path.normpath(dir)

    if (not os.path.exists(dir)):
        os.mkdir(dir)
