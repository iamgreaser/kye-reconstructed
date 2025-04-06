#!/bin/sh
set -e

for C_NAME in $@; do
  C_SRC="D:\\SRC\\${C_NAME}.C"
  C_OBJ="D:\\BUILD\\${C_NAME}.OBJ"
  echo "bcc -W -ms -IC:\\BORLANDC\\INCLUDE -c -o${C_OBJ} ${C_SRC}"
done
