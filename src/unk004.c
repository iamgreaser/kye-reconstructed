#include "kye.h"

// Word alignment mandates that this starts the module.
// CS:18FE - ***CODE MATCH!*** (if and only if -Ov is not set)
#pragma option -O-v
int convert_char_to_internal(int *subgroup_out, int *internal_type_out, char external_char_in) {
  // DI = subgroup_out
  int i; // CX

  for (i = 0; TILECONV_MAP[i].subgroup != -1; i++) {
    if (TILECONV_MAP[i].external_char == external_char_in) {
      *subgroup_out = TILECONV_MAP[i].subgroup;
      *internal_type_out = TILECONV_MAP[i].internal_type;
      return 1;
    }
  }

  return 0;
}

// CS:1942 - ***CODE MATCH!*** (if and only if -Ov is not set)
int convert_internal_to_char(int subgroup_in, int internal_type_in, char *external_char_out) {
  int i; // CX

  for (i = 0; TILECONV_MAP[i].subgroup != -1; i++) {
    if (TILECONV_MAP[i].subgroup == subgroup_in
        && TILECONV_MAP[i].internal_type == internal_type_in) {
      *external_char_out = TILECONV_MAP[i].external_char;
      return 1;
    }
  }

  return 0;
}
