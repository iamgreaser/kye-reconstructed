// vim: set syntax=cpp :
// Any globals that have yet to be placed in the right module.

#include <windows.h>
#include <stdio.h>

#include <stdlib.h>

#include "kyetypes.h"
#include "kyeprocs.h"
#include "kyevars.h"

// This variable is why a module split is required after the CS:5395 function.
// This is the first aligned point, and it seems to make sense.
int g_door_anim_flag = 1; // DS:0D6A

// CS:5898 - ***CODE MATCH!***
void animate_diamonds(void) {
  // Stack 0x0E (BP/F2)
  int cx; // BP/FE
  int cy; // SI
  int px; // BP/FC
  int py; // BP/FA
  int src_px; // BP/F8
  int src_py; // BP/F6
  HDC dc; // BP/F4

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
          if (random(3) == 1) {
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

// CS:5961 - ***CODE MATCH!***
void animate_doors(void) {
  // stack: 0x08 bytes (/F8)
  // BP/F8
  int cx; // BP/FE
  int cy; // DI
  int src_py; // BP/FC
  HDC dc; // BP/FA

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
        if (g_level_tiles[cx][cy] == T_DOOR_W_E || g_level_tiles[cx][cy] == T_DOOR_E_W) {
          px = cx * g_tile_lx;
          py = cy * g_tile_ly;
          BitBlt(g_maindc,
            px, py,
            TILE_LX, TILE_LY,
            dc,
            14 * TILE_LX, src_py,
            SRCCOPY);
        } else if (g_level_tiles[cx][cy] == T_DOOR_N_S || g_level_tiles[cx][cy] == T_DOOR_S_N) {
          px = cx * g_tile_lx;
          py = cy * g_tile_ly;
          BitBlt(g_maindc,
            px, py,
            TILE_LX, TILE_LY,
            dc,
            13 * TILE_LX, src_py,
            SRCCOPY);
        }
      }
    }

    DeleteDC(dc);
  }
}

// CS:5A54 - ***CODE MATCH!***
void animate_autospawners(void) {
  int spawn_type; // [bp-0x2].w
  int ai; // [bp-0x4].w
  int cx; // [bp-0x6].w
  int cy; // [bp-0x8].w
  int other; // DX
  int spawn_x; // [bp-0xA].w
  int spawn_y; // [bp-0xC].w
  // ^^ These are real variables, everything else is potentially spillage.
  //int v0E; // [bp-0xE].w - auto ai*sizeof(g_actors[0])
  // SI = auto &g_actors[ai].type
  // DI = auto &g_actors[ai].var0

  if ((g_tick_counter % 7) == 0) {
    for (ai = 0; ai < g_actor_count; ai++) {
      if (g_actors[ai].type >= T_AUTOSLIDER1 && g_actors[ai].type <= T_AUTOSLIDER4) {
        cx = g_actors[ai].cx;
        cy = g_actors[ai].cy;

        g_actors[ai].type = g_actors[ai].type + 1;
        if (g_actors[ai].type > T_AUTOSLIDER4) {
          g_actors[ai].type = T_AUTOSLIDER1;
        }

        move_actor(ai, cx, cy);

        if (g_actors[ai].var0 < cy) {
          g_actors[ai].var0 = g_actors[ai].var0 + 1;

        } else {
          switch (g_actors[ai].type) {
            case T_AUTOSLIDER1:
              spawn_type = T_SLIDER_E;
              spawn_x = cx + 1;
              spawn_y = cy;
              break;

            case T_AUTOSLIDER2:
              spawn_type = T_SLIDER_N;
              spawn_x = cx;
              spawn_y = cy - 1;
              break;

            case T_AUTOSLIDER3:
              spawn_type = T_SLIDER_W;
              spawn_x = cx - 1;
              spawn_y = cy;
              break;

            case T_AUTOSLIDER4:
              spawn_type = T_SLIDER_S;
              spawn_x = cx;
              spawn_y = cy + 1;
              break;
          }
          if (g_level_tiles[spawn_x][spawn_y] == T_EMPTY) {
            g_actors[ai].var0 = 0;
            other = create_actor(spawn_type, spawn_x, spawn_y);
            move_actor(other, spawn_x, spawn_y);
          }
        }

      } else if (g_actors[ai].type >= T_AUTOROCKY1 && g_actors[ai].type <= T_AUTOROCKY4) {
        cx = g_actors[ai].cx;
        cy = g_actors[ai].cy;

        g_actors[ai].type = g_actors[ai].type + 1;
        if (g_actors[ai].type > T_AUTOROCKY4) {
          g_actors[ai].type = T_AUTOROCKY1;
        }

        move_actor(ai, cx, cy);

        if (g_actors[ai].var0 < cy) {
          g_actors[ai].var0 = g_actors[ai].var0 + 1;

        } else {
          switch (g_actors[ai].type) {
            case T_AUTOROCKY1:
              spawn_type = T_ROCKY_E;
              spawn_x = cx + 1;
              spawn_y = cy;
              break;

            case T_AUTOROCKY2:
              spawn_type = T_ROCKY_N;
              spawn_x = cx;
              spawn_y = cy - 1;
              break;

            case T_AUTOROCKY3:
              spawn_type = T_ROCKY_W;
              spawn_x = cx - 1;
              spawn_y = cy;
              break;

            case T_AUTOROCKY4:
              spawn_type = T_ROCKY_S;
              spawn_x = cx;
              spawn_y = cy + 1;
              break;
          }

          if (g_level_tiles[spawn_x][spawn_y] == T_EMPTY) {
            g_actors[ai].var0 = 0;
            other = create_actor(spawn_type, spawn_x, spawn_y);
            move_actor(other, spawn_x, spawn_y);
          }
        }
      }
    }
  }

  garbage_collect_actors();
}

// CS:5C8A - ***CODE MATCH!*** (not sure how though, because it sure feels wrong!)
void animate_blackies(void) {
  // stack: 0x08 bytes (/F8)
  int i; // BP/FE
  int py; // BP/FC
  HDC dc; // BP/FA
  // BP/F8

  register int px; // CX
  register int src_py; // DX

  // DI = actor table offset
  // BP/F8 = actor ptr
  // SI = actor->var0 ptr

  if (g_tick_counter % 5 == 0) {
    dc = CreateCompatibleDC(g_maindc);
    SelectObject(dc, g_hbmpBlock);
    for (i = 0; i < g_actor_count; i++) {
      if (g_actors[i].type == T_BLACKY1) {
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
      } else if (g_actors[i].type == T_BLACKY2) {
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
      }
    }
    DeleteDC(dc);
  }
}

// CS:5D9C - ***CODE MATCH!***
void animate_monsters(void) {
  // needs 0x8 bytes of stack (/F8)
  int i; // BP/FE
  int px; // CX
  int py; // BP/FC
  HDC dc; // BP/FA
  //int actor_type; // BP/F8

  register int src_px; // DX
  register int src_py; // BX
  // SI = i*sizeof(g_actors[i]);
  // DI = running counter for &g_actors[i].var0;

  if (g_tick_counter % 3 == 0) {
    dc = CreateCompatibleDC(g_maindc);
    SelectObject(dc, g_hbmpBlock);
    for (i = 0; i < g_actor_count; i++) {
      switch (g_actors[i].type) {
      case T_B_TWISTER:
      case T_B_GNASHER:
      case T_B_BLOB:
      case T_B_VIRUS:
      case T_B_SPIKE:
        px = g_actors[i].cx * g_tile_lx;
        py = g_actors[i].cy * g_tile_ly;
        src_px = g_actors[i].type * TILE_LX;
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

// CS:5E61 - ***CODE MATCH!***
void animate_everything(void) {
  animate_monsters();
  animate_diamonds();
  animate_doors();
  animate_autospawners();
  animate_blackies();
}
