// vim: set syntax=cpp :
#include <windows.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kyetypes.h"
#include "kyeprocs.h"
#include "kyevars.h"

// DS:050C pertains to some kind of file open dialogue?
char g_050C[128]; // DS:050C
char g_058C[128]; // DS:058C
char g_060C[128]; // DS:060C TODO CONFIRM SIZE

char g_068C[13] = "*.kye"; // DS:068C
// DS:0699 contains constant NUL-terminated string ".kye"
char g_069E[13] = "~~~~~~~~.*\\"; // DS:069E

// Weird compiler flags from the previous module seems to mandate that this starts the module.
// CS:1986 - ***CODE MATCH!***
void clear_level(void) {
  // NOTE: Levels are stored transposed.
  // TODO: Check optimisation! This put a lot of stuff into registers. --GM
  int x;
  int y;

  g_actor_count = 0;
  strcpy(g_level_name, "EMPTY"); // DS:045A
  strcpy(g_level_congrats, "message"); // DS:0460
  strcpy(g_level_hint, "No level data"); // DS:0468

  for (y = 0; y < LEVEL_LY; y++) {
    for (x = 0; x < LEVEL_LX; x++) {
      g_level_tiles[x][y] = T_WALL5;
    }
  }
  for (y = 1; y < LEVEL_LY-1; y++) {
    for (x = 1; x < LEVEL_LX-1; x++) {
      g_level_tiles[x][y] = T_EMPTY;
    }
  }
  g_level_tiles[2][1] = T_WALL5;
  g_level_tiles[1][2] = T_WALL5;
  g_level_tiles[2][2] = T_WALL3;
  g_kye_main_cx = 3;
  g_kye_main_cy = 3;
  g_kye_spawn_cx = 3;
  g_kye_spawn_cy = 3;
  g_level_tiles[3][3] = T_KYE;
  g_level_tiles[1][1] = T_DIAMOND;
  g_tile_under_kye = T_EMPTY;
}

// CS:1A37 - ***CODE MATCH!***
void process_loaded_level(void) {
  // SI = tile pointer
  int cx; // DI
  int cy; // DX
  int found; // CX

  // Ensure at least one diamond exists.
  found = 0;
  for (cx = 0; cx < LEVEL_LX; cx++) {
    for (cy = 0; cy < LEVEL_LY; cy++) {
      if (g_level_tiles[cx][cy] == T_DIAMOND) {
        found++;
        goto done_diamond_search;
      }
    }
  }
done_diamond_search:
  if (!found) {
    // Nothing exists? Place a diamond.
    g_level_tiles[1][1] = T_DIAMOND;
  }

  // Ensure exactly one Kye exists.
  found = 0;
  for (cx = 0; cx < LEVEL_LX; cx++) {
    for (cy = 0; cy < LEVEL_LY; cy++) {
      if (g_level_tiles[cx][cy] == T_KYE) {
        found++;
        goto done_kye_search;
      }
    }
  }
done_kye_search:
  // Nothing exists, or two Kyes on the board? Place a Kye.
  // (load_level_row sets cx,cy)
  if ((!found) || cx != g_kye_main_cx || cy != g_kye_main_cy) {
    g_kye_main_cx = 3;
    g_kye_main_cy = 3;
    g_kye_spawn_cx = 3;
    g_kye_spawn_cy = 3;
    g_level_tiles[3][3] = T_KYE;
  }

  // Ensure borders have walls
  for (cx = 0; cx < LEVEL_LX; cx++) {
    if (!(g_level_tiles[cx][0] >= T_WALL9 && g_level_tiles[cx][0] <= T_WALL1)) {
      if (g_level_tiles[cx][0] >= 0) {
        mark_actor_for_deletion(g_level_tiles[cx][0]);
      }
      g_level_tiles[cx][0] = T_WALL5;
    }
  }
  for (cx = 0; cx < LEVEL_LX; cx++) {
    if (!(g_level_tiles[cx][LEVEL_LY-1] >= T_WALL9 && g_level_tiles[cx][LEVEL_LY-1] <= T_WALL1)) {
      if (g_level_tiles[cx][LEVEL_LY-1] >= 0) {
        mark_actor_for_deletion(g_level_tiles[cx][LEVEL_LY-1]);
      }
      g_level_tiles[cx][LEVEL_LY-1] = T_WALL5;
    }
  }
  for (cy = 0; cy < LEVEL_LY; cy++) {
    if (!(g_level_tiles[0][cy] >= T_WALL9 && g_level_tiles[0][cy] <= T_WALL1)) {
      if (g_level_tiles[0][cy] >= 0) {
        mark_actor_for_deletion(g_level_tiles[0][cy]);
      }
      g_level_tiles[0][cy] = T_WALL5;
    }
  }
  for (cy = 0; cy < LEVEL_LY; cy++) {
    if (!(g_level_tiles[LEVEL_LX-1][cy] >= T_WALL9 && g_level_tiles[LEVEL_LX-1][cy] <= T_WALL1)) {
      if (g_level_tiles[LEVEL_LX-1][cy] >= 0) {
        mark_actor_for_deletion(g_level_tiles[LEVEL_LX-1][cy]);
      }
      g_level_tiles[LEVEL_LX-1][cy] = T_WALL5;
    }
  }

  // Finally, clean up our actors list!
  garbage_collect_actors();
}

// CS:1B6C - ***CODE MATCH!***
void read_line(FILE *fp, char *buf, int max_len) {
  char linebuf[256];

  int i;

  buf[0] = 0;
  fgets(linebuf, 255, fp);

  for (i = 0;
       linebuf[i] != 0
         && i != max_len
         && linebuf[i] != '\n'
         && linebuf[i] != '\r';
       i++) {
    buf[i] = linebuf[i];
  }
  buf[i] = 0;
}

// CS:1BBE - ***CODE MATCH!***
void read_level(FILE *fp, file_level_s *lvl) {
  int cy; // register SI

  read_line(fp, lvl->name, 35-2);
  read_line(fp, lvl->hint, 85-2);
  read_line(fp, lvl->congrats, 85-2);
  for (cy = 0; cy < LEVEL_LY; cy++) {
    read_line(fp, lvl->rows[cy], LEVEL_LX+5-2);
  }
}

// CS:1C13 - ***CODE MATCH!***
int write_line(FILE *fp, const char *s) {
  int result;
  result = fputs(s, fp);
  if (result == -1) { return 0; }
  result = fputc('\n', fp);
  if (result == -1) { return 0; }
  return 1;
}

// CS:1C45 - ***CODE MATCH!***
int write_level(FILE *fp, file_level_s *lvl) {
  int cy; // SI
  int result; // DX

  result = write_line(fp, lvl->name);
  if (result == 0) { return 0; }
  result = write_line(fp, lvl->hint);
  if (result == 0) { return 0; }
  result = write_line(fp, lvl->congrats);
  if (result == 0) { return 0; }
  for (cy = 0; cy < LEVEL_LY; cy++) {
    result = write_line(fp, lvl->rows[cy]);
    if (result == 0) { return 0; }
  }
  return 1;
}

// CS:1CCB - ***CODE MATCH!***
// UNUSED FUNCTION
void cs_1CCB(void) {
  g_actor_count = 0;
}

// CS:1CD2 - MISMATCH FIXME: Registers are loaded in the wrong order and I need a hack to force the type into a register so the rest of the code can mostly match --GM
int create_actor(int type, int cx, int cy) {
  register int atype = type; // CX
  if (g_actor_count < MAX_ACTORS) {
    // CX = type
    // SI = cx
    // DI = cy
    g_level_tiles[cx][cy] = g_actor_count;
    g_actors[g_actor_count].type = atype;
    g_actors[g_actor_count].cx = cx;
    g_actors[g_actor_count].cy = cy;
    g_actors[g_actor_count].var0 = 0;
    g_actor_count = g_actor_count + 1;
    return g_actor_count - 1;
  }
  return -1;
}

// CS:1D40 - ***CODE MATCH!***
void mark_actor_for_deletion(int ai) {
  g_actors[ai].type = T_DEAD_ACTOR;
}

// CS:1D51 - ***CODE MATCH!***
void garbage_collect_actors(void) {
#if 1
  // This is the real implementation.
  // stack: 0x04 bytes (BP/FC)
  // BP/FC - temporary actor pointer
  // BP/FE - temporary column pointer

  // These are probably called i, j, k.
  int ai; // CX
  int cx; // DX - also secondary actor index pointer
  int cy; // DI

  ai = 0;
  while (ai < g_actor_count) {
    if (g_actors[ai].type == T_DEAD_ACTOR) {
      // BUG: Actor count apparently doesn't decrement when there's only one actor?
      // I'm not sure how exploitable this is - I don't know how to create actors when there's no actors remaining. --GM
      if (g_actor_count >= 1) {
        g_actor_count = g_actor_count - 1;
        for (cx = ai; cx < g_actor_count; cx++) {
          // BUG: var0 is not copied. This could be abused to, for example, make a blacky last longer.
          g_actors[cx].type = g_actors[cx+1].type;
          g_actors[cx].cx = g_actors[cx+1].cx;
          g_actors[cx].cy = g_actors[cx+1].cy;
        }
        for (cy = 1; cy < LEVEL_LY-1; cy++) {
          for (cx = 1; cx < LEVEL_LX-1; cx++) {
            if (g_level_tiles[cx][cy] > ai) {
              g_level_tiles[cx][cy] = g_level_tiles[cx][cy] - 1;
            }
          }
        }
      }
    } else {
      ai++;
    }
  }

#else
  // This is how I implemented it before attempting to match the code. --GM
  // It's O(n) instead of O(n^2) and it's easier to understand.
  // It does not have the bugs identified. As a result, due to one of the bugs being triggered frequently in practice, this is inaccurate.

  int ai;
  int deleted;

  ai = 0;
  deleted = 0;
  for (ai = 0; ai < g_actor_count; ai++) {
    if (g_actors[ai].type == T_DEAD_ACTOR) {
      deleted += 1;
    } else if (deleted != 0) {
      g_level_tiles[g_actors[ai].cx][g_actors[ai].cy] -= deleted;
      memcpy(&g_actors[ai-deleted], &g_actors[ai], sizeof(g_actors[0]));
    }
  }
  g_actor_count -= deleted;
#endif
}

// CS:1DF3 - ***CODE MATCH!***
void load_level_row(int cy, const char *rowstr) {
  // TODO: Clean up some optimisations? --GM
  // needs 0x8 extra bytes on stack
  int ai; // BP/FE BP-0x02
  int subgroup; // BP/FC BP-0x04
  int internal_type; // BP/FA BP-0x06
  const char *str_ptr; // BP/F8 BP-0x08

  // registers
  int cx; // SI
  int *tileptr; // DI

  if (cy >= LEVEL_LY) return;

  // Clear row
  for (cx = 0; cx < LEVEL_LX; cx++) {
    g_level_tiles[cx][cy] = T_EMPTY;
  }

  for (cx = 0, str_ptr = rowstr, tileptr = &g_level_tiles[0][cy];
       (*str_ptr) != 0;
       str_ptr++, tileptr += LEVEL_LY, cx++) {

    if (cx >= LEVEL_LX) return;
    if (convert_char_to_internal(&subgroup, &internal_type, *str_ptr)) {
      switch (subgroup) {
        case 0:
          *tileptr = T_EMPTY;
          break;
        case 1:
          *tileptr = internal_type;
          break;
        case 2:
          switch(internal_type) {
          case T_B_TWISTER:
          case T_B_GNASHER:
          case T_B_BLOB:
          case T_B_VIRUS:
          case T_B_SPIKE:
            ai = create_actor(internal_type, cx, cy);
            // BUG: Failure (-1) not checked
            // Mathematically it won't happen, but I like hygiene. --GM

            // random(x) is a Turbo C++ / Borland C++ extension, apparently?
            // It's defined as a macro.
            // Probably something like ((((long)rand()) * (x)) / RAND_MAX)?
            g_actors[ai].var0 = random(4);
            break;
          case T_AUTOSLIDER1:
            create_actor((cx % 4) + T_AUTOSLIDER1, cx, cy);
            break;
          case T_AUTOROCKY1:
            create_actor((cx % 4) + T_AUTOROCKY1, cx, cy);
            break;
          default:
            create_actor(internal_type, cx, cy);
            break;
          }
          break;
        case 3:
          *tileptr = T_KYE;
          g_kye_main_cx = cx;
          g_kye_main_cy = cy;
          g_kye_spawn_cy = cy;
          g_kye_spawn_cx = cx;
          break;
      }
    }
  }
}

// CS:1F43 - ***CODE MATCH!***
void encode_level_row(int cy, char *line) {
  // Stack: 0x06 bytes
  char external_char; // BP/FE
  int cx; // BP/FC
  int internal_type; // BP/FA

  int tile; // DX
  int subgroup; // CX
  // DI = cy<<1
  // SI = running tile counter

  for (cx = 0; cx < LEVEL_LX; cx++) {
    internal_type = 0;
    tile = g_level_tiles[cx][cy];
    if (tile >= 0) {
      subgroup = 2;
      internal_type = g_actors[tile].type;
    } else if (tile == T_EMPTY) {
      subgroup = 0;
    } else if (tile == T_KYE) {
      subgroup = 3;
    } else {
      subgroup = 1;
      internal_type = tile;
    }
    if (convert_internal_to_char(subgroup, internal_type, &external_char)) {
      line[cx] = external_char;
    } else {
      line[cx] = ' ';
    }
  }
  line[LEVEL_LX] = 0;
}

// CS:1FC5 - ***CODE MATCH!***
int find_level_by_name(const char *name) {
  // stack: 0x03DE bytes
  int levelidx; // BP/FFFE
  FILE *fp; // BP/FFFC
  file_level_s lvl; // BP/FC72
  char buf80a[80]; // BP/FC22

  if (g_levelset_fname[0] == 0) {
    return -1;
  }
  fp = fopen(g_levelset_fname, "r"); // DS:0476
  if (fp == NULL) {
    return -1;
  }
  fseek(fp, 0, SEEK_SET);
  read_line(fp, buf80a, 80-1);
  g_levelcount = atol(buf80a);
  for (levelidx = 1; levelidx <= g_levelcount; levelidx++) {
    read_level(fp, &lvl);
    if (!strcmp(name, lvl.name)) {
      fclose(fp);
      return levelidx;
    }
  }

  fclose(fp);
  return -1;
}

// CS:2088 - ***CODE MATCH!***
int registration_check(const char *cmdline) {
  // If you want to use this, give 10 GBP to a childrens charity.
  // It skips border.kye and the "please consider donating" screen.
  if (cmdline[0] != 'd') return 0;
  if (cmdline[1] != 'i') return 0;
  if (cmdline[2] != 'g') return 0;
  if (cmdline[3] != 'g') return 0;
  if (cmdline[4] != 'e') return 0;
  if (cmdline[5] != 'r') return 0;
  return 1;
}

// CS:20D6 - ***CODE MATCH!***
void load_level(int levelidx) {
  // needs 0x3DC (988) extra bytes on stack
  FILE *fp; // BP/FFFE BP-0x002
  file_level_s lvl; // BP/FC74
  char buf80a[80]; // BP/FC24

  int i;

  if (g_levelset_fname[0]) {
    fp = fopen(g_levelset_fname, "r"); // DS:0478
    if (!fp) {
      msgbox_error("Cannot open file: ", g_levelset_fname); // DS:047A
      clear_level();
      g_levelset_fname[0] = 0;
      return;
    }
    fseek(fp, 0, SEEK_SET);
    read_line(fp, buf80a, 80-1);
    g_levelcount = atol(buf80a);
    g_level_complete = 0;
    g_kye_needs_update = 0;
    g_kye_fizzle_in = 1;
    g_kye_injured = 0;
    g_kye_lives = 3;
    g_tile_under_kye = T_EMPTY;
    if ((levelidx < 1) || (levelidx > g_levelcount)) {
      levelidx = 1;
    }

    for (i = 0; i < levelidx; i++) {
      read_level(fp, &lvl);
    }
    fclose(fp);
    g_actor_count = 0;
    strcpy(g_level_name, lvl.name);
    strcpy(g_level_congrats, lvl.congrats);
    strcpy(g_level_hint, lvl.hint);
    for (i = 0; i < LEVEL_LY; i++) {
      load_level_row(i, lvl.rows[i]);
    }
    process_loaded_level();
  }
}

// CS:2241 - ***CODE MATCH!***
void save_single_level_file(void) {
  // stack: 0x3DC bytes
  FILE *fp; // BP/FFFE
  file_level_s lvl; // BP/FC74
  char buf80a[80]; // BP/FC24

  int result; // DX
  int cy; // SI
  // DI = row pointer

  strcpy(lvl.name, g_level_name);
  strcpy(lvl.congrats, g_level_congrats);
  strcpy(lvl.hint, g_level_hint);

  for (cy = 0; cy < LEVEL_LY; cy++) {
    encode_level_row(cy, lvl.rows[cy]);
  }

  fp = fopen(g_levelset_fname, "w"); // DS:048D
  if (fp == 0) goto open_fail;
  // ^ The above has to be a goto. Making this gotoless gives the following mismatch:
  //   mov [bp-0x2],ax
  //   cmp word [bp-0x2],byte +0x0
  //   jz 0x2340
  //   push byte +0x0
  //   push byte +0x0
  //   push byte +0x0
  // and then instead of this:
  //   000022F3  FF76FE            push word [bp-0x2]
  // it'll do this:
  //   push ax

  fseek(fp, 0, SEEK_SET);
  strcpy(buf80a, "1"); // DS:048F
  result = write_line(fp, buf80a);
  if (!result) goto write_fail;
  result = write_level(fp, &lvl);
  if (!result) goto write_fail;
  fclose(fp);
  return;

open_fail:
  msgbox_error("Cannot open file: ", g_levelset_fname); // DS:0491
  clear_level();
  g_levelset_fname[0] = 0;
  return;

  // BUG: fp is not closed in this situation!
write_fail:
  msgbox_error("Cannot write file: ", g_levelset_fname); // DS:04A4
  clear_level();
  g_levelset_fname[0] = 0;
  return;
}

// CS:2370 - ***CODE MATCH!***
void delete_and_draw_unacting_tile(int cx, int cy) {
  // SI = cx
  // DI = cy
  int px; // BX
  int py; // CX

  g_level_tiles[cx][cy] = T_EMPTY;
  // BUG: Previous pen not saved
  SelectObject(g_maindc, g_hpenWhite);
  px = g_r_playfield.left + (cx * g_tile_lx);
  py = g_r_playfield.top  + (cy * g_tile_ly);
  Rectangle(
    g_maindc,
    px,
    py,
    px + g_tile_lx,
    py + g_tile_ly);
}

// CS:23D2 - ***CODE MATCH!***
void draw_actor(HDC dst_dc, HDC src_dc, int ai) {
  int dst_px; // BP/FE
  int dst_py; // BP/FC

  // SI = ai
  int type; // CX
  int src_py; // DX
  int src_px; // DI

  type = g_actors[ai].type;
  dst_px = g_actors[ai].cx * g_tile_lx;
  dst_py = g_actors[ai].cy * g_tile_ly;

  if (type >= 23) {
    src_px = (type - 23) * TILE_LX;
    src_py = 4 * TILE_LY;
  } else {
    src_px = type * TILE_LX;
    src_py = 0;
  }

  switch (type) {
  case T_B_TWISTER:
  case T_B_GNASHER:
  case T_B_BLOB:
  case T_B_VIRUS:
  case T_B_SPIKE:
    src_py = g_actors[ai].var0 * TILE_LY;
    break;
  default:
    if (type >= T_TIMER_0 && type <= T_TIMER_9) {
      src_px = (type - T_TIMER_0) * TILE_LX;
      src_py = 1 * TILE_LY;
    }
    break;
  }
  BitBlt(
    dst_dc,
    dst_px, dst_py,
    TILE_LX, TILE_LY,
    src_dc,
    src_px, src_py,
    SRCCOPY);
}

// CS:247F - ***CODE MATCH!***
void repaint_all_actors(void) {
  HDC dc; // DI
  int ai; // SI

  // BUG: DC not checked for null handle
  dc = CreateCompatibleDC(g_maindc);
  SelectObject(dc, g_hbmpBlock);
  for (ai = 0; ai < g_actor_count; ai++) {
    draw_actor(g_maindc, dc, ai);
  }
  DeleteDC(dc);
}

// CS:24BA - MISMATCH FIXME: 2 extra write-only bytes on the stack, use of the wrong register, and 2 bytes of extra code so we're out of phase. Using a kludge to at least get this back into phase. --GM
void draw_wall(int cx, int cy, HDC dc) {
  // stack: 0x04 bytes
  // BP/FE
  // int *column; // BP/FC

  // General arrangement:
  // SI = x
  // CX = y

  int px; // SI
  int py; // DI
  register int src_px; // DX - MISMATCH FIXME: This needs to be DX! Not AX! --GM

#define tile (g_level_tiles[cx][cy])

  // These types are negative numbers, T_WALL1 > T_WALL9
  if (tile <= T_WALL1 && tile >= T_WALL9) {
    // Draw actual wall
    px = cx * g_tile_lx;
    py = cy * g_tile_ly;
    //
    // MISMATCH: There's a dummy register spill here.
    //
    // Original: [bp-0x2] gets set to DX which is set after calculating all this crap.
    //
    // This code: [bp-0x2] gets set to AX which is set to tile which is g_level_tiles[cx][cy].
    // In doing so, the second abs() calculation adds in an extra MOV DX,AX.
    //
    // This particular kludging constrains the mismatch to this if() block.
    //
    src_px = abs(tile);
    src_px = TILE_LX * (src_px - abs(T_WALL1) + 3);
    BitBlt(g_maindc, px, py, TILE_LX, TILE_LY, dc, src_px, 0 * TILE_LY, SRCCOPY);
    return;
  }

  if (tile == T_EARTH) {
    px = cx * g_tile_lx;
    py = cy * g_tile_ly;
    BitBlt(g_maindc, px, py, TILE_LX, TILE_LY, dc, 0 * TILE_LX, 0 * TILE_LY, SRCCOPY);
    return;
  }

  if (tile == T_DIAMOND) {
    px = cx * g_tile_lx;
    py = cy * g_tile_ly;
    BitBlt(g_maindc, px, py, TILE_LX, TILE_LY, dc, 12 * TILE_LX, 0 * TILE_LY, SRCCOPY);
    return;
  }

  if (tile == T_DOOR_W_E || tile == T_DOOR_E_W) {
    px = cx * g_tile_lx;
    py = cy * g_tile_ly;
    BitBlt(g_maindc, px, py, TILE_LX, TILE_LY, dc, 14 * TILE_LX, 0 * TILE_LY, SRCCOPY);
    return;
  }

  if (tile == T_DOOR_N_S || tile == T_DOOR_S_N) {
    px = cx * g_tile_lx;
    py = cy * g_tile_ly;
    BitBlt(g_maindc, px, py, TILE_LX, TILE_LY, dc, 13 * TILE_LX, 0 * TILE_LY, SRCCOPY);
    return;
  }
#undef tile
}

// CS:2665 - ***CODE MATCH!***
void repaint_all_walls(void) {
  HDC dc; // BP/FE BP-0x02
  // BP-0x00

  int cx; // DI
  int cy; // SI

  // BUG: DC not checked for null handle
  dc = CreateCompatibleDC(g_maindc);
  SelectObject(dc, g_hbmpWall);
  for (cx = 0; cx < LEVEL_LX; cx++) {
    for (cy = 0; cy < LEVEL_LY; cy++) {
      draw_wall(cx, cy, dc);
    }
  }
  DeleteDC(dc);
}

// CS:26AA - ***CODE MATCH!***
void draw_fizzled_kye(int fizzle_level){
  // Stack: 0x08 bytes
  int px; // BP/FE
  int py; // BP/FC
  HDC src_dc; // BP/FA
  HBITMAP bmp; // BP/F8

  HDC bmp_dc; // DI
  int reps; // SI

  src_dc = CreateCompatibleDC(g_maindc);
  bmp = CreateCompatibleBitmap(g_maindc, TILE_LX, TILE_LY);
  bmp_dc = CreateCompatibleDC(g_maindc);
  SelectObject(bmp_dc, bmp);
  SelectObject(src_dc, g_hbmpKye);

  BitBlt(bmp_dc, 0, 0, TILE_LX, TILE_LY, src_dc,
    0 * TILE_LX, 0 * TILE_LY, SRCCOPY);

  for (reps = 0; reps < fizzle_level; reps++) {
    SetPixel(bmp_dc, random(TILE_LX), random(TILE_LY), RGB(0xFF, 0xFF, 0xFF));
  }
  px = g_kye_main_cx * g_tile_lx;
  py = g_kye_main_cy * g_tile_ly;
  BitBlt(g_maindc, px, py, TILE_LX, TILE_LY, bmp_dc, 0, 0, SRCCOPY);

  DeleteDC(src_dc);
  DeleteDC(bmp_dc);
  DeleteObject(bmp);
}

// CS:2791 - ***CODE MATCH!***
void paint_tile_in_editor(int cx, int cy) {
  // Stack: 0x02 bytes
  int tile; // BP/FE

  // DI = cx
  HDC dc; // SI

  dc = CreateCompatibleDC(g_maindc);

  tile = g_level_tiles[cx][cy];
  if (tile >= 0) {
    SelectObject(dc, g_hbmpBlock);
    draw_actor(g_maindc, dc, tile);

  } else if (tile == T_KYE) {
    SelectObject(dc, g_hbmpKye);
    BitBlt(g_maindc,
      cx * g_tile_lx, cy * g_tile_ly,
      TILE_LX, TILE_LY,
      dc,
      0 * TILE_LX, 0 * TILE_LY,
      SRCCOPY);

  } else if (tile == T_EMPTY) {
    delete_and_draw_unacting_tile(cx, cy);

  } else {
    SelectObject(dc, g_hbmpWall);
    draw_wall(cx, cy, dc);
  }

  DeleteDC(dc);
}

// CS:2847 - ***CODE MATCH!***
void repaint_status_left(void) {
  // stack needs 0x6C bytes (BP/94)
  HDC dc; // BP/FE
  int px; // BP/FC
  // int *ent1; // BP/FA
  // tile ptr // BP/F8
  char textbuf[100]; // BP/94

  int i; // SI
  int diamonds; // CX
  int x; // DI
  int y; // DX

  switch (g_is_in_editor) {
  case 0:
    SelectObject(g_maindc, g_hpenWhite);
    Rectangle(
      g_maindc,
      g_r_status_left.left,
      g_r_status_left.top,
      g_r_status_left.left + 70,
      g_r_status_left.top + 17);
    dc = CreateCompatibleDC(g_maindc);
    SelectObject(dc, g_hbmpKye);
    for (i = 0; i < g_kye_lives; i++) {
      x = g_r_status_left.left + i * (TILE_LX + 4) + 1;
      y = g_r_status_left.top + 1;
      BitBlt(
        g_maindc,
        x, y,
        TILE_LX, TILE_LY,
        dc,
        0, 0,
        SRCCOPY);
    }
    DeleteDC(dc);

    SelectObject(g_maindc, g_hpenBlack);
    sprintf(textbuf, "Level:  %d   ", g_levelidx); // DS:04B8
    y = strlen(textbuf);
    TextOut(g_maindc,
      g_r_status_left.left + 80,
      g_r_status_left.top,
      textbuf, y);

    diamonds = 0;
    for (x = 0; x < LEVEL_LX; x++) {
      for (y = 0; y < LEVEL_LY; y++) {
        if (g_level_tiles[x][y] == T_DIAMOND) {
          diamonds++;
        }
      }
    }
    sprintf(textbuf, "Diamonds left:  %d   ", diamonds); // DS:04C6
    y = strlen(textbuf);
    TextOut(g_maindc,
      g_r_status_left.left + 160,
      g_r_status_left.top,
      textbuf, y);
    break;

  case 1:
    diamonds = 0;
    for (x = 0; x < LEVEL_LX; x++) {
      for (y = 0; y < LEVEL_LY; y++) {
        if (g_level_tiles[x][y] == T_DIAMOND) {
          diamonds++;
        }
      }
    }
    sprintf(textbuf, "Diamonds:  %d   ", diamonds); // DS:04DC
    y = strlen(textbuf);
    TextOut(g_maindc,
      g_r_status_left.left + 5,
      g_r_status_left.top,
      textbuf, y);

    sprintf(textbuf, "Tool:  %s                    ", g_toolname); // DS:04ED
    y = strlen(textbuf);
    if (y > 25) {
      y = 25;
      textbuf[25] = 0;
    }
    TextOut(g_maindc,
      g_r_status_left.left + 110,
      g_r_status_left.top,
      textbuf,
      y);

    break;
  }
}

// CS:2A48 - ***CODE MATCH!***
void cs_2A48(HWND hDlg) {
  // [BP-2].w = temporary storage for segment for strcat

  strcpy(g_2D90, g_060C);
  strcat(g_2D90, g_068C);
  DlgDirList(hDlg, g_2D90, 404, 403, 0);

  // FIXME: 0x4010 is DDL_DRIVES | DDL_DIRECTORY, this seems to be missing from BC++3.0 headers! --GM
  DlgDirList(hDlg, g_069E, 405, 403, 0x4010); // DS:069E

  SetDlgItemText(hDlg, 401, g_068C); // DS:068C
}

// CS:2AE1 - ***CODE MATCH!***
void cs_2AE1(HWND hDlg, LPSTR a1, LPSTR a3, LPSTR a5) {
  LPSTR fol;
  char tmpc0;

  fol = a5 + lstrlen(a5);
  while (*fol != ':' && *fol != '\\' && fol > a5) {
    fol = AnsiPrev(a5, fol);
  }

  if (*fol != ':' && *fol != '\\') {
    lstrcpy(a3, a5);
    *a1 = 0;
    return;
  }

  lstrcpy(a3, fol+1);
  tmpc0 = fol[1];
  lstrcpy(a1, a5);
  fol[1] = tmpc0;
  a1[fol-a5+1] = 0;
}

// CS:2BA6 - ***CODE MATCH!***
void cs_2BA6(char *a0, const char *a1) {
  const char *fol; // SI

  for (fol = a1; *fol != 0 && *fol != '.'; fol++) {
    // Do nothing, just search
  }

  if (*fol != 0) {
    if (strchr(fol, '*') == NULL) {
      if (strchr(fol, '?') == NULL) {
        strcpy(a0, fol);
      }
    }
  }
}

// CS:2C31 - ***CODE MATCH!***
void cs_2C31(char *a0, const char *a1) {
  char *fol; // SI

  for (fol = a0; *fol != 0 && *fol != '.'; fol++) {
    // Do nothing, just search
  }

  if (*fol != '.') {
    strcat(a0, a1);
  }
}

// CS:2C83 - ***CODE MATCH!***
void cs_2C83(char *a0, const char *a1) {
  int tmp0; // AX

  tmp0 = strlen(a0);
  if (tmp0 > 0 && a0[tmp0-1] != '\\' && a0[tmp0-1] != ':') {
    strcat(a0, "\\"); // DS:06A9
  }

  strcat(a0, a1);
}

// CS:2CFB - ***CODE MATCH!***
int FAR PASCAL _export DLG_FOPEN_FUNC(HWND hDlg, WORD msg, WORD wParam, LONG lParam) {
  switch (msg) {
    case WM_COMMAND:
      switch (wParam) {
        case 404: // 2D42
          switch (HIWORD(lParam)) {
            case 1:
              if (!DlgDirSelect(hDlg, g_2D90, 404)) {
                SetDlgItemText(hDlg, 401, g_2D90);
                SendDlgItemMessage(hDlg, 401, WM_USER + 1, 0x0000, 0x7FFF0000UL);
              }
              break;

            case 2:
              goto unk_2E1D;
          }
          return 1;

        case 405: // 2D99
          switch (HIWORD(lParam)) {
            case 1: // 2DA8
              if (!DlgDirSelect(hDlg, g_2D90, 405)) {
                break;
              }

              strcat(g_2D90, g_068C);
              SetDlgItemText(hDlg, 401, g_2D90);
              SendDlgItemMessage(hDlg, 401, WM_USER + 1, 0x0000, 0x7FFF0000UL);
              break;

            case 2:
              goto unk_2E1D;
          }

          return 1;

        case 1:
        unk_2E1D:
          GetDlgItemText(hDlg, 401, g_050C, 128);
          if (strchr(g_050C, '*') != NULL || strchr(g_050C, '?') != NULL) {
            cs_2AE1(hDlg, g_2D90, g_068C, g_050C);
            if (g_2D90[0] != 0) {
              strcpy(g_060C, g_2D90);
            }
            // TODO: I think this might be backwards, and this string might have to be modifiable --GM
            cs_2BA6(g_068C, ".kye"); // DS:0699
            cs_2A48(hDlg);
            return 1;
          }

          cs_2C31(g_050C, ".kye"); // DS:0699
          GetDlgItemText(hDlg, 403, g_058C, 128);
          EndDialog(hDlg, 0);
          return 1;

        case 2: // 2F14
          EndDialog(hDlg, 0);
          g_050C[0] = 0;
          return 1;
      }

    case WM_INITDIALOG:
      cs_2A48(hDlg);
      SetDlgItemText(hDlg, 401, g_068C); // DS:068C
      SendDlgItemMessage(hDlg, 401, WM_USER + 1, 0x0000, 0x7FFF0000UL);
      SetFocus(GetDlgItem(hDlg, 401));
      return 0;
  }

  return 0;
}

// CS:2F89 - ***CODE MATCH!***
void cs_2F89(char *fname_buf) {
  FARPROC dlgproc;

  dlgproc = MakeProcInstance((FARPROC)DLG_FOPEN_FUNC, g_hInstance);
  DialogBox(g_hInstance, "DLG_OPENFILE", g_mainwnd, dlgproc); // DS:06AB
  FreeProcInstance(dlgproc);

  if (g_050C[0] != 0) {
    strcpy(fname_buf, g_058C);
    cs_2C83(fname_buf, g_050C);
  }
}

