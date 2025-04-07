#!/bin/sh
# amidone.sh: Do we have a 100% match?
set -e

hexdump -Xv ref/20/KYE.EXE >build/matchref.txt
hexdump -Xv out/KYE.EXE >build/matchnew.txt
vimdiff build/matchref.txt build/matchnew.txt
