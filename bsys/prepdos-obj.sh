#!/bin/sh
set -e

for C_NAME in $@; do
  C_SRC="D:\\SRC\\${C_NAME}.C"
  C_OBJ="D:\\BUILD\\${C_NAME}.OBJ"
  case "${C_NAME}" in
    #unk004)
    #  # convert_char_to_internal() needs -Ov disabled in order to match (induction variable generation)
    #  echo "bcc -v -O2 -O-v -1 -W -ms -IC:\\BORLANDC\\INCLUDE -ID:\\SRC -c -o${C_OBJ} ${C_SRC}"
    #  ;;
    *)
      echo "bcc -v -O2 -1 -W -ms -IC:\\BORLANDC\\INCLUDE -ID:\\SRC -c -o${C_OBJ} ${C_SRC}"
      ;;
  esac

  echo "if errorlevel 1 goto fail"
done
