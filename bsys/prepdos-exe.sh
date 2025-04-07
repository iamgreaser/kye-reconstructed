#!/bin/sh
set -e

C_OBJ_LIST=""
for C_NAME in $@; do
  C_OBJ_LIST="${C_OBJ_LIST} D:\\BUILD\\${C_NAME}.OBJ"
done
echo "bcc -O2 -a -1 -W -ms -M -LC:\\BORLANDC\\LIB -eD:\\OUT\\KYE.EXE ${C_OBJ_LIST}"
echo "rc D:\\SRC\\KYE.RES D:\\OUT\\KYE.EXE"
