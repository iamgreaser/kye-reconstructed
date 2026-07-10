// vim: set syntax=cpp :
#include <windows.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kyetypes.h"
#include "kyeprocs.h"
#include "kyevars.h"

int g_has_maintimer; // DS:0458
// DS:045A contains strings.

// int // DS:06A9 == 0x5C
// DS:06AB contains strings.

// This map is at DS:06B8
// TODO: Editor entity spec --GM
// DS:0D38 = 0x000A
// DS:0D3A = 0x0000
// DS:0D3C contains at least one string...

// const int g_CIRCLE_X[4]; // DS:0D5A
// const int g_CIRCLE_Y[4]; // DS:0D62

// Word alignment mandates that this starts the module.
// CS:186C - ***CODE MATCH!*** (hopefully - the alignment throws ndisasm off)
void acquire_main_dc(void) {
  if (!g_has_maindc) {
    g_maindc = GetDC(g_mainwnd);
    g_has_maindc = 1;
  }
}

// CS:1886 - ***CODE MATCH!***
void release_main_dc(void) {
  if (g_has_maindc) {
    ReleaseDC(g_mainwnd, g_maindc);
    g_has_maindc = 0;
  }
}

// CS:18A1 - ***CODE MATCH!***
// UNUSED FUNCTION
void cs_18A1(RECT FAR *lprc) {
  HRGN rgn; // SI

  // Apparently this is not marked as const in 3.0, but is in 3.1
  rgn = CreateRectRgnIndirect(lprc);
  if (rgn) {
    SelectClipRgn(g_maindc, rgn);
    DeleteObject(rgn);
  }
}

// CS:18C9 - ***CODE MATCH!***
void init_timer(void) {
  g_maintimer = SetTimer(g_mainwnd, 0, 100, NULL);
  g_has_maintimer = 1;
}

// CS:18E4 - ***CODE MATCH!***
void deinit_timer(void) {
  // BUG: Checks against handle, but only clears the "has handle" flag.
  if (g_maintimer) {
    KillTimer(g_mainwnd, 0);
  }
  g_has_maintimer = 0;
}
