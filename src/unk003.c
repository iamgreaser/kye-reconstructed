// vim: set syntax=cpp :
#include <windows.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kyetypes.h"
#include "kyeprocs.h"
#include "kyevars.h"

// NOTE: Not sure if these should go here.
HWND g_mainwnd; // DS:2D5C
HANDLE g_hInstance; // DS:2D5A
HDC g_maindc; // DS:2D58
int g_has_maindc; // DS:2D56

// NOTE: These should probably go in main.c or earlier but it causes a mismatch.
PAINTSTRUCT g_paint; // DS:2D36
HCURSOR g_hcArrow; // DS:2D34
HCURSOR g_hcCross; // DS:2D32

// This is used here.
WORD g_maintimer; // DS:2D30

// NOTE: These should probably go in main.c or earlier but it causes a mismatch.
char g_statusmsg[256]; // DS:2C30
char g_dialog_message[80]; // DS:2BE0
char g_dialog_text[80]; // DS:2B90
RECT g_r_mainfrm; // DS:2B88
RECT g_r_playfield; // DS:2B80
RECT g_r_status_right; // DS:2B78
RECT g_r_status_left; // DS:2B70
RECT g_r_maincli; // DS:2B68
int unused_2B66; // DS:2B66

int g_has_maintimer = 0; // DS:0458
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
void unused_18A1(RECT FAR *lprc) {
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
