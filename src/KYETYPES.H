#define LEVEL_LX 30
#define LEVEL_LY 20

#define TILE_LX 16
#define TILE_LY 16

#define MAX_ACTORS (LEVEL_LX*LEVEL_LY)

typedef struct {
  char name[35];
  char congrats[85];
  char hint[85];
  char rows[LEVEL_LY][LEVEL_LX+5];
  // we tend to get 1 byte of padding...?
} file_level_s;

typedef enum {
  T_DOOR_S_N = 0xFFEF,
  T_DOOR_N_S = 0xFFF0,
  T_DOOR_E_W = 0xFFF1,
  T_DOOR_W_E = 0xFFF2,
  T_DIAMOND = 0xFFF3,
  T_EARTH = 0xFFF4,
  T_WALL9 = 0xFFF5,
  T_WALL8 = 0xFFF6,
  T_WALL7 = 0xFFF7,
  T_WALL6 = 0xFFF8,
  T_WALL5 = 0xFFF9,
  T_WALL4 = 0xFFFA,
  T_WALL3 = 0xFFFB,
  T_WALL2 = 0xFFFC,
  T_WALL1 = 0xFFFD,
  T_KYE = 0xFFFE,
  T_EMPTY = 0xFFFF,
  //
  T_SBLOCK = 0x0000,
  T_SLIDER_N = 0x0001,
  T_SLIDER_S = 0x0002,
  T_SLIDER_W = 0x0003,
  T_SLIDER_E = 0x0004,
  T_STICKY_V = 0x0005,
  T_STICKY_H = 0x0006,
  T_BOUNCER_N = 0x0007,
  T_BOUNCER_S = 0x0008,
  T_BOUNCER_W = 0x0009,
  T_BOUNCER_E = 0x000A,
  T_ROCKY_N = 0x000B,
  T_ROCKY_S = 0x000C,
  T_ROCKY_W = 0x000D,
  T_ROCKY_E = 0x000E,
  T_B_TWISTER = 0x000F,
  T_B_GNASHER = 0x0010,
  T_B_BLOB = 0x0011,
  T_B_VIRUS = 0x0012,
  T_B_SPIKE = 0x0013,
  T_ROT_A = 0x0014,
  T_ROT_C = 0x0015,
  T_RBLOCK = 0x0016,
  T_AUTOSLIDER1 = 0x0017,
  T_AUTOSLIDER2 = 0x0018,
  T_AUTOSLIDER3 = 0x0019,
  T_AUTOSLIDER4 = 0x001A,
  T_AUTOROCKY1 = 0x001B,
  T_AUTOROCKY2 = 0x001C,
  T_AUTOROCKY3 = 0x001D,
  T_AUTOROCKY4 = 0x001E,
  T_BLACKY1 = 0x001F,
  T_BLACKY2 = 0x0020,
  //
  T_TIMER_0 = 0x0032,
  T_TIMER_1 = 0x0033,
  T_TIMER_2 = 0x0034,
  T_TIMER_3 = 0x0035,
  T_TIMER_4 = 0x0036,
  T_TIMER_5 = 0x0037,
  T_TIMER_6 = 0x0038,
  T_TIMER_7 = 0x0039,
  T_TIMER_8 = 0x003A,
  T_TIMER_9 = 0x003B,

  T_DEAD_ACTOR = 0x00FF,
} tiletype_e;

typedef struct {
  int subgroup;
  int internal_type;
  char external_char;
} tileconv_map_entry_s;

typedef struct {
  int type;
  int cx, cy;
  int var0;
} actor_s;
