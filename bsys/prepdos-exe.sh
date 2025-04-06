#!/bin/sh
set -e

C_OBJ_LIST=""
for C_NAME in $@; do
  C_OBJ_LIST="${C_OBJS} D:\\BUILD\\${C_NAME}.OBJ"
done
echo "bcc -W -ms -LC:\\BORLANDC\\LIB -eD:\\OUT\\KYE.EXE ${C_OBJ_LIST}"
echo "rc D:\\SRC\\KYE.RES D:\\OUT\\KYE.EXE"
