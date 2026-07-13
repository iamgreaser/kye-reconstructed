// vim: set syntax=cpp :
#include <windows.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kyetypes.h"
#include "kyeprocs.h"
#include "kyevars.h"

// Word alignment mandates that this starts the module.

// CS:3006 - ***CODE MATCH!***
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
  g_level_tiles[g_actors[ai].cx][g_actors[ai].cy] = ai;
  draw_actor(g_maindc, dc, ai);
  DeleteDC(dc);
}

// CS:3084 - ***CODE MATCH!***
int apply_sticky_blocks(int ai) {
  // need 0x4 bytes of stack (BP/FC)
  int cx; // BP/FE
  int result; // BP/FC

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

// CS:31AC - ***CODE MATCH!***
int actor_held_by_sticky_block(int ai) {
  // need 0x2 bytes of stack
  int cx; // BP/FE

  int cy; // SI
  int result; // CX - this is a variable and there is one "return" statement at the end.
  int tmp0; // DX

  result = 0;

  cx = g_actors[ai].cx;
  cy = g_actors[ai].cy;

  tmp0 = g_level_tiles[cx][cy+1];
  if (tmp0 >= 0 && g_actors[tmp0].type == T_STICKY_V) {
    result = 1;
  } else {
    tmp0 = g_level_tiles[cx][cy-1];
    if (tmp0 >= 0 && g_actors[tmp0].type == T_STICKY_V) {
      result = 1;
    } else {
      tmp0 = g_level_tiles[cx+1][cy];
      if (tmp0 >= 0 && g_actors[tmp0].type == T_STICKY_H) {
        result = 1;
      } else {
        tmp0 = g_level_tiles[cx-1][cy];
        if (tmp0 >= 0 && g_actors[tmp0].type == T_STICKY_H) {
          result = 1;
        }
      }
    }
  }

  return result;
}

// CS:3251 - ***CODE MATCH!***
// This function calculates the movement delta to use when doing mouse movement. I think.
void cs_3251(int ai, int targx, int targy, int *newxptr, int *newyptr) {
  int cx; // [bp-0x2].w
  int cy; // DI
  int dx; // CX
  int dy; // SI

  cx = g_actors[ai].cx;
  cy = g_actors[ai].cy;

  *newxptr = cx;
  *newyptr = cy;

  if ((targx - cx) > 0) {
    dx = 1;
  } else if ((targx - cx) < 0) {
    dx = -1;
  } else {
    dx = 0;
  }

  if ((targy - cy) > 0) {
    dy = 1;
  } else if ((targy - cy) < 0) {
    dy = -1;
  } else {
    dy = 0;
  }

  if (abs(dx) > 0 && abs(dy) > 0) {
    if (g_level_tiles[cx+dx][cy] == T_EMPTY && g_level_tiles[cx][cy+dy] == T_EMPTY) {
      if (abs(dx) > abs(dy)) {
        dy = 0;
      } else {
        dx = 0;
      }
    } else {
      if (g_level_tiles[cx+dx][cy] == T_EMPTY) {
        dy = 0;
      } else if (g_level_tiles[cx][cy+dy] == T_EMPTY) {
        dx = 0;
      } else {
        // Force one component of (dx,dy) to be 0 so it can be blocked by the check at the end.
        dx = 0;
      }
    }
  }

  if (g_level_tiles[cx+dx][cy+dy] == T_EMPTY) {
    *newxptr = cx+dx;
    *newyptr = cy+dy;
  }
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
