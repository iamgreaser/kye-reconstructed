#!/bin/sh
set -e

for C_NAME in $@; do
  C_SRC="D:\\SRC\\${C_NAME}.CPP"
  C_OBJ="D:\\BUILD\\${C_NAME}.OBJ"
  echo "bcc -W -ms -IC:\\BORLANDC\\INCLUDE -ID:\\SRC -c -o${C_OBJ} ${C_SRC}"
done
