#!/bin/sh
set -e

C_OBJ_LIST=""
for C_NAME in $@; do
  C_OBJ_LIST="${C_OBJ_LIST} D:\\BUILD\\${C_NAME}.OBJ"
done

if false; then
  # Link via BCC
  echo "-y -M -O2 -1 -W -ms -LC:\\BORLANDC\\LIB -eD:\\OUT\\KYE.EXE ${C_OBJ_LIST}" >build/MKEXE.ARG
  echo "bcc @D:\\BUILD\\MKEXE.ARG"
else
  # Link with TLINK directly
  LIB_LIST=""
  for LIB_NAME in CWS.LIB IMPORT.LIB; do
    LIB_LIST="${LIB_LIST} C:\\BORLANDC\\LIB\\${LIB_NAME}"
  done
  echo "/Twe C:\\BORLANDC\\LIB\\C0WS.OBJ ${C_OBJ_LIST},D:\\OUT\\KYE.EXE,D:\\OUT\\KYE.MAP,${LIB_LIST},D:\\SRC\\KYE.DEF" >build/MKEXE.ARG
  echo "tlink @D:\\BUILD\\MKEXE.ARG"
fi

echo "if errorlevel 1 goto fail"
echo "rc -?"
echo "cd D:\\SRC\\"
echo "rc -r -i C:\\BORLANDC\\INCLUDE -fo D:\\BUILD\\KYE.RES D:\\SRC\\KYE.RC"
echo "if errorlevel 1 goto fail"
echo "cd \\"
echo "rc D:\\BUILD\\KYE.RES D:\\OUT\\KYE.EXE"
echo "if errorlevel 1 goto fail"
