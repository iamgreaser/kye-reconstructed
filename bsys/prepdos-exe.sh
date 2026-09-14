#!/bin/sh
set -e

C_OBJ_LIST=""
for C_NAME in $@; do
  C_OBJ_LIST="${C_OBJ_LIST} D:\\BUILD\\${C_NAME}.OBJ"
done

if false; then
  # Link via BCC
  #echo "-y -M -O2 -1 -W -ms -LC:\\BORLANDC\\LIB -eD:\\OUT\\KYE.EXE ${C_OBJ_LIST}" >build/MKEXE.ARG
  echo "-1 -W -ms -LC:\\BORLANDC\\LIB -eD:\\OUT\\KYE.EXE ${C_OBJ_LIST}" >build/MKEXE.ARG
  echo "bcc @D:\\BUILD\\MKEXE.ARG"
else
  # Link with TLINK directly
  # NOTE: Probably not used unless there's a way to make 6-byte entry table entries appear.
  LIB_LIST="import cws"
  echo "/Twe /m /s /LC:\\BORLANDC\\LIB c0ws ${C_OBJ_LIST},D:\\OUT\\Kye.,D:\\OUT\\KYE.MAP,${LIB_LIST},D:\\SRC\\KYE.DEF" >build/MKEXE.ARG
  echo "tlink"
  echo "tlink @D:\\BUILD\\MKEXE.ARG"
  echo "if errorlevel 1 goto fail"
  echo "copy D:\\OUT\\KYE D:\\OUT\\KYE.EXE"
  echo "if errorlevel 1 goto fail"
fi

echo "if errorlevel 1 goto fail"
echo "rc -?"
if true; then
  # Compile resource separately from binding to the EXE
  echo "cd D:\\SRC\\"
  echo "rc -r -i C:\\BORLANDC\\INCLUDE -fo D:\\BUILD\\KYE.RES D:\\SRC\\KYE.RC"
  echo "if errorlevel 1 goto fail"
  echo "cd \\"
  echo "rc D:\\BUILD\\KYE.RES D:\\OUT\\KYE.EXE"
  echo "if errorlevel 1 goto fail"
else
  # Compile resource directly into EXE
  # NOTE: Probably not used, I suspect Borland's tools were used to generate a .RES file directly.
  echo "cd D:\\SRC\\"
  echo "rc -i C:\\BORLANDC\\INCLUDE D:\\SRC\\KYE.RC D:\\OUT\\KYE.EXE"
  echo "if errorlevel 1 goto fail"
fi
