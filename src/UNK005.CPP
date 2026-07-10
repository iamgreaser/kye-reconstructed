// vim: set syntax=cpp :
#include <windows.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kyetypes.h"
#include "kyeprocs.h"
#include "kyevars.h"

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
    src_px = ((abs(tile) - abs(T_WALL1)) * TILE_LX) + (3 * TILE_LX);
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

  // KLUDGE: Try to make the function a little smaller so we can at least get back into phase.
  px = cx * g_tile_lx;
  if (tile == T_DOOR_W_E || tile == T_DOOR_E_W) {
    //px = cx * g_tile_lx;
    py = cy * g_tile_ly;
    BitBlt(g_maindc, px, py, TILE_LX, TILE_LY, dc, 14 * TILE_LX, 0 * TILE_LY, SRCCOPY);
    return;
  }

  if (tile == T_DOOR_N_S || tile == T_DOOR_S_N) {
    //px = cx * g_tile_lx;
    py = cy * g_tile_ly;
    // 3 effective no-op pairs of bytes gets us back into phase.
    py++; py--; py++; py--; py++; py--;
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
void cs_2AE1(int a0_unk, LPSTR a1, LPSTR a3, LPSTR a5) {
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

// CS:2CFB
int FAR PASCAL _export DLG_FOPEN_FUNC(HWND hDlg, WORD msg, WORD wParam, LONG lParam) {
  // TODO! --GM
  return 0;
}

// CS:2F89
void cs_2F89(char *fname_buf) {
  // TODO! --GM
}

// CS:3006
void move_actor(int ai, int newx, int newy) {
  HDC dc; // DI

  // SI = ai

  dc = CreateCompatibleDC(g_maindc);
  // BUG: dc not checked for failure
  SelectObject(dc, g_hbmpBlock);
  // BUG: old object not saved
  delete_and_draw_unacting_tile(
    g_actors[ai].cx,
    g_actors[ai].cy);
  g_actors[ai].cx = newx;
  g_actors[ai].cy = newy;
  g_level_tiles[newx][newy] = ai;
  draw_actor(g_maindc, dc, ai);
  DeleteDC(dc);
}

// CS:3084
int apply_sticky_blocks(int ai) {
  // need 0x4 bytes of stack (BP/FC)
  int result; // BP/FC
  int cx; // BP/FE

  int cy; // CX

  result = 0;
  cx = g_actors[ai].cx;
  cy = g_actors[ai].cy;
  if (g_level_tiles[cx][cy+1] == T_EMPTY
      && g_level_tiles[cx][cy+2] >= 0
      && g_actors[g_level_tiles[cx][cy+2]].type == T_STICKY_V) {
    move_actor(ai, cx, cy+1);
    result = 1;
  } else if (g_level_tiles[cx][cy-1] == T_EMPTY
      && g_level_tiles[cx][cy-2] >= 0
      && g_actors[g_level_tiles[cx][cy-2]].type == T_STICKY_V) {
    move_actor(ai, cx, cy-1);
    result = 1;
  } else if (g_level_tiles[cx+1][cy] == T_EMPTY
      && g_level_tiles[cx+2][cy] >= 0
      && g_actors[g_level_tiles[cx+2][cy]].type == T_STICKY_H) {
    move_actor(ai, cx+1, cy);
    result = 1;
  } else if (g_level_tiles[cx-1][cy] == T_EMPTY
      && g_level_tiles[cx-2][cy] >= 0
      && g_actors[g_level_tiles[cx-2][cy]].type == T_STICKY_H) {
    move_actor(ai, cx-1, cy);
    result = 1;
  }

  return result;
}

// CS:31AC
int actor_held_by_sticky_block(int ai) {
  // need 0x2 bytes of stack
  int cx; // BP/FE

  int cy; // SI
  int result; // CX

  result = 0;
  cx = g_actors[ai].cx;
  cy = g_actors[ai].cy;

  if (g_level_tiles[cx][cy+1] >= 0
      && g_actors[g_level_tiles[cx][cy+1]].type == T_STICKY_V) {
    result = 1;
  } else if (g_level_tiles[cx][cy-1] >= 0
      && g_actors[g_level_tiles[cx][cy-1]].type == T_STICKY_V) {
    result = 1;
  } else if (g_level_tiles[cx+1][cy] >= 0
      && g_actors[g_level_tiles[cx+1][cy]].type == T_STICKY_H) {
    result = 1;
  } else if (g_level_tiles[cx-1][cy] >= 0
      && g_actors[g_level_tiles[cx-1][cy]].type == T_STICKY_H) {
    result = 1;
  }

  return result;
}

// CS:3374
void respawn_kye(void) {
  // Stack: 0x0A bytes
  int cy1; // BP/F6
  int cx1; // BP/F8
  int cy0; // BP/FA
  int cx0; // BP/FC
  int radius; // BP/FE

  int rad2; // SI
  int i; // CX

  g_kye_injured = 1;
  g_kye_target_cx = g_kye_spawn_cx;
  g_kye_target_cy = g_kye_spawn_cy;

  // Spiral out if spawn blocked
  if (g_level_tiles[g_kye_spawn_cx][g_kye_spawn_cy] != T_EMPTY) {
    for (radius = 1,
        cx0 = g_kye_spawn_cx - 1,
        cy0 = g_kye_spawn_cy + 1,
        rad2 = 2,
        cx1 = g_kye_spawn_cx + 1,
        cy1 = g_kye_spawn_cy - 1;
        radius <= 4; cx0--, cy0++, rad2 += 2, cx1--, cy1++, radius++) {

      g_kye_target_cx = cx0;
      g_kye_target_cy = cy0;
      for (i = 0; i < rad2; g_kye_target_cx++, i++) {
        if (g_level_tiles[g_kye_target_cx][g_kye_target_cy] == T_EMPTY) {
          return;
        }
      }
      g_kye_target_cx = cx1;
      g_kye_target_cy = cy0;
      for (i = 0; i < rad2; g_kye_target_cy--, i++) {
        if (g_level_tiles[g_kye_target_cx][g_kye_target_cy] == T_EMPTY) {
          return;
        }
      }
      g_kye_target_cx = cx1;
      g_kye_target_cy = cy1;
      for (i = 0; i < rad2; g_kye_target_cx--, i++) {
        if (g_level_tiles[g_kye_target_cx][g_kye_target_cy] == T_EMPTY) {
          return;
        }
      }
      g_kye_target_cx = cx0;
      g_kye_target_cy = cy1;
      for (i = 0; i < rad2; g_kye_target_cy++, i++) {
        if (g_level_tiles[g_kye_target_cx][g_kye_target_cy] == T_EMPTY) {
          return;
        }
      }
    }

    // Respawn in-place
    g_kye_target_cx = g_kye_main_cx;
    g_kye_target_cy = g_kye_main_cy;
  }
}

// CS:34ED
int try_killing_adjacent_kye(int ai) {
  // stack: 0x02 bytes
  int cx; // BP/FE

  int cy; // CX
  int result; // SI

  cx = g_actors[ai].cx;
  cy = g_actors[ai].cy;
  result = (g_level_tiles[cx][cy+1] == T_KYE
      || g_level_tiles[cx][cy-1] == T_KYE
      || g_level_tiles[cx-1][cy] == T_KYE
      || g_level_tiles[cx+1][cy] == T_KYE);

  if (result) {
    g_kye_lives--;
    respawn_kye();
    repaint_status_left();
  }
  return result;
}

// CS:3562
void tick_all_timer_blocks(void) {
  int ai; // SI
  int cx; // DX
  int cy; // CX

  if (g_tick_counter % 30 == 0) {
    for (ai = 0; ai < g_actor_count; ai++) {
      if (g_actors[ai].type >= T_TIMER_0
          && g_actors[ai].type <= T_TIMER_9) {
        //
        cx = g_actors[ai].cx;
        cy = g_actors[ai].cy;
        if (g_actors[ai].type > T_TIMER_0) {
          g_actors[ai].type--;
          move_actor(ai, cx, cy);
        } else {
          delete_and_draw_unacting_tile(cx, cy);
          mark_actor_for_deletion(ai);
        }
      }
    }
  }
  garbage_collect_actors();
}

// CS:35EA
int try_enter_blacky(int ai, int cx, int cy) {
  // stack: 0x06 bytes
  // BP/FA = tile row ptr
  int src_cy; // BP/FC
  int src_cx; // BP/FE

  int dst_ai; // DX?
  // SI = ai
  // DI = cx
  // CX = cy

  if (g_level_tiles[cx][cy] >= 0) {
   dst_ai = g_level_tiles[cx][cy];
   if (g_actors[dst_ai].type == T_BLACKY1) {
     src_cx = g_actors[ai].cx;
     src_cy = g_actors[ai].cy;
     g_actors[dst_ai].type = T_BLACKY2;
     g_actors[dst_ai].var0 = 0;
     move_actor(dst_ai, cx, cy);
     delete_and_draw_unacting_tile(src_cx, src_cy);
     mark_actor_for_deletion(ai);
     return 1;
   }
  }

  return 0;
}

// CS:3684
void tick_game_state(void) {
  // stack: 0x12 bytes
  int newy; // BP/EE
  actor_s *actor; // BP/F0
  int unkFFF2; // BP/F2
  // BP/F4
  int dy; // BP/FA
  int cy; // BP/FC
  int cx; // BP/FE
  // BP-0x00

  int ai; // SI
  int other; // DI - other tile type
  int dx; // DI
  int newx; // CX

  // TODO: Work out how this actually goes --GM
  int slide_left;
  int slide_right;

  // BUG: Counter wraparound is not seamless.
  // We cover 32001 different values.
  // If left alone, this takes less than an hour.
  // A counter wraparound breaks DEFAULT.KYE's COUNTERS level,
  // as the bouncer at the bottom steps ahead a bit too early.
  g_tick_counter++;
  if (g_tick_counter > 32000) {
    g_tick_counter = 0;
  }

  tick_all_timer_blocks();
  for (ai = 0, unkFFF2 = 0, actor = &g_actors[0];
      ai < g_actor_count;
      ai++, actor++, unkFFF2 += 8) {
    //
    cx = actor->cx;
    cy = actor->cy;
    switch(actor->type) {
    case T_SBLOCK:
    case T_ROT_A:
    case T_ROT_C:
    case T_RBLOCK:
    case T_AUTOSLIDER1:
    case T_AUTOSLIDER2:
    case T_AUTOSLIDER3:
    case T_AUTOSLIDER4:
      apply_sticky_blocks(ai);
      break;

    // BUG: North-facing sliders do not redraw on rotation.
    case T_SLIDER_N:
      if ((!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        other = g_level_tiles[cx][cy-1];
        if (other == T_EMPTY) {
          move_actor(ai, cx, cy-1);
        } else {
          if (!try_enter_blacky(ai, cx, cy-1)) {
            if (other >= 0 && g_actors[other].type == T_ROT_A) {
              actor->type = T_SLIDER_E;
              // BUG: This needs to redraw the actor!
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              actor->type = T_SLIDER_W;
              // BUG: This needs to redraw the actor!
            }
          }
        }
      }
      break;

    case T_SLIDER_S:
      if ((!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        other = g_level_tiles[cx][cy+1];
        if (other == T_EMPTY) {
          move_actor(ai, cx, cy+1);
        } else {
          if (!try_enter_blacky(ai, cx, cy+1)) {
            if (other >= 0 && g_actors[other].type == T_ROT_A) {
              actor->type = T_SLIDER_W;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              actor->type = T_SLIDER_E;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_SLIDER_W:
      if ((!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        other = g_level_tiles[cx-1][cy];
        if (other == T_EMPTY) {
          move_actor(ai, cx-1, cy);
        } else {
          if (!try_enter_blacky(ai, cx-1, cy)) {
            if (other >= 0 && g_actors[other].type == T_ROT_A) {
              actor->type = T_SLIDER_N;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              actor->type = T_SLIDER_S;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_SLIDER_E:
      if ((!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        other = g_level_tiles[cx+1][cy];
        if (other == T_EMPTY) {
          move_actor(ai, cx+1, cy);
        } else {
          if (!try_enter_blacky(ai, cx+1, cy)) {
            if (other >= 0 && g_actors[other].type == T_ROT_A) {
              actor->type = T_SLIDER_S;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              actor->type = T_SLIDER_N;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_STICKY_V:
      if (g_level_tiles[cx][cy+1] == T_EMPTY
          && g_level_tiles[cx][cy+2] == T_KYE) {
        move_actor(ai, cx, cy+1);
      } else if (g_level_tiles[cx][cy-1] == T_EMPTY
          && g_level_tiles[cx][cy-2] == T_KYE) {
        move_actor(ai, cx, cy-1);
      } else if (g_level_tiles[cx+1][cy] == T_EMPTY
          && g_level_tiles[cx+2][cy] >= 1
          && g_actors[g_level_tiles[cx+2][cy]].type == T_STICKY_H) {
        move_actor(ai, cx+1, cy);
      } else if (g_level_tiles[cx-1][cy] == T_EMPTY
          && g_level_tiles[cx-2][cy] >= 1
          && g_actors[g_level_tiles[cx-2][cy]].type == T_STICKY_H) {
        move_actor(ai, cx-1, cy);
      }
      break;

    case T_STICKY_H:
      if (g_level_tiles[cx+1][cy] == T_EMPTY
          && g_level_tiles[cx+2][cy] == T_KYE) {
        move_actor(ai, cx+1, cy);
      } else if (g_level_tiles[cx-1][cy] == T_EMPTY
          && g_level_tiles[cx-2][cy] == T_KYE) {
        move_actor(ai, cx-1, cy);
      } else if (g_level_tiles[cx][cy+1] == T_EMPTY
          && g_level_tiles[cx][cy+2] >= 1
          && g_actors[g_level_tiles[cx][cy+2]].type == T_STICKY_V) {
        move_actor(ai, cx, cy+1);
      } else if (g_level_tiles[cx][cy-1] == T_EMPTY
          && g_level_tiles[cx][cy-2] >= 1
          && g_actors[g_level_tiles[cx][cy-2]].type == T_STICKY_V) {
        move_actor(ai, cx, cy-1);
      }
      break;

    case T_BOUNCER_N:
      if (g_tick_counter % 5 == 0
          && (!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        //
        if (g_level_tiles[cx][cy-1] == T_EMPTY) {
          move_actor(ai, cx, cy-1);
        } else if (!try_enter_blacky(ai, cx, cy-1)) {
          actor->type = T_BOUNCER_S;
          move_actor(ai, cx, cy);
          if (g_level_tiles[cx][cy-1] >= 0) {
            other = g_level_tiles[cx][cy-1];
            cx = g_actors[other].cx;
            cy = g_actors[other].cy - 1;
            if (g_level_tiles[cx][cy] == T_EMPTY
                || (g_level_tiles[cx][cy] >= 0
                  && g_actors[g_level_tiles[cx][cy]].type == T_BLACKY1)) {
              //
              if (!try_enter_blacky(other, cx, cy)) {
                move_actor(other, cx, cy);
              }
            }
          }
        }
      }
      break;

    case T_BOUNCER_S:
      if (g_tick_counter % 5 == 0
          && (!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        //
        if (g_level_tiles[cx][cy+1] == T_EMPTY) {
          move_actor(ai, cx, cy+1);
        } else if (!try_enter_blacky(ai, cx, cy+1)) {
          actor->type = T_BOUNCER_N;
          move_actor(ai, cx, cy);
          if (g_level_tiles[cx][cy+1] >= 0) {
            other = g_level_tiles[cx][cy+1];
            cx = g_actors[other].cx;
            cy = g_actors[other].cy + 1;
            if (g_level_tiles[cx][cy] == T_EMPTY
                || (g_level_tiles[cx][cy] >= 0
                  && g_actors[g_level_tiles[cx][cy]].type == T_BLACKY1)) {
              //
              if (!try_enter_blacky(other, cx, cy)) {
                move_actor(other, cx, cy);
              }
            }
          }
        }
      }
      break;

    case T_BOUNCER_W:
      if (g_tick_counter % 5 == 0
          && (!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        //
        if (g_level_tiles[cx-1][cy] == T_EMPTY) {
          move_actor(ai, cx-1, cy);
        } else if (!try_enter_blacky(ai, cx-1, cy)) {
          actor->type = T_BOUNCER_E;
          move_actor(ai, cx, cy);
          if (g_level_tiles[cx-1][cy] >= 0) {
            other = g_level_tiles[cx-1][cy];
            cx = g_actors[other].cx - 1;
            cy = g_actors[other].cy;
            if (g_level_tiles[cx][cy] == T_EMPTY
                || (g_level_tiles[cx][cy] >= 0
                  && g_actors[g_level_tiles[cx][cy]].type == T_BLACKY1)) {
              //
              if (!try_enter_blacky(other, cx, cy)) {
                move_actor(other, cx, cy);
              }
            }
          }
        }
      }
      break;

    case T_BOUNCER_E:
      if (g_tick_counter % 5 == 0
          && (!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        //
        if (g_level_tiles[cx+1][cy] == T_EMPTY) {
          move_actor(ai, cx+1, cy);
        } else if (!try_enter_blacky(ai, cx+1, cy)) {
          actor->type = T_BOUNCER_W;
          move_actor(ai, cx, cy);
          if (g_level_tiles[cx+1][cy] >= 0) {
            other = g_level_tiles[cx+1][cy];
            cx = g_actors[other].cx + 1;
            cy = g_actors[other].cy;
            if (g_level_tiles[cx][cy] == T_EMPTY
                || (g_level_tiles[cx][cy] >= 0
                  && g_actors[g_level_tiles[cx][cy]].type == T_BLACKY1)) {
              //
              if (!try_enter_blacky(other, cx, cy)) {
                move_actor(other, cx, cy);
              }
            }
          }
        }
      }
      break;

    case T_ROCKY_N:
      if ((!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        other = g_level_tiles[cx][cy-1];
        if (other == T_EMPTY) {
          move_actor(ai, cx, cy-1);
        } else {
          if (!try_enter_blacky(ai, cx, cy-1)) {
            // FIXME this is coded differently --GM
            slide_left = slide_right = 0;
            if (other >= 0
                && g_actors[other].type >= T_ROCKY_N
                && g_actors[other].type <= T_ROCKY_E) {
              slide_left = slide_right = 1;
            } else if (other >= 0
                && g_actors[other].type == T_RBLOCK) {
              slide_left = slide_right = 1;
            } else if (other == T_WALL2) {
              slide_left = slide_right = 1;
            } else if (other == T_WALL1 || other == T_WALL4) {
              slide_left = 1;
            } else if (other == T_WALL3 || other == T_WALL6) {
              slide_right = 1;
            }
            slide_left = slide_left && g_level_tiles[cx-1][cy] == T_EMPTY;
            slide_right = slide_right && g_level_tiles[cx+1][cy] == T_EMPTY;
            slide_left = slide_left && g_level_tiles[cx-1][cy-1] == T_EMPTY;
            slide_right = slide_right && g_level_tiles[cx+1][cy-1] == T_EMPTY;

            if (slide_left && slide_right) {
              // TODO understand rand() usage --GM
              move_actor(ai, cx + 1 - (2*(rand()%2)), cy - 1);
            } else if (slide_right) {
              move_actor(ai, cx + 1, cy - 1);
            } else if (slide_left) {
              move_actor(ai, cx - 1, cy - 1);
            } else if (other >= 0 && g_actors[other].type == T_ROT_A) {
              actor->type = T_ROCKY_E;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              actor->type = T_ROCKY_W;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_ROCKY_S:
      if ((!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        other = g_level_tiles[cx][cy+1];
        if (other == T_EMPTY) {
          move_actor(ai, cx, cy+1);
        } else {
          if (!try_enter_blacky(ai, cx, cy+1)) {
            // FIXME this is coded differently --GM
            slide_left = slide_right = 0;
            if (other >= 0
                && g_actors[other].type >= T_ROCKY_N
                && g_actors[other].type <= T_ROCKY_E) {
              slide_left = slide_right = 1;
            } else if (other >= 0
                && g_actors[other].type == T_RBLOCK) {
              slide_left = slide_right = 1;
            } else if (other == T_WALL8) {
              slide_left = slide_right = 1;
            } else if (other == T_WALL9 || other == T_WALL6) {
              slide_left = 1;
            } else if (other == T_WALL7 || other == T_WALL4) {
              slide_right = 1;
            }
            slide_left = slide_left && g_level_tiles[cx+1][cy] == T_EMPTY;
            slide_right = slide_right && g_level_tiles[cx-1][cy] == T_EMPTY;
            slide_left = slide_left && g_level_tiles[cx+1][cy+1] == T_EMPTY;
            slide_right = slide_right && g_level_tiles[cx-1][cy+1] == T_EMPTY;

            if (slide_left && slide_right) {
              // TODO understand rand() usage --GM
              move_actor(ai, cx + 1 - (2*(rand()%2)), cy + 1);
            } else if (slide_right) {
              move_actor(ai, cx - 1, cy + 1);
            } else if (slide_left) {
              move_actor(ai, cx + 1, cy + 1);
            } else if (other >= 0 && g_actors[other].type == T_ROT_A) {
              actor->type = T_ROCKY_W;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              actor->type = T_ROCKY_E;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_ROCKY_W:
      if ((!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        other = g_level_tiles[cx-1][cy];
        if (other == T_EMPTY) {
          move_actor(ai, cx-1, cy);
        } else {
          if (!try_enter_blacky(ai, cx-1, cy)) {
            // FIXME this is coded differently --GM
            slide_left = slide_right = 0;
            if (other >= 0
                && g_actors[other].type >= T_ROCKY_N
                && g_actors[other].type <= T_ROCKY_E) {
              slide_left = slide_right = 1;
            } else if (other >= 0
                && g_actors[other].type == T_RBLOCK) {
              slide_left = slide_right = 1;
            } else if (other == T_WALL6) {
              slide_left = slide_right = 1;
            } else if (other == T_WALL3 || other == T_WALL2) {
              slide_left = 1;
            } else if (other == T_WALL9 || other == T_WALL8) {
              slide_right = 1;
            }
            slide_left = slide_left && g_level_tiles[cx][cy+1] == T_EMPTY;
            slide_right = slide_right && g_level_tiles[cx][cy-1] == T_EMPTY;
            slide_left = slide_left && g_level_tiles[cx-1][cy+1] == T_EMPTY;
            slide_right = slide_right && g_level_tiles[cx-1][cy-1] == T_EMPTY;

            if (slide_left && slide_right) {
              // TODO understand rand() usage --GM
              move_actor(ai, cx - 1, cy + 1 - (2*(rand()%2)));
            } else if (slide_right) {
              move_actor(ai, cx - 1, cy - 1);
            } else if (slide_left) {
              move_actor(ai, cx - 1, cy + 1);
            } else if (other >= 0 && g_actors[other].type == T_ROT_A) {
              actor->type = T_ROCKY_N;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              actor->type = T_ROCKY_S;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_ROCKY_E:
      if ((!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        other = g_level_tiles[cx+1][cy];
        if (other == T_EMPTY) {
          move_actor(ai, cx+1, cy);
        } else {
          if (!try_enter_blacky(ai, cx+1, cy)) {
            // FIXME this is coded differently --GM
            slide_left = slide_right = 0;
            if (other >= 0
                && g_actors[other].type >= T_ROCKY_N
                && g_actors[other].type <= T_ROCKY_E) {
              slide_left = slide_right = 1;
            } else if (other >= 0
                && g_actors[other].type == T_RBLOCK) {
              slide_left = slide_right = 1;
            } else if (other == T_WALL4) {
              slide_left = slide_right = 1;
            } else if (other == T_WALL7 || other == T_WALL8) {
              slide_left = 1;
            } else if (other == T_WALL1 || other == T_WALL2) {
              slide_right = 1;
            }
            slide_left = slide_left && g_level_tiles[cx][cy-1] == T_EMPTY;
            slide_right = slide_right && g_level_tiles[cx][cy+1] == T_EMPTY;
            slide_left = slide_left && g_level_tiles[cx+1][cy-1] == T_EMPTY;
            slide_right = slide_right && g_level_tiles[cx+1][cy+1] == T_EMPTY;

            if (slide_left && slide_right) {
              // TODO understand rand() usage --GM
              move_actor(ai, cx + 1, cy + 1 - (2*(rand()%2)));
            } else if (slide_right) {
              move_actor(ai, cx + 1, cy + 1);
            } else if (slide_left) {
              move_actor(ai, cx + 1, cy - 1);
            } else if (other >= 0 && g_actors[other].type == T_ROT_A) {
              actor->type = T_ROCKY_S;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              actor->type = T_ROCKY_N;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_B_TWISTER:
    case T_B_GNASHER:
    case T_B_BLOB:
    case T_B_VIRUS:
    case T_B_SPIKE:
      // BUG: If a monster kills kye on its tick, it loses that tick
      if ((!try_killing_adjacent_kye(ai)) && g_tick_counter % 3 == 0) {
        dx = 0;
        dy = 0;
        if (!apply_sticky_blocks(ai)) {
          if (!actor_held_by_sticky_block(ai)) {

            // Apply direction
            newx = cx + dx;
            newy = cy + dy;
            if (g_level_tiles[cx][cy] == T_EMPTY
                || g_level_tiles[cx][cy] == T_BLACKY1) {
              cx = newx;
              cy = newy;
            }
          }
          if (!try_enter_blacky(ai, cx, cy)) {
            move_actor(ai, cx, cy);
          }
        }
      }
      break;

    case T_TIMER_0:
    case T_TIMER_1:
    case T_TIMER_2:
    case T_TIMER_3:
    case T_TIMER_4:
    case T_TIMER_5:
    case T_TIMER_6:
    case T_TIMER_7:
    case T_TIMER_8:
    case T_TIMER_9:
      apply_sticky_blocks(ai);
      break;
    }
  }
  garbage_collect_actors();
}
// ends at CS:49F2 after 1 byte of padding

// CS:4B7A
void cs_4B7A(int cx, int cy) {
  // TODO! --GM
}

// CS:501F
int cs_501F(void) {
  // TODO: Optionally register + open the editor tools window --GM
  // TODO: Show the editor tools window --GM
  return 1;
}

// CS:5119
int cs_5119(void) {
  // TODO: Hide the editor tools window --GM
  // ShowWindow(DS:2E90, SW_HIDE);
  return 1;
}

// CS:5128
void count_all_diamonds(void) {
  // needs 0x2 bytes of stack
  // BP/FE - running row pointer

  int diamonds; // CX
  int cy; // DX
  int cx; // DI

  diamonds = 0;
  for (cx = 0; cx < LEVEL_LX; cx++) {
    for (cy = 0; cy < LEVEL_LY; cy++) {
      if (g_level_tiles[cx][cy] == T_DIAMOND) {
        diamonds++;
      }
    }
  }
  if (diamonds == 0) {
    g_level_complete = 1;
  }
}

// CS:5165
void draw_kye_mouse_ghost(void) {
  // stack: 0x04 bytes
  int py; // BP/FC
  int px; // BP/FE

  HDC dc; // SI

  dc = CreateCompatibleDC(g_maindc);
  SelectObject(dc, g_hbmpKye);
  px = g_kye_mouse_cx * g_tile_lx;
  py = g_kye_mouse_cy * g_tile_ly; // stored value ignored, optimised to use AX
  BitBlt(g_maindc,
    px, py,
    TILE_LX, TILE_LY,
    dc,
    1 * TILE_LX, 0 * TILE_LY,
    SRCCOPY);

  DeleteDC(dc);
}

// CS:51B9
void clear_kye_mouse_ghost(void) {
  if (g_02AA != 0 && g_level_tiles[g_kye_mouse_cx][g_kye_mouse_cy] == T_EMPTY) {
    delete_and_draw_unacting_tile(g_kye_mouse_cx, g_kye_mouse_cy);
  }
  g_02AA = 0;
}

// CS:51EE
void update_kye_mouse_target(int cx, int cy) {
  // SI = cx
  // DI = cy
  if (g_02AA == 0 || cx != g_kye_mouse_cx || cy != g_kye_mouse_cy) {
    clear_kye_mouse_ghost();
    g_kye_mouse_cx = cx;
    g_kye_mouse_cy = cy;
    if (abs(cx - g_kye_main_cx) > 1 || abs(cy - g_kye_main_cy) > 1) {
      if (g_level_tiles[cx][cy] == T_EMPTY) {
        draw_kye_mouse_ghost();
        g_02AA = 1;
      }
    }
  }
}

// CS:525D
void repaint_kye(int fizzle_mode) {
  int py; // BP/FC
  int px; // BP/FE

  int i; // SI, first
  HDC dc; // SI, second

  if (fizzle_mode == 1) {
    for (i = TILE_LX*TILE_LY; i >= 0; i -= TILE_LY) {
      draw_fizzled_kye(i);
    }
  } else if (fizzle_mode == 2) {
    for (i = 0; i < TILE_LX*TILE_LY; i += TILE_LY) {
      draw_fizzled_kye(i);
    }
    return;
  }

  dc = CreateCompatibleDC(g_maindc);
  // BUG: dc not checked for null handle on creation
  px = g_kye_main_cx * g_tile_lx;
  py = g_kye_main_cy * g_tile_ly;
  SelectObject(dc, g_hbmpKye);
  BitBlt(
    g_maindc,
    px, py,
    TILE_LX, TILE_LY,
    dc,
    0, 0,
    SRCCOPY);

  DeleteDC(dc);
}

// CS:52EA
void move_kye(int dx, int dy) {
  // int // BP/FE

  HDC dc; // SI

  if (g_tile_under_kye == T_EMPTY) {
    delete_and_draw_unacting_tile(g_kye_main_cx, g_kye_main_cy);
  } else {
    g_level_tiles[g_kye_main_cx][g_kye_main_cy] = g_tile_under_kye;
    dc = CreateCompatibleDC(g_maindc);
    // BUG: dc not checked for failure
    SelectObject(dc, g_hbmpWall);
    draw_wall(g_kye_main_cx, g_kye_main_cy, dc);
    DeleteDC(dc);
  }
  g_kye_main_cx += dx;
  g_kye_main_cy += dy;
  g_tile_under_kye = g_level_tiles[g_kye_main_cx][g_kye_main_cy];
  g_level_tiles[g_kye_main_cx][g_kye_main_cy] = T_KYE;
  repaint_kye(0);
}

// CS:5395
void try_move_kye(int dx, int dy) {
  // Stack: 0x2E bytes (BP/D2 start)
  int dy2; // BP/D2
  int dx2; // BP/D4
  // BP/D6 - pointer premultiplier
  // BP/D8
  // FIXME: This needs to be placed into a later file! --GM
#if 0
  int circle_y[4] = {0,1,0,-1}; // BP/DA const from DS:0D62
  int circle_x[4] = {1,0,-1,0}; // BP/E2 const from DS:0D5A
#endif
  // BP/EA
  // BP/EC
  int didmove; // BP/EE
  int pushy; // BP/F0
  int pushx; // BP/F2
  int newx; // BP/F4
  int pushdiagblocked; // BP/F6
  int blocked; // BP/F8
  // BP/FA
  int ai; // BP/FC
  // BP/FE
  // BP+0x00
  int newy; // CX

  // DX = dx;
  // BX = dy;
  didmove = 0;
  blocked = 0;
  pushdiagblocked = 0;
  // DI = dx;
  // SI = dy;
  newx = g_kye_main_cx + dx;
  newy = g_kye_main_cy + dy;

  // Check if diagonal is valid
  if (dx != 0 && dy != 0) {
    blocked = !(g_level_tiles[newx][g_kye_main_cy] == T_EMPTY
             && g_level_tiles[g_kye_main_cx][newy] == T_EMPTY);
    if (blocked) {
      if (g_moving_by_mouse != 0) {
        if (g_level_tiles[newx][g_kye_main_cy] == T_EMPTY) {
          dy = 0;
          newy = g_kye_main_cy;
          blocked = 0;
        } else if (g_level_tiles[g_kye_main_cx][newy]) {
          dx = 0;
          newx = g_kye_main_cx;
          blocked = 0;
        }
      }
    }
  }

  if (!blocked) {
    switch(g_level_tiles[newx][newy]) {
    case T_EMPTY:
      move_kye(dx, dy);
      didmove = 1;
      break;
    case T_EARTH:
      move_kye(dx, dy);
      g_tile_under_kye = T_EMPTY;
      didmove = 1;
      break;
    case T_DIAMOND:
      move_kye(dx, dy);
      g_tile_under_kye = T_EMPTY;
      didmove = 1;
      count_all_diamonds();
      repaint_status_left();
      break;

    // These don't set didmove.
    // There may be a reason for it...
    case T_DOOR_W_E:
      if (dy == 0 && dx == 1) {
        move_kye(1, 0);
      }
      break;
    case T_DOOR_E_W:
      if (dy == 0 && dx == -1) {
        move_kye(-1, 0);
      }
      break;
    case T_DOOR_N_S:
      if (dy == 1 && dx == 0) {
        move_kye(0, 1);
      }
      break;
    case T_DOOR_S_N:
      if (dy == -1 && dx == 0) {
        move_kye(0, -1);
      }
      break;

    // Then we get into actor handling...
    default:
      if (g_level_tiles[newx][newy] >= 0) {
        ai = g_level_tiles[newx][newy];
        switch (g_actors[ai].type) {
          case T_BLACKY1:
            g_kye_lives--;
            respawn_kye();
            repaint_status_left();
            return;
          case T_BLACKY2:
            return;
        }
        pushx = g_actors[ai].cx + dx;
        pushy = g_actors[ai].cy + dy;
        if (dx != 0 && dy != 0) {
          pushdiagblocked = !(g_level_tiles[pushx][g_actors[ai].cy] == T_EMPTY
                           && g_level_tiles[g_actors[ai].cx][pushy] == T_EMPTY);
        }

        if (g_level_tiles[pushx][pushy] == T_EMPTY) {
          if (!pushdiagblocked) {
            move_actor(ai, pushx, pushy);
            move_kye(dx, dy);
            didmove = 1;
          }
        } else if (g_level_tiles[pushx][pushy] >= 0
            && g_actors[g_level_tiles[pushx][pushy]].type == T_BLACKY1
            && !pushdiagblocked) {
          // POTENTIAL BUG: If try_enter_blacky fails then this breaks.
          // An actor will clobber an actor, or Kye will clobber an actor.
          if (!try_enter_blacky(ai, pushx, pushy)) {
            move_actor(ai, pushx, pushy);
          }
          move_kye(dx, dy);
        }
      }
    }
  }
  // TODO: Handle the weird thing here --GM
  // (this uses circle_x, circle_y to detect various movements?)
  if (g_moving_by_mouse != 0 && !didmove) {
    // TODO! --GM
  }
}

// CS:57F5
void tick_kye(void) {
  int dx; // AX
  int dy; // DX

  if (g_kye_injured != 0) {
    repaint_kye(2);
    delete_and_draw_unacting_tile(g_kye_main_cx, g_kye_main_cy);
    g_kye_main_cx = g_kye_target_cx;
    g_kye_main_cy = g_kye_target_cy;
    g_level_tiles[g_kye_target_cx][g_kye_target_cy] = T_KYE;
    repaint_kye(1);
    g_kye_injured = 0;
    return;
  }

  if ((g_kye_target_cx != g_kye_main_cx)
      || (g_kye_target_cy != g_kye_main_cy)) {
    if (g_kye_target_cx < g_kye_main_cx) {
      dx = -1;
    } else if (g_kye_target_cx > g_kye_main_cx) {
      dx = 1;
    } else {
      dx = 0;
    }
    if (g_kye_target_cy < g_kye_main_cy) {
      dy = -1;
    } else if (g_kye_target_cy > g_kye_main_cy) {
      dy = 1;
    } else {
      dy = 0;
    }
    try_move_kye(dx, dy);
  }
}

// CS:5898
void animate_diamonds(void) {
  // Stack 0x0E (BP/F2)
  HDC dc; // BP/F4
  int src_py; // BP/F6
  int src_px; // BP/F8
  int py; // BP/FA
  int px; // BP/FC
  int cx; // BP/FE

  int cy; // SI

  if (g_tick_counter % 10 == 0) {
    dc = CreateCompatibleDC(g_maindc);
    SelectObject(dc, g_hbmpWall);
    for (cx = 0; cx < LEVEL_LX; cx++) {
      for (cy = 0; cy < LEVEL_LY; cy++) {
        if (g_level_tiles[cx][cy] == T_DIAMOND) {
          px = cx * g_tile_lx;
          py = cy * g_tile_ly;
          src_px = 12 * TILE_LX;
          src_py = 0 * TILE_LY;
          // TODO: Work out how rand() is used --GM
          if (rand() % 2 == 0) {
            src_py = 1 * TILE_LY;
          }
          BitBlt(g_maindc,
            px, py,
            TILE_LX, TILE_LY,
            dc,
            src_px, src_py,
            SRCCOPY);
        }
      }
    }

    DeleteDC(dc);
  }
}

// CS:5961
void animate_doors(void) {
  // stack: 0x08 bytes (/F8)
  // BP/F8
  HDC dc; // BP/FA
  int src_py; // BP/FC
  int cx; // BP/FE

  int cy; // DI
  int px; // BX
  int py; // DX (somehow unused as such)

  if (g_tick_counter % 4 == 0) {
    dc = CreateCompatibleDC(g_maindc);
    SelectObject(dc, g_hbmpWall);

    if (g_door_anim_flag) {
      src_py = 0 * TILE_LY;
      g_door_anim_flag = 0;
    } else {
      src_py = 1 * TILE_LY;
      g_door_anim_flag = 1;
    }

    for (cx = 0; cx < LEVEL_LX; cx++) {
      for (cy = 0; cy < LEVEL_LY; cy++) {
        switch(g_level_tiles[cx][cy]) {
        case T_DOOR_E_W:
        case T_DOOR_W_E:
          px = cx * g_tile_lx;
          py = cy * g_tile_ly;
          BitBlt(g_maindc,
            px, py,
            TILE_LX, TILE_LY,
            dc,
            14 * TILE_LX, src_py,
            SRCCOPY);
          break;
        case T_DOOR_N_S:
        case T_DOOR_S_N:
          px = cx * g_tile_lx;
          py = cy * g_tile_ly;
          BitBlt(g_maindc,
            px, py,
            TILE_LX, TILE_LY,
            dc,
            13 * TILE_LX, src_py,
            SRCCOPY);
          break;
        }
      }
    }

    DeleteDC(dc);
  }
}

// CS:5A54
void animate_autospawners(void) {
  // TODO! --GM
}

// CS:5C8A
void animate_blackies(void) {
  // stack: 0x08 bytes (/F8)
  // BP/F8
  HDC dc; // BP/FA
  int py; // BP/FC
  int i; // BP/FE

  int px; // CX
  int src_py; // DX

  // DI = actor table offset
  // BP/F8 = actor ptr
  // SI = actor->var0 ptr

  if (g_tick_counter % 5 == 0) {
    dc = CreateCompatibleDC(g_maindc);
    SelectObject(dc, g_hbmpBlock);
    for (i = 0; i < g_actor_count; i++) {
      switch (g_actors[i].type) {
      case T_BLACKY1:
        px = g_actors[i].cx * g_tile_lx;
        py = g_actors[i].cy * g_tile_ly;
        src_py = (g_actors[i].var0 * TILE_LY) + (4 * TILE_LY);
        BitBlt(g_maindc,
          px, py,
          TILE_LX, TILE_LY,
          dc,
          8 * TILE_LX,
          src_py,
          SRCCOPY);
        g_actors[i].var0 = (g_actors[i].var0 + 1) % 4;
        break;
      case T_BLACKY2:
        px = g_actors[i].cx * g_tile_lx;
        py = g_actors[i].cy * g_tile_ly;
        src_py = (g_actors[i].var0 * TILE_LY) + (4 * TILE_LY);
        BitBlt(g_maindc,
          px, py,
          TILE_LX, TILE_LY,
          dc,
          9 * TILE_LX,
          src_py,
          SRCCOPY);
        if (g_actors[i].var0 == 3) {
          g_actors[i].type = T_BLACKY1;
        }
        g_actors[i].var0 = (g_actors[i].var0 + 1) % 4;
        break;
      }
    }
    DeleteDC(dc);
  }
}

// CS:5D9C
void animate_monsters(void) {
  // needs 0x8 bytes of stack (/F8)
  int actor_type; // BP/F8
  HDC dc; // BP/FA
  int py; // BP/FC
  int i; // BP/FE

  int px; // CX
  int src_px; // DX
  int src_py; // BX
  // SI = i*sizeof(g_actors[i]);
  // DI = running counter for &g_actors[i].var0;

  if (g_tick_counter % 3 == 0) {
    dc = CreateCompatibleDC(g_maindc);
    SelectObject(dc, g_hbmpBlock);
    for (i = 0; i < g_actor_count; i++) {
      actor_type = g_actors[i].type;
      switch (actor_type) {
      case T_B_TWISTER:
      case T_B_GNASHER:
      case T_B_BLOB:
      case T_B_VIRUS:
      case T_B_SPIKE:
        px = g_actors[i].cx * g_tile_lx;
        py = g_actors[i].cy * g_tile_ly;
        src_px = actor_type * TILE_LX;
        src_py = g_actors[i].var0 * TILE_LY;
        BitBlt(g_maindc,
          px, py,
          TILE_LX, TILE_LY,
          dc,
          src_px, src_py,
          SRCCOPY);
        g_actors[i].var0 = (g_actors[i].var0 + 1) % 4;
        break;
      }
    }

    DeleteDC(dc);
  }
}

// CS:5E61
void animate_everything(void) {
  animate_monsters();
  animate_diamonds();
  animate_doors();
  animate_autospawners();
  animate_blackies();
}
