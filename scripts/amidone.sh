#!/bin/sh
# amidone.sh: Do we have a 100% match?
set -e

case $1 in
  hex)
    # Hex dump
    #hexdump -Xv ref/20/KYE.EXE >build/matchref.txt
    #hexdump -Xv out/KYE.EXE >build/matchnew.txt
    hexdump -Cv ref/20/KYE.EXE >build/matchref.txt
    hexdump -Cv out/KYE.EXE >build/matchnew.txt
    vimdiff build/matchref.txt build/matchnew.txt
    ;;
  reloc)
    # Relocation dump (assumes relocations are at 0x890B!)
    ./scripts/relocscan.py ref/20/KYE.EXE >build/matchref.txt
    ./scripts/relocscan.py out/KYE.EXE >build/matchnew.txt
    vimdiff build/matchref.txt build/matchnew.txt
    ;;
  asm)
    # Disassembly
    ndisasm -b16 -e0x0600 -o0x0000 ref/20/KYE.EXE >build/matchref.txt
    ndisasm -b16 -e0x0600 -o0x0000 out/KYE.EXE >build/matchnew.txt
    vimdiff build/matchref.txt build/matchnew.txt
    ;;
  *)
    echo "INVALID ARGUMENT - provide a single argument of one of: asm hex reloc"
    false
    ;;
esac
