// vim: set syntax=cpp :
#include <windows.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kyetypes.h"
#include "kyeprocs.h"
#include "kyevars.h"

// Data segment and the fact that this isn't the editor tool window code hints at a split around about here.

// CS:5128 - ***CODE MATCH!***
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

// CS:5165 - ***CODE MATCH!***
void draw_kye_mouse_ghost(void) {
  // stack: 0x04 bytes
  int px; // BP/FE
  int py; // BP/FC

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

// CS:51B9 - ***CODE MATCH!***
void clear_kye_mouse_ghost(void) {
  #if APPLY_BUGFIXES
    int had_maindc = g_has_maindc;
    if (had_maindc == 0) {
      acquire_main_dc();
    }
  #endif
  if (g_drawing_kye_mouse_ghost != 0 && g_level_tiles[g_kye_mouse_cx][g_kye_mouse_cy] == T_EMPTY) {
    delete_and_draw_unacting_tile(g_kye_mouse_cx, g_kye_mouse_cy);
  }
  g_drawing_kye_mouse_ghost = 0;
  #if APPLY_BUGFIXES
    if (had_maindc == 0) {
      release_main_dc();
    }
  #endif
}

// CS:51EE - ***CODE MATCH!***
void update_kye_mouse_target(int cx, int cy) {
  // SI = cx
  // DI = cy
  #if APPLY_BUGFIXES
    int had_maindc = g_has_maindc;
    if (had_maindc == 0) {
      acquire_main_dc();
    }
  #endif
  if (g_drawing_kye_mouse_ghost == 0 || g_kye_mouse_cx != cx || g_kye_mouse_cy != cy) {
    clear_kye_mouse_ghost();
    g_kye_mouse_cx = cx;
    g_kye_mouse_cy = cy;
    if (abs(cx - g_kye_main_cx) > 1 || abs(cy - g_kye_main_cy) > 1) {
      if (g_level_tiles[g_kye_mouse_cx][g_kye_mouse_cy] == T_EMPTY) {
        draw_kye_mouse_ghost();
        g_drawing_kye_mouse_ghost = 1;
      }
    }
  }
  #if APPLY_BUGFIXES
    if (had_maindc == 0) {
      release_main_dc();
    }
  #endif
}

// CS:525D - ***CODE MATCH!***
void repaint_kye(int fizzle_mode) {
  int px; // BP/FE
  int py; // BP/FC

  int i; // SI, first
  HDC dc; // SI, second

  if (fizzle_mode == 1) {
    for (i = TILE_LX*TILE_LY; i >= 0; i += -TILE_LY) {
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

// CS:52EA - ***CODE MATCH!***
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
  g_kye_main_cx = g_kye_main_cx + dx;
  g_kye_main_cy = g_kye_main_cy + dy;
  g_tile_under_kye = g_level_tiles[g_kye_main_cx][g_kye_main_cy];
  g_level_tiles[g_kye_main_cx][g_kye_main_cy] = T_KYE;
  repaint_kye(0);
}

// CS:5395 - ***CODE MATCH!***
void try_move_kye(int indx, int indy) {
  // Stack: 0x2E bytes (BP/D2 start)
  int i; // BP/FE
  int ai; // BP/FC
  int other; // BP/FA
  int blocked; // BP/F8
  int pushdiagblocked; // BP/F6
  int newx; // BP/F4
  int newy; // CX
  int pushx; // BP/F2
  int pushy; // BP/F0
  int didmove; // BP/EE
  int foundmousedir; // BP/EC
  int distsqr; // BP/EA
  int circle_x[4] = {1,0,-1,0}; // BP/E2 const from DS:0D5A
  int circle_y[4] = {0,1,0,-1}; // BP/DA const from DS:0D62
  //int v28; // BP/D8 - pointer premultiplier
  // BP/D6 - pointer premultiplier
  //int dx2; // BP/D4
  //int dy2; // BP/D2
  // BP+0x00
  int dx; // DI
  int dy; // SI
  int newdistsqr; // BX - THIS WAS THE LAST MISMATCH IN THE CODE *EVER*.

  // DX = dx;
  // BX = dy;
  didmove = 0;
  blocked = 0;
  pushdiagblocked = 0;
  // DI = dx;
  // SI = dy;
  dx = indx;
  dy = indy;
  newx = g_kye_main_cx + dx;
  newy = g_kye_main_cy + dy;

  // Check if diagonal is valid
  if ((dx != 0) & (dy != 0)) {
    blocked = !(g_level_tiles[newx][g_kye_main_cy] == T_EMPTY
             && g_level_tiles[g_kye_main_cx][newy] == T_EMPTY);
    if (blocked) {
      if (g_moving_by_mouse != 0) {
        if (g_level_tiles[newx][g_kye_main_cy] == T_EMPTY) {
          dy = 0;
          newy = g_kye_main_cy;
          blocked = 0;
        } else if (g_level_tiles[g_kye_main_cx][newy] == T_EMPTY) {
          dx = 0;
          newx = g_kye_main_cx;
          blocked = 0;
        }
      }
    }
  }

  if (!blocked) {
    if (g_level_tiles[newx][newy] == T_EMPTY) {
      move_kye(dx, dy);
      didmove = 1;
    } else if (g_level_tiles[newx][newy] == T_EARTH) {
      move_kye(dx, dy);
      g_tile_under_kye = T_EMPTY;
      didmove = 1;
    } else if (g_level_tiles[newx][newy] == T_DIAMOND) {
      move_kye(dx, dy);
      g_tile_under_kye = T_EMPTY;
      didmove = 1;
      count_all_diamonds();
      repaint_status_left();
    } else if (g_level_tiles[newx][newy] == T_DOOR_W_E) {
      // These don't set didmove.
      // There may be a reason for it...
      if (dy == 0 && dx == 1) {
        move_kye(dx, dy);
      }
    } else if (g_level_tiles[newx][newy] == T_DOOR_E_W) {
      if (dy == 0 && dx == -1) {
        move_kye(dx, dy);
      }
    } else if (g_level_tiles[newx][newy] == T_DOOR_N_S) {
      if (dy == 1 && dx == 0) {
        move_kye(dx, dy);
      }
    } else if (g_level_tiles[newx][newy] == T_DOOR_S_N) {
      if (dy == -1 && dx == 0) {
        move_kye(dx, dy);
      }

    } else {
      // Then we get into actor handling...
      if (g_level_tiles[newx][newy] >= 0) {
        ai = g_level_tiles[newx][newy];
        if (g_actors[ai].type == T_BLACKY1) {
          g_kye_lives = g_kye_lives - 1;
          respawn_kye();
          repaint_status_left();
          return;
        } else if (g_actors[ai].type == T_BLACKY2) {
          return;
        }
        pushx = g_actors[ai].cx + dx;
        pushy = g_actors[ai].cy + dy;
        if ((dx != 0) & (dy != 0)) {
          pushdiagblocked = !(g_level_tiles[pushx][g_actors[ai].cy] == T_EMPTY
                           && g_level_tiles[g_actors[ai].cx][pushy] == T_EMPTY);
        }

        if (g_level_tiles[pushx][pushy] == T_EMPTY) {
          if (!pushdiagblocked) {
            move_actor(ai, pushx, pushy);
            move_kye(dx, dy);
            didmove = 1;
          }
        } else {
          if (g_level_tiles[pushx][pushy] >= 0) {
            other = g_level_tiles[pushx][pushy];
            if (g_actors[other].type == T_BLACKY1 && !pushdiagblocked) {
              // POTENTIAL BUG: If try_enter_blacky fails then this breaks.
              // An actor will clobber an actor, or Kye will clobber an actor.
              if (!try_enter_blacky(ai, pushx, pushy)) {
                move_actor(ai, pushx, pushy);
              }
              move_kye(dx, dy);
              didmove = 1;
            }
          }
        }
      }
    }
  }

  if (g_moving_by_mouse != 0 && !didmove) {
    #define dx2 (g_kye_target_cx - g_kye_main_cx)
    #define dy2 (g_kye_target_cy - g_kye_main_cy)
    distsqr = (dx2*dx2)+(dy2*dy2);
    #undef dx2;
    #undef dy2;

    foundmousedir = 0;
    for (i = 0; (!foundmousedir) && i < 4; i = i + 1) {
      dx = circle_x[i];
      dy = circle_y[i];
      newx = g_kye_main_cx + dx;
      newy = g_kye_main_cy + dy;
      if (g_level_tiles[newx][newy] == T_EMPTY) {
        newdistsqr = ((g_kye_target_cx-newx)*(g_kye_target_cx-newx)) + ((g_kye_target_cy-newy)*(g_kye_target_cy-newy));
        if (newdistsqr < distsqr) {
            foundmousedir = 1;
        }
      }
    }

    if (foundmousedir != 0) {
      move_kye(dx, dy);
    }
  }
}

// CS:57F5 - ***CODE MATCH!***
void tick_kye(void) {
  int fulldx; // DX
  int fulldy; // BX
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

    fulldx = (g_kye_target_cx - g_kye_main_cx);
    fulldy = (g_kye_target_cy - g_kye_main_cy);
    if (fulldx < 0) {
      dx = -1;
    } else if (fulldx > 0) {
      dx = 1;
    } else {
      dx = 0;
    }
    if (fulldy < 0) {
      dy = -1;
    } else if (fulldy > 0) {
      dy = 1;
    } else {
      dy = 0;
    }
    try_move_kye(dx, dy);
  }
}
