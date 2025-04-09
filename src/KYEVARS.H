// DS:0000 16 bytes unused
// DS:0010 seems to be standard runtime
// DS:0029 "Borland C++ - Copyright 1991 Borland Intl."

extern int unused_0054[4]; // DS:0054 8 bytes 0xFF, seemingly unused?
extern int g_mouse_captured; // DS:005C
extern int g_key_pressed; // DS:005E

// DS:0060: Constant strings in code

extern int g_level_complete; // DS:019E
extern char g_levelset_fname[256]; // DS:01A0
extern int g_levelidx; // DS:02A0
extern int g_levelcount; // DS:02A2
extern int g_tile_under_kye; // DS:02A4
extern int g_tile_lx; // DS:02A6
extern int g_tile_ly; // DS:02A8
extern int g_02AA; // DS:02AA -- TODO! --GM
extern int g_kye_target_cx; // DS:02AC
extern int g_kye_target_cy; // DS:02AE
extern int g_kye_needs_update; // DS:02B0
extern int g_moving_by_mouse; // DS:02B2
extern int g_kye_fizzle_in; // DS:02B4
extern int g_kye_injured; // DS:02B6
extern int g_kye_lives; // DS:02B8
extern int g_actor_count; // DS:02BA
extern int g_02BC; // DS:02BC
extern int g_tick_counter; // DS:02BE

// This map is at DS:02C0
extern tileconv_map_entry_s TILECONV_MAP[];

extern int g_is_in_editor; // DS:03F2
// DS:03F4 contains strings.

extern int g_has_maintimer; // DS:0458
// DS:045A contains strings.

// DS:050C pertains to some kind of file open dialogue?

// int // DS:06A9 == 0x5C
// DS:06AB contains strings.

// This map is at DS:06B8
// TODO: Editor entity spec --GM
// DS:0D38 = 0x000A
// DS:0D3A = 0x0000
// DS:0D3C contains at least one string...

// const int g_CIRCLE_X[4]; // DS:0D5A
// const int g_CIRCLE_Y[4]; // DS:0D62
extern int g_door_anim_flag; // DS:0D6A

extern int g_level_tiles[LEVEL_LX][LEVEL_LY]; // DS:127E
extern actor_s g_actors[MAX_ACTORS]; // DS:172E
extern int g_kye_mouse_cy; // DS:29EE
extern int g_kye_mouse_cx; // DS:29F0
extern int g_kye_spawn_cy; // DS:29F2
extern int g_kye_spawn_cx; // DS:29F4
extern int g_kye_main_cy; // DS:29F6
extern int g_kye_main_cx; // DS:29F8
extern char g_level_hint[80]; // DS:29FA
extern char g_level_congrats[80]; // DS:2A4A
extern char g_level_name[20]; // DS:2A9A
extern char g_2AAE[80]; // DS:2AAE
extern char g_2AFE[80]; // DS:2AFE
extern HBITMAP g_hbmpWall; // DS:2B4E
extern HBITMAP g_hbmpBlock; // DS:2B50
extern HBITMAP g_hbmpKye; // DS:2B52
extern HPEN g_hpenRed;   // DS:2B54
extern HPEN g_hpenWhite; // DS:2B56
extern HPEN g_hpenBlack; // DS:2B58
extern HPEN g_saved_hpen; // DS:2B5A
extern HBRUSH g_hbrBlue;  // DS:2B5C
extern HBRUSH g_hbrGreen; // DS:2B5E
extern HBRUSH g_hbrRed;   // DS:2B60
extern HBRUSH g_hbrWhite; // DS:2B62
extern HBRUSH g_hbrBlack; // DS:2B64

extern RECT g_r_maincli; // DS:2B68
extern RECT g_r_status_left; // DS:2B70
extern RECT g_r_status_right; // DS:2B78
extern RECT g_r_playfield; // DS:2B80
extern RECT g_r_mainfrm; // DS:2B88
extern char g_dialog_text[80]; // DS:2B90
extern char g_dialog_message[80]; // DS:2BE0
extern char g_statusmsg[256]; // DS:2C30 TODO CONFIRM SIZE
extern WORD g_maintimer; // DS:2D30
extern HCURSOR g_hcCross; // DS:2D32
extern HCURSOR g_hcArrow; // DS:2D34
extern PAINTSTRUCT g_paint; // DS:2D36
extern int g_has_maindc; // DS:2D56
extern HDC g_maindc; // DS:2D58
extern HANDLE g_hInstance; // DS:2D5A
extern HWND g_mainwnd; // DS:2D5C
