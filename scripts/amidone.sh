#!/bin/sh
# amidone.sh: Do we have a 100% match?
set -e

#hexdump -Xv ref/20/KYE.EXE >build/matchref.txt
#hexdump -Xv out/KYE.EXE >build/matchnew.txt
#hexdump -Cv ref/20/KYE.EXE >build/matchref.txt
#hexdump -Cv out/KYE.EXE >build/matchnew.txt
ndisasm -b16 -e0x0600 -o0x0000 ref/20/KYE.EXE >build/matchref.txt
ndisasm -b16 -e0x0600 -o0x0000 out/KYE.EXE >build/matchnew.txt
vimdiff build/matchref.txt build/matchnew.txt
