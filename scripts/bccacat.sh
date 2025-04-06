#!/bin/sh
for A in $*; do
  dd if="$A" ibs=1 skip=4
done
