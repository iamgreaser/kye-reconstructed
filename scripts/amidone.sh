#!/bin/sh
# amidone.sh: Do we have a 100% match?
set -e

hexdump -Cv ref/20/KYE.EXE >build/matchref.txt
hexdump -Cv out/KYE.EXE >build/matchnew.txt
vimdiff build/matchref.txt build/matchnew.txt
