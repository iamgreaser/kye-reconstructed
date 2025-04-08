#!/bin/sh
set -e

C_OBJ_LIST=""
for C_NAME in $@; do
  C_OBJ_LIST="${C_OBJ_LIST} D:\\BUILD\\${C_NAME}.OBJ"
done
  echo "-O2 -y -1 -G -W -ms -M -LC:\\BORLANDC\\LIB -eD:\\OUT\\KYE.EXE ${C_OBJ_LIST}" >build/MKEXE.ARG
echo "bcc @D:\\BUILD\\MKEXE.ARG"
echo "rc D:\\SRC\\KYE.RES D:\\OUT\\KYE.EXE"
