// vim: set syntax=cpp :
#include <windows.h>

#include "kyetypes.h"
#include "kyevars.h"

int g_level_complete = 0; // DS:019E
char g_levelset_fname[256] = "default.kye"; // DS:01A0
int g_levelidx = 1; // DS:02A0
int g_levelcount = 1; // DS:02A2
int g_tile_under_kye = T_EMPTY; // DS:02A4
int g_tile_lx = TILE_LX; // DS:02A6
int g_tile_ly = TILE_LY; // DS:02A8
int g_02AA = 0; // DS:02AA -- TODO! --GM
int g_kye_target_cx = 0; // DS:02AC
int g_kye_target_cy = 0; // DS:02AE
int g_kye_needs_update = 0; // DS:02B0
int g_moving_by_mouse = 0; // DS:02B2
int g_kye_fizzle_in = 1; // DS:02B4
int g_kye_injured = 0; // DS:02B6
int g_kye_lives = 3; // DS:02B8
int g_actor_count = 0; // DS:02BA
int g_02BC = 0; // DS:02BC
int g_tick_counter = 0; // DS:02BE

// This map is at DS:02C0
tileconv_map_entry_s TILECONV_MAP[] = {
  {0, 0x0000, ' '},

  {3, 0x0000, 'K'},

  {1, T_WALL1, '1'},
  {1, T_WALL2, '2'},
  {1, T_WALL3, '3'},
  {1, T_WALL4, '4'},
  {1, T_WALL5, '5'},
  {1, T_WALL6, '6'},
  {1, T_WALL7, '7'},
  {1, T_WALL8, '8'},
  {1, T_WALL9, '9'},
  {1, T_EARTH, 'e'},
  {1, T_DIAMOND, '*'},
  {1, T_DOOR_W_E, 'f'},
  {1, T_DOOR_E_W, 'g'},
  {1, T_DOOR_N_S, 'h'},
  {1, T_DOOR_S_N, 'i'},

  {2, T_SBLOCK, 'b'},
  {2, T_SLIDER_N, 'u'},
  {2, T_SLIDER_S, 'd'},
  {2, T_SLIDER_W, 'l'},
  {2, T_SLIDER_E, 'r'},
  {2, T_STICKY_V, 's'},
  {2, T_STICKY_H, 'S'},
  {2, T_BOUNCER_N, 'U'},
  {2, T_BOUNCER_S, 'D'},
  {2, T_BOUNCER_W, 'L'},
  {2, T_BOUNCER_E, 'R'},
  {2, T_ROCKY_N, '^'},
  {2, T_ROCKY_S, 'v'},
  {2, T_ROCKY_W, '<'},
  {2, T_ROCKY_E, '>'},
  {2, T_B_TWISTER, 'T'},
  {2, T_B_GNASHER, 'E'},
  {2, T_B_BLOB, 'C'},
  {2, T_B_VIRUS, '~'},
  {2, T_B_SPIKE, '['},
  {2, T_ROT_A, 'a'},
  {2, T_ROT_C, 'c'},
  {2, T_RBLOCK, 'B'},
  {2, T_AUTOSLIDER1, 'A'},
  {2, T_AUTOSLIDER2, 'A'},
  {2, T_AUTOSLIDER3, 'A'},
  {2, T_AUTOSLIDER4, 'A'},
  {2, T_AUTOROCKY1, 'F'},
  {2, T_AUTOROCKY2, 'F'},
  {2, T_AUTOROCKY3, 'F'},
  {2, T_AUTOROCKY4, 'F'},
  {2, T_BLACKY1, 'H'},
  {2, T_BLACKY2, 'H'},
  {2, T_TIMER_9, 'w'},
  {2, T_TIMER_8, 'x'},
  {2, T_TIMER_7, 'y'},
  {2, T_TIMER_6, 'z'},
  {2, T_TIMER_5, '{'},
  {2, T_TIMER_4, '|'},
  {2, T_TIMER_3, '}'},
  {2, T_TIMER_2, '}'},
  {2, T_TIMER_1, '}'},
  {2, T_TIMER_0, '}'},

  {-1, 0, 0},
};
