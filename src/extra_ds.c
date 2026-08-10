// vim: set syntax=cpp :
// Any globals that have yet to be placed in the right module.

#include <windows.h>

#include "kyetypes.h"

// There's probably one more source file here.
int g_door_anim_flag = 1; // DS:0D6A

HWND g_editor_tools; // DS:2E90
char g_2D90[256]; // DS:2D90
char g_toolname[50]; // DS:2D5E
HWND g_mainwnd; // DS:2D5C
HANDLE g_hInstance; // DS:2D5A
HDC g_maindc; // DS:2D58
int g_has_maindc; // DS:2D56
PAINTSTRUCT g_paint; // DS:2D36
HCURSOR g_hcArrow; // DS:2D34
HCURSOR g_hcCross; // DS:2D32
WORD g_maintimer; // DS:2D30
char g_statusmsg[256]; // DS:2C30 TODO CONFIRM SIZE
char g_dialog_message[80]; // DS:2BE0
char g_dialog_text[80]; // DS:2B90
RECT g_r_mainfrm; // DS:2B88
RECT g_r_playfield; // DS:2B80
RECT g_r_status_right; // DS:2B78
RECT g_r_status_left; // DS:2B70
RECT g_r_maincli; // DS:2B68

HBRUSH unused_2B66; // DS:2B66
HBRUSH g_hbrBlack; // DS:2B64
HBRUSH g_hbrWhite; // DS:2B62
HBRUSH g_hbrRed;   // DS:2B60
HBRUSH g_hbrGreen; // DS:2B5E
HBRUSH g_hbrBlue;  // DS:2B5C
HPEN g_saved_hpen; // DS:2B5A
HPEN g_hpenBlack; // DS:2B58
HPEN g_hpenWhite; // DS:2B56
HPEN g_hpenRed;  // DS:2B54
HBITMAP g_hbmpKye; // DS:2B52
HBITMAP g_hbmpBlock; // DS:2B50
HBITMAP g_hbmpWall; // DS:2B4E
char g_2AFE[80]; // DS:2AFE
char g_2AAE[80]; // DS:2AAE
char g_level_name[20]; // DS:2A9A
char g_level_congrats[80]; // DS:2A4A
char g_level_hint[80]; // DS:29FA
int g_kye_main_cx; // DS:29F8
int g_kye_main_cy; // DS:29F6
int g_kye_spawn_cx; // DS:29F4
int g_kye_spawn_cy; // DS:29F2
int g_kye_mouse_cx; // DS:29F0
int g_kye_mouse_cy; // DS:29EE
actor_s g_actors[MAX_ACTORS]; // DS:172E
int g_level_tiles[LEVEL_LX][LEVEL_LY]; // DS:127E
int unused_127C; // DS:127C
