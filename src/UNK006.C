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
// This function calculates the movement delta to use when doing mouse movement. I think. OK, it's used for monsters at least.
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

// CS:337F - ***CODE MATCH!***
void respawn_kye(void) {
  // Stack: 0x0A bytes
  int radius; // BP/FE
  int cx0; // BP/FC
  int cy0; // BP/FA
  int cx1; // BP/F8
  int cy1; // BP/F6

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
        radius < 5; cx0--, cy0++, rad2 = rad2 + 2, cx1++, cy1--, radius++) {

      g_kye_target_cx = cx0;
      g_kye_target_cy = cy0;
      for (i = 0; rad2 > i; i++) {
        if (g_level_tiles[g_kye_target_cx][g_kye_target_cy] == T_EMPTY) {
          return;
        }
        g_kye_target_cx = g_kye_target_cx + 1;
      }
      g_kye_target_cx = cx1;
      g_kye_target_cy = cy0;
      for (i = 0; rad2 > i; i++) {
        if (g_level_tiles[g_kye_target_cx][g_kye_target_cy] == T_EMPTY) {
          return;
        }
        g_kye_target_cy = g_kye_target_cy - 1;
      }
      g_kye_target_cx = cx1;
      g_kye_target_cy = cy1;
      for (i = 0; rad2 > i; i++) {
        if (g_level_tiles[g_kye_target_cx][g_kye_target_cy] == T_EMPTY) {
          return;
        }
        g_kye_target_cx = g_kye_target_cx - 1;
      }
      g_kye_target_cx = cx0;
      g_kye_target_cy = cy1;
      for (i = 0; rad2 > i; i++) {
        if (g_level_tiles[g_kye_target_cx][g_kye_target_cy] == T_EMPTY) {
          return;
        }
        g_kye_target_cy = g_kye_target_cy + 1;
      }
    }

    // Respawn in-place
    g_kye_target_cx = g_kye_main_cx;
    g_kye_target_cy = g_kye_main_cy;
  }
}

// CS:34ED - ***CODE MATCH!***
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
    g_kye_lives = g_kye_lives - 1;
    respawn_kye();
    repaint_status_left();
  }
  return result;
}

// CS:3562 - ***CODE MATCH!***
void tick_all_timer_blocks(void) {
  int ai; // SI
  int cx; // DX
  int cy; // CX

  if ((g_tick_counter % 30) == 0) {
    ai = 0;
    while (ai < g_actor_count) {
      if (g_actors[ai].type >= T_TIMER_0 && g_actors[ai].type <= T_TIMER_9) {
        cx = g_actors[ai].cx;
        cy = g_actors[ai].cy;

        if (g_actors[ai].type > T_TIMER_0) {
          g_actors[ai].type = g_actors[ai].type - 1;
          move_actor(ai, cx, cy);
          ai++;
        } else {
          delete_and_draw_unacting_tile(cx, cy);
          mark_actor_for_deletion(ai);
        }
      } else {
        ai++;
      }
    }
  }

  garbage_collect_actors();
}

// CS:35EA - ***CODE MATCH!***
int try_enter_blacky(int ai, int cx, int cy) {
  // stack: 0x06 bytes
  // BP/FA = tile row ptr
  int src_cx; // BP/FE
  int src_cy; // BP/FC

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

// CS:3684 - TODO: Doesn't quite sync, likely needs work on variable placement. --GM
void tick_game_state(void) {
  // stack: 0x12 bytes
  register int ai; // SI
  int cx; // [BP-0x2]
  int cy; // [BP-0x4]

  // BUG: Counter wraparound is not seamless.
  // We cover 32001 different values.
  // If left alone, this takes less than an hour.
  // A counter wraparound breaks DEFAULT.KYE's COUNTERS level,
  // as the bouncer at the bottom steps ahead a bit too early.
  g_tick_counter = g_tick_counter + 1;
  if (g_tick_counter > 32000) {
    g_tick_counter = 0;
  }

  tick_all_timer_blocks();
  for (ai = 0; ai < g_actor_count; ai++) {
    //
    cx = g_actors[ai].cx;
    cy = g_actors[ai].cy;
    switch (g_actors[ai].type) {
    // TODO: Find how these are split so the switch conditions can be correct, because there are two identical code branches here! --GM
    case T_SBLOCK:
    case T_ROT_A:
    case T_ROT_C:
    case T_RBLOCK:
    case T_AUTOSLIDER1:
    case T_AUTOSLIDER2:
    case T_AUTOSLIDER3:
    case T_AUTOSLIDER4:
      if (apply_sticky_blocks(ai)) {
        // Do nothing
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
      if (apply_sticky_blocks(ai)) {
        // Do nothing
      }
      break;

    // BUG: North-facing sliders do not redraw on rotation.
    case T_SLIDER_N:
      if ((!apply_sticky_blocks(ai)) && (!actor_held_by_sticky_block(ai))) {
        register int other = g_level_tiles[cx][cy-1];
        if (other == T_EMPTY) {
          move_actor(ai, cx, cy-1);
        } else {
          if (!try_enter_blacky(ai, cx, cy-1)) {
            if (other >= 0 && g_actors[other].type == T_ROT_A) {
              g_actors[ai].type = T_SLIDER_E;
              // BUG: This needs to redraw the actor!
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              g_actors[ai].type = T_SLIDER_W;
              // BUG: This needs to redraw the actor!
            }
          }
        }
      }
      break;

    case T_SLIDER_S:
      if ((!apply_sticky_blocks(ai)) && (!actor_held_by_sticky_block(ai))) {
        int other = g_level_tiles[cx][cy+1];
        if (other == T_EMPTY) {
          move_actor(ai, cx, cy+1);
        } else {
          if (!try_enter_blacky(ai, cx, cy+1)) {
            if (other >= 0 && g_actors[other].type == T_ROT_A) {
              g_actors[ai].type = T_SLIDER_W;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              g_actors[ai].type = T_SLIDER_E;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_SLIDER_W:
      if ((!apply_sticky_blocks(ai)) && (!actor_held_by_sticky_block(ai))) {
        int other = g_level_tiles[cx-1][cy];
        if (other == T_EMPTY) {
          move_actor(ai, cx-1, cy);
        } else {
          if (!try_enter_blacky(ai, cx-1, cy)) {
            if (other >= 0 && g_actors[other].type == T_ROT_A) {
              g_actors[ai].type = T_SLIDER_N;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              g_actors[ai].type = T_SLIDER_S;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_SLIDER_E:
      if ((!apply_sticky_blocks(ai)) && (!actor_held_by_sticky_block(ai))) {
        int other = g_level_tiles[cx+1][cy];
        if (other == T_EMPTY) {
          move_actor(ai, cx+1, cy);
        } else {
          if (!try_enter_blacky(ai, cx+1, cy)) {
            if (other >= 0 && g_actors[other].type == T_ROT_A) {
              g_actors[ai].type = T_SLIDER_S;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              g_actors[ai].type = T_SLIDER_N;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_ROCKY_N:
      if (!apply_sticky_blocks(ai)) {
        if (actor_held_by_sticky_block(ai)) {
          // Do nothing
        } else {
          int fw = g_level_tiles[cx][cy-1];
          if (fw == T_EMPTY) {
            move_actor(ai, cx, cy-1);
          } else if (try_enter_blacky(ai, cx, cy-1)) {
            // Do nothing
          } else {
            // CX = row offset
            int slide_right = g_level_tiles[cx+1][cy];
            register int slide_left = g_level_tiles[cx-1][cy];
            int slide_fw_right = g_level_tiles[cx+1][cy-1];
            int slide_fw_left = g_level_tiles[cx-1][cy-1];
            int hitting_rocky = (fw >= 0 && g_actors[fw].type >= T_ROCKY_N && g_actors[fw].type <= T_ROCKY_E);
            int hitting_rblock = (fw >= 0 && g_actors[fw].type == T_RBLOCK);
            register int can_slide_left = (slide_left == T_EMPTY && slide_fw_left == T_EMPTY && (fw == T_WALL1 || fw == T_WALL2 || fw == T_WALL4 || hitting_rocky || hitting_rblock));
            register int can_slide_right = (slide_right == T_EMPTY && slide_fw_right == T_EMPTY && (fw == T_WALL3 || fw == T_WALL2 || fw == T_WALL6 || hitting_rocky || hitting_rblock));
            if (can_slide_right && can_slide_left) {
              move_actor(ai, cx+1-(random(2)*2), cy-1);
            } else if (can_slide_right) {
              move_actor(ai, cx+1, cy-1);
            } else if (can_slide_left) {
              move_actor(ai, cx-1, cy-1);
            } else if (fw >= 0 && g_actors[fw].type == T_ROT_A) {
              g_actors[ai].type = T_ROCKY_E;
              move_actor(ai, cx, cy);
            } else if (fw >= 0 && g_actors[fw].type == T_ROT_C) {
              g_actors[ai].type = T_ROCKY_W;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_ROCKY_S:
      if (!apply_sticky_blocks(ai)) {
        if (actor_held_by_sticky_block(ai)) {
          // Do nothing
        } else {
          int fw = g_level_tiles[cx][cy+1];
          if (fw == T_EMPTY) {
            move_actor(ai, cx, cy+1);
          } else if (try_enter_blacky(ai, cx, cy+1)) {
            // Do nothing
          } else {
            // CX = row offset
            int slide_right = g_level_tiles[cx-1][cy];
            register int slide_left = g_level_tiles[cx+1][cy];
            int slide_fw_right = g_level_tiles[cx-1][cy+1];
            int slide_fw_left = g_level_tiles[cx+1][cy+1];
            int hitting_rocky = (fw >= 0 && g_actors[fw].type >= T_ROCKY_N && g_actors[fw].type <= T_ROCKY_E);
            int hitting_rblock = (fw >= 0 && g_actors[fw].type == T_RBLOCK);
            register int can_slide_left = (slide_left == T_EMPTY && slide_fw_left == T_EMPTY && (fw == T_WALL9 || fw == T_WALL8 || fw == T_WALL6 || hitting_rocky || hitting_rblock));
            register int can_slide_right = (slide_right == T_EMPTY && slide_fw_right == T_EMPTY && (fw == T_WALL7 || fw == T_WALL8 || fw == T_WALL4 || hitting_rocky || hitting_rblock));
            if (can_slide_right && can_slide_left) {
              move_actor(ai, cx+1-(random(2)*2), cy+1);
            } else if (can_slide_right) {
              move_actor(ai, cx-1, cy+1);
            } else if (can_slide_left) {
              move_actor(ai, cx+1, cy+1);
            } else if (fw >= 0 && g_actors[fw].type == T_ROT_A) {
              g_actors[ai].type = T_ROCKY_W;
              move_actor(ai, cx, cy);
            } else if (fw >= 0 && g_actors[fw].type == T_ROT_C) {
              g_actors[ai].type = T_ROCKY_E;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_ROCKY_W:
      if (!apply_sticky_blocks(ai)) {
        if (actor_held_by_sticky_block(ai)) {
          // Do nothing
        } else {
          int fw = g_level_tiles[cx-1][cy];
          if (fw == T_EMPTY) {
            move_actor(ai, cx-1, cy);
          } else if (try_enter_blacky(ai, cx-1, cy)) {
            // Do nothing
          } else {
            // CX = row offset
            int slide_right = g_level_tiles[cx][cy-1];
            register int slide_left = g_level_tiles[cx][cy+1];
            int slide_fw_right = g_level_tiles[cx-1][cy-1];
            int slide_fw_left = g_level_tiles[cx-1][cy+1];
            int hitting_rocky = (fw >= 0 && g_actors[fw].type >= T_ROCKY_N && g_actors[fw].type <= T_ROCKY_E);
            int hitting_rblock = (fw >= 0 && g_actors[fw].type == T_RBLOCK);
            register int can_slide_left = (slide_left == T_EMPTY && slide_fw_left == T_EMPTY && (fw == T_WALL3 || fw == T_WALL6 || fw == T_WALL2 || hitting_rocky || hitting_rblock));
            register int can_slide_right = (slide_right == T_EMPTY && slide_fw_right == T_EMPTY && (fw == T_WALL9 || fw == T_WALL6 || fw == T_WALL8 || hitting_rocky || hitting_rblock));
            if (can_slide_right && can_slide_left) {
              move_actor(ai, cx-1, cy+1-(random(2)*2));
            } else if (can_slide_right) {
              move_actor(ai, cx-1, cy-1);
            } else if (can_slide_left) {
              move_actor(ai, cx-1, cy+1);
            } else if (fw >= 0 && g_actors[fw].type == T_ROT_A) {
              g_actors[ai].type = T_ROCKY_N;
              move_actor(ai, cx, cy);
            } else if (fw >= 0 && g_actors[fw].type == T_ROT_C) {
              g_actors[ai].type = T_ROCKY_S;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;

    case T_ROCKY_E:
      if (!apply_sticky_blocks(ai)) {
        if (actor_held_by_sticky_block(ai)) {
          // Do nothing
        } else {
          int fw = g_level_tiles[cx+1][cy];
          if (fw == T_EMPTY) {
            move_actor(ai, cx+1, cy);
          } else if (try_enter_blacky(ai, cx+1, cy)) {
            // Do nothing
          } else {
            // CX = row offset
            int slide_right = g_level_tiles[cx][cy+1];
            register int slide_left = g_level_tiles[cx][cy-1];
            int slide_fw_right = g_level_tiles[cx+1][cy+1];
            int slide_fw_left = g_level_tiles[cx+1][cy-1];
            int hitting_rocky = (fw >= 0 && g_actors[fw].type >= T_ROCKY_N && g_actors[fw].type <= T_ROCKY_E);
            int hitting_rblock = (fw >= 0 && g_actors[fw].type == T_RBLOCK);
            register int can_slide_left = (slide_left == T_EMPTY && slide_fw_left == T_EMPTY && (fw == T_WALL7 || fw == T_WALL4 || fw == T_WALL8 || hitting_rocky || hitting_rblock));
            register int can_slide_right = (slide_right == T_EMPTY && slide_fw_right == T_EMPTY && (fw == T_WALL1 || fw == T_WALL4 || fw == T_WALL2 || hitting_rocky || hitting_rblock));
            if (can_slide_right && can_slide_left) {
              move_actor(ai, cx+1, cy+1-(random(2)*2));
            } else if (can_slide_right) {
              move_actor(ai, cx+1, cy+1);
            } else if (can_slide_left) {
              move_actor(ai, cx+1, cy-1);
            } else if (fw >= 0 && g_actors[fw].type == T_ROT_A) {
              g_actors[ai].type = T_ROCKY_S;
              move_actor(ai, cx, cy);
            } else if (fw >= 0 && g_actors[fw].type == T_ROT_C) {
              g_actors[ai].type = T_ROCKY_N;
              move_actor(ai, cx, cy);
            }
          }
        }
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
          g_actors[ai].type = T_BOUNCER_S;
          move_actor(ai, cx, cy);
          if (g_level_tiles[cx][cy-1] >= 0) {
            int other = g_level_tiles[cx][cy-1];
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
          g_actors[ai].type = T_BOUNCER_N;
          move_actor(ai, cx, cy);
          if (g_level_tiles[cx][cy+1] >= 0) {
            int other = g_level_tiles[cx][cy+1];
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
          g_actors[ai].type = T_BOUNCER_E;
          move_actor(ai, cx, cy);
          if (g_level_tiles[cx-1][cy] >= 0) {
            int other = g_level_tiles[cx-1][cy];
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
          g_actors[ai].type = T_BOUNCER_W;
          move_actor(ai, cx, cy);
          if (g_level_tiles[cx+1][cy] >= 0) {
            int other = g_level_tiles[cx+1][cy];
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

    case T_STICKY_V:
      if (g_level_tiles[cx][cy+1] == T_EMPTY
          && g_level_tiles[cx][cy+2] == T_KYE) {
        move_actor(ai, cx, cy+1);
      } else if (g_level_tiles[cx][cy-1] == T_EMPTY
          && g_level_tiles[cx][cy-2] == T_KYE) {
        move_actor(ai, cx, cy-1);
      } else if (g_level_tiles[cx+1][cy] == T_EMPTY
          && g_level_tiles[cx+2][cy] >= 0
          && g_actors[g_level_tiles[cx+2][cy]].type == T_STICKY_H) {
        move_actor(ai, cx+1, cy);
      } else if (g_level_tiles[cx-1][cy] == T_EMPTY
          && g_level_tiles[cx-2][cy] >= 0
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
          && g_level_tiles[cx][cy+2] >= 0
          && g_actors[g_level_tiles[cx][cy+2]].type == T_STICKY_V) {
        move_actor(ai, cx, cy+1);
      } else if (g_level_tiles[cx][cy-1] == T_EMPTY
          && g_level_tiles[cx][cy-2] >= 0
          && g_actors[g_level_tiles[cx][cy-2]].type == T_STICKY_V) {
        move_actor(ai, cx, cy-1);
      }
      break;

    case T_B_TWISTER:
    case T_B_GNASHER:
    case T_B_BLOB:
    case T_B_VIRUS:
    case T_B_SPIKE:
      // BUG: If a monster kills kye on its tick, it loses that tick
      if ((!try_killing_adjacent_kye(ai)) && g_tick_counter % 3 == 0) {
        register int dx = 0; // DI
        int dy = 0; // [BP-0x6]

        if (!apply_sticky_blocks(ai)) {
          if (!actor_held_by_sticky_block(ai)) {
            register int newx; // CX
            int newy; // [BP-0x12]
            int fw;

            // Compute direction
            if (random(2) == 1) {
              cs_3251(ai, g_kye_main_cx, g_kye_main_cy, &cx, &cy);
            } else {
              if (random(2) == 1) {
                dx = random(3)-1;
              } else {
                dy = random(3)-1;
              }
            }

            // Apply direction
            newx = cx + dx; // CX
            newy = cy + dy; // [BP-0x12]

            // TODO: Work out how to get this into sync WAIT THIS OLD COMPILER STILL LETS ME DEFINE VARIABLES MID-BLOCK?!?! --GM
            // UPDATE: No, that's a C++ special feature. In C, you can't.
            fw = g_level_tiles[newx][newy];
            if (fw == T_EMPTY || (0 <= fw && g_actors[fw].type == T_BLACKY1)) {
              cx = newx;
              cy = newy;
              cx++;cx++;cx-=2; // FIXME RESYNCING HACK --GM
            }
          }

          if (!try_enter_blacky(ai, cx, cy)) {
            move_actor(ai, cx, cy);
          }
        }
      }
      break;
    }
  }
  garbage_collect_actors();
}
// ends at CS:49F2 after 1 byte of padding
