#!/bin/sh

set -e

# Microsoft C PDS 6.0a, 5.25" 360K
for A in 1 2 3 4 5 6 7; do
  echo "$A"
  mkdir -p "osdisks/cc$A"
  mcopy -s -Do -n -i "packages/msc60a/Disk$A\ *.img" '::' "osdisks/cc$A/"
  echo
done

# Windows 3.0 SDK, 90mm 720K
for A in 29 30 31 32 33 34 35 36 37 38; do
  echo "$A"
  mkdir -p "osdisks/sdk$A"
  mcopy -s -Do -n -i "packages/winsdk30/*-0$A\).img" '::' "osdisks/sdk$A/"
  echo
done

