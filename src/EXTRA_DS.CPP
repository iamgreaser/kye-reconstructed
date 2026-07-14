// vim: set syntax=cpp :
// Any globals that have yet to be placed in the right module.

#include <windows.h>

#include "kyetypes.h"

//char _pad0004[0x004]; // Put this *somewhere*...

// TODO: SIZE --GM
// TODO: Fill this in! --GM
tilespec_s g_06B8[1] = {
  {0},
};
char _pad0003[0x680-(26*1)]; // Put this *somewhere*...

int g_0D38; // DS:0D38
int g_0D3A; // DS:0D3A
char _pad0006[0x02E]; // Put this *somewhere*...
int g_door_anim_flag; // DS:0D6A

char _pad0001[0x512]; // Put this *somewhere*...

int g_level_tiles[LEVEL_LX][LEVEL_LY]; // DS:127E
actor_s g_actors[MAX_ACTORS]; // DS:172E
int g_kye_mouse_cy; // DS:29EE
int g_kye_mouse_cx; // DS:29F0
int g_kye_spawn_cy; // DS:29F2
int g_kye_spawn_cx; // DS:29F4
int g_kye_main_cy; // DS:29F6
int g_kye_main_cx; // DS:29F8
char g_level_hint[80]; // DS:29FA
char g_level_congrats[80]; // DS:2A4A
char g_level_name[20]; // DS:2A9A
char g_2AAE[80]; // DS:2AAE
char g_2AFE[80]; // DS:2AFE
HBITMAP g_hbmpWall; // DS:2B4E
HBITMAP g_hbmpBlock; // DS:2B50
HBITMAP g_hbmpKye; // DS:2B52
HPEN g_hpenRed;  // DS:2B54
HPEN g_hpenWhite; // DS:2B56
HPEN g_hpenBlack; // DS:2B58
HPEN g_saved_hpen; // DS:2B5A
HBRUSH g_hbrBlue;  // DS:2B5C
HBRUSH g_hbrGreen; // DS:2B5E
HBRUSH g_hbrRed;   // DS:2B60
HBRUSH g_hbrWhite; // DS:2B62
HBRUSH g_hbrBlack; // DS:2B64

char _pad0002[0x2];

RECT g_r_maincli; // DS:2B68
RECT g_r_status_left; // DS:2B70
RECT g_r_status_right; // DS:2B78
RECT g_r_playfield; // DS:2B80
RECT g_r_mainfrm; // DS:2B88
char g_dialog_text[80]; // DS:2B90
char g_dialog_message[80]; // DS:2BE0
char g_statusmsg[256]; // DS:2C30 TODO CONFIRM SIZE
WORD g_maintimer; // DS:2D30
HCURSOR g_hcCross; // DS:2D32
HCURSOR g_hcArrow; // DS:2D34
PAINTSTRUCT g_paint; // DS:2D36
int g_has_maindc; // DS:2D56
HDC g_maindc; // DS:2D58
HANDLE g_hInstance; // DS:2D5A
HWND g_mainwnd; // DS:2D5C
char g_toolname[50]; // DS:2D5E
char g_2D90[50]; // DS:2D90 TODO CONFIRM SIZE
char _pad0005[0x0CE]; // Put this *somewhere*...
HWND g_2E90; // DS:2E60
