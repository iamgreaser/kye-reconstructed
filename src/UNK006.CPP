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

// CS:35EA
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

// CS:3684
void tick_game_state(void) {
  // stack: 0x12 bytes
  // BP-0x00
  int cx; // BP/FE
  int cy; // BP/FC
  register int dx; // DI
  int dy; // BP/FA
  // TODO: Work out how this actually goes --GM
  register int slide_left; // DX
  int slide_right; // BP/F8
  int slide_fw_right; // BP/F6
  int slide_fw_left; // BP/F4
  //int unkFFF2; // BP/F2 - automatically created
  //actor_s *actor; // BP/F0 - automatically created
  int newy; // BP/EE

  register int ai; // SI
  //register int other; // DI - other tile type
  #define other dx
  register int newx; // CX - might be automatic? TODO!
  register int reg_bx; // BX - might be automatic? TODO!
  register int reg_BX2; // BX - might be automatic? TODO!
  register int reg_DX2; // DX - might be automatic? TODO!


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
  for (ai = 0;
      ai < g_actor_count;
      ai++) {
    //
    cx = g_actors[ai].cx;
    cy = g_actors[ai].cy;
    switch (g_actors[ai].type) {
    // TODO: Find how these are split so the switch conditions can be correct, because there are two identical code branches here! --GM
    case T_SBLOCK:
    case T_ROT_A:
    case T_ROT_C:
    case T_RBLOCK:
      if (apply_sticky_blocks(ai)) {
        // Do nothing
      }
      break;
    case T_AUTOSLIDER1:
    case T_AUTOSLIDER2:
    case T_AUTOSLIDER3:
    case T_AUTOSLIDER4:
      if (apply_sticky_blocks(ai)) {
        // Do nothing
      }
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
      if ((!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        other = g_level_tiles[cx][cy+1];
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
      if ((!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        other = g_level_tiles[cx-1][cy];
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
      if ((!apply_sticky_blocks(ai))
          && (!actor_held_by_sticky_block(ai))) {
        other = g_level_tiles[cx+1][cy];
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
      // This is *pretty close*, but TODO: Match this! --GM
      if (!apply_sticky_blocks(ai)) {
        if (actor_held_by_sticky_block(ai)) {
          // Do nothing
        } else {
          dx = g_level_tiles[cx][cy-1];
          if (dx == T_EMPTY) {
            move_actor(ai, cx, cy-1);
          } else if (try_enter_blacky(ai, cx, cy-1)) {
            // Do nothing
          } else {
            // CX = row offset
            // 00003A0C  8B46FE            mov ax,[bp-0x2]
            // 00003A0F  BA2800            mov dx,0x28
            // 00003A12  F7EA              imul dx
            // 00003A14  8BC8              mov cx,ax
            // 00003A16  8B56FC            mov dx,[bp-0x4]
            // 00003A19  D1E2              shl dx,1
            // 00003A1B  03C2              add ax,dx
            // 00003A1D  8BD8              mov bx,ax
            // 00003A1F  8B87A612          mov ax,[bx+0x12a6]
            // 00003A23  8946F8            mov [bp-0x8],ax
            slide_right = g_level_tiles[cx+1][cy];
            // 00003A26  8BD9              mov bx,cx
            // 00003A28  8B46FC            mov ax,[bp-0x4]
            // 00003A2B  D1E0              shl ax,1
            // 00003A2D  03D8              add bx,ax
            // 00003A2F  8B975612          mov dx,[bx+0x1256]
            slide_left = g_level_tiles[cx-1][cy];
            // 00003A33  8BD9              mov bx,cx
            // 00003A35  8B46FC            mov ax,[bp-0x4]
            // 00003A38  D1E0              shl ax,1
            // 00003A3A  03D8              add bx,ax
            // 00003A3C  8B87A412          mov ax,[bx+0x12a4]
            // 00003A40  8946F6            mov [bp-0xa],ax
            slide_fw_right = g_level_tiles[cx+1][cy-1];
            // 00003A43  8BD9              mov bx,cx
            // 00003A45  8B46FC            mov ax,[bp-0x4]
            // 00003A48  D1E0              shl ax,1
            // 00003A4A  03D8              add bx,ax
            // 00003A4C  8B875412          mov ax,[bx+0x1254]
            // 00003A50  8946F4            mov [bp-0xc],ax
            slide_fw_left = g_level_tiles[cx-1][cy-1];

            // OK, these two are likely to be preprocessor macros.
            // 00003A53  0BFF              or di,di
            // 00003A55  7C1D              jl 0x3a74
            // 00003A57  8BDF              mov bx,di
            // 00003A59  C1E303            shl bx,byte 0x3
            // 00003A5C  83BF2E170B        cmp word [bx+0x172e],0xb
            // 00003A61  7C11              jl 0x3a74
            // 00003A63  8BDF              mov bx,di
            // 00003A65  C1E303            shl bx,byte 0x3
            // 00003A68  83BF2E170E        cmp word [bx+0x172e],0xe
            // 00003A6D  7F05              jg 0x3a74
            // 00003A6F  B80100            mov ax,0x1
            // 00003A72  EB02              jmp 0x3a76
            // 00003A74  33C0              xor ax,ax
            // 00003A76  8BC8              mov cx,ax
            //#define reg_CX1 (dx >= 0 && g_actors[dx].type >= T_ROCKY_N && g_actors[dx].type <= T_ROCKY_E)
            newx = (dx >= 0 && g_actors[dx].type >= T_ROCKY_N && g_actors[dx].type <= T_ROCKY_E);
            // 00003A78  0BFF              or di,di
            // 00003A7A  7C11              jl 0x3a8d
            // 00003A7C  8BDF              mov bx,di
            // 00003A7E  C1E303            shl bx,byte 0x3
            // 00003A81  83BF2E1716        cmp word [bx+0x172e],0x16
            // 00003A86  7505              jnz 0x3a8d
            // 00003A88  B80100            mov ax,0x1
            // 00003A8B  EB02              jmp 0x3a8f
            // 00003A8D  33C0              xor ax,ax
            // 00003A8F  8BD8              mov bx,ax
            //#define reg_BX1 (dx >= 0 && g_actors[dx].type == T_RBLOCK)
            reg_bx = (dx >= 0 && g_actors[dx].type == T_RBLOCK);

            // 00003A91  83FAFF            cmp dx,0xffffffffffffffff
            // 00003A94  7522              jnz 0x3ab8
            // 00003A96  837EF4FF          cmp word [bp-0xc],0xffffffffffffffff
            // 00003A9A  751C              jnz 0x3ab8
            // 00003A9C  83FFFD            cmp di,0xfffffffffffffffd
            // 00003A9F  7412              jz 0x3ab3
            // 00003AA1  83FFFC            cmp di,0xfffffffffffffffc
            // 00003AA4  740D              jz 0x3ab3
            // 00003AA6  83FFFA            cmp di,0xfffffffffffffffa
            // 00003AA9  7408              jz 0x3ab3
            // 00003AAB  0BC9              or cx,cx
            // 00003AAD  7504              jnz 0x3ab3
            // 00003AAF  0BDB              or bx,bx
            // 00003AB1  7405              jz 0x3ab8
            // 00003AB3  B80100            mov ax,0x1
            // 00003AB6  EB02              jmp 0x3aba
            // 00003AB8  33C0              xor ax,ax
            // 00003ABA  8BD0              mov dx,ax
            reg_DX2 = (slide_left == T_EMPTY && slide_fw_left == T_EMPTY && (dx == T_WALL1 || dx == T_WALL2 || dx == T_WALL4 || newx || reg_bx));

            // 00003ABC  837EF8FF          cmp word [bp-0x8],0xffffffffffffffff
            // 00003AC0  7522              jnz 0x3ae4
            // 00003AC2  837EF6FF          cmp word [bp-0xa],0xffffffffffffffff
            // 00003AC6  751C              jnz 0x3ae4
            // 00003AC8  83FFFB            cmp di,0xfffffffffffffffb
            // 00003ACB  7412              jz 0x3adf
            // 00003ACD  83FFFC            cmp di,0xfffffffffffffffc
            // 00003AD0  740D              jz 0x3adf
            // 00003AD2  83FFF8            cmp di,0xfffffffffffffff8
            // 00003AD5  7408              jz 0x3adf
            // 00003AD7  0BC9              or cx,cx
            // 00003AD9  7504              jnz 0x3adf
            // 00003ADB  0BDB              or bx,bx
            // 00003ADD  7405              jz 0x3ae4
            // 00003ADF  B80100            mov ax,0x1
            // 00003AE2  EB02              jmp 0x3ae6
            // 00003AE4  33C0              xor ax,ax
            // 00003AE6  8BD8              mov bx,ax
            reg_BX2 = (slide_right == T_EMPTY && slide_fw_right == T_EMPTY && (dx == T_WALL3 || dx == T_WALL2 || dx == T_WALL6 || newx || reg_bx));

            // 00003AE8  0BDB              or bx,bx
            // 00003AEA  742E              jz 0x3b1a
            // 00003AEC  0BD2              or dx,dx
            // 00003AEE  742A              jz 0x3b1a
            if (reg_BX2 && reg_DX2) {
              // TODO: Decipher random number usage --GM
              // (cy-1, 0, 0x8000) cwd << 1 into 0x57FE then << 1 ? huh...
              // 00003AF0  8B46FC            mov ax,[bp-0x4]
              // 00003AF3  48                dec ax
              // 00003AF4  50                push ax
              // 00003AF5  6A00              push word 0x0
              // 00003AF7  680080            push word 0x8000
              // 00003AFA  E83427            call 0x6231
              // 00003AFD  99                cwd
              // 00003AFE  D1E0              shl ax,1
              // 00003B00  D1D2              rcl dx,1
              // 00003B02  52                push dx
              // 00003B03  50                push ax
              // 00003B04  E87724            call 0x5f7e
              // 00003B07  D1E0              shl ax,1
              // 00003B09  8B56FE            mov dx,[bp-0x2]
              // 00003B0C  42                inc dx
              // 00003B0D  2BD0              sub dx,ax
              // 00003B0F  52                push dx
              // 00003B10  56                push si
              // 00003B11  E8F2F4            call 0x3006
              // 00003B14  83C406            add sp,0x6
              move_actor(ai, cx+1-(random(2)*2), cy-1);

              // 00003B17  E9460E            jmp 0x4960

            // 00003B1A  0BDB              or bx,bx
            // 00003B1C  7414              jz 0x3b32
            } else if (reg_BX2) {
              // 00003B1E  8B46FC            mov ax,[bp-0x4]
              // 00003B21  48                dec ax
              // 00003B22  50                push ax
              // 00003B23  8B46FE            mov ax,[bp-0x2]
              // 00003B26  40                inc ax
              // 00003B27  50                push ax
              // 00003B28  56                push si
              // 00003B29  E8DAF4            call 0x3006
              // 00003B2C  83C406            add sp,0x6
              move_actor(ai, cx+1, cy-1);

              // 00003B2F  E92E0E            jmp 0x4960

            // 00003B32  0BD2              or dx,dx
            // 00003B34  7414              jz 0x3b4a
            } else if (reg_DX2) {
              // 00003B36  8B46FC            mov ax,[bp-0x4]
              // 00003B39  48                dec ax
              // 00003B3A  50                push ax
              // 00003B3B  8B46FE            mov ax,[bp-0x2]
              // 00003B3E  48                dec ax
              // 00003B3F  50                push ax
              // 00003B40  56                push si
              // 00003B41  E8C2F4            call 0x3006
              // 00003B44  83C406            add sp,0x6
              move_actor(ai, cx-1, cy-1);

              // 00003B47  E9160E            jmp 0x4960

            // 00003B4A  0BFF              or di,di
            // 00003B4C  7C23              jl 0x3b71
            // 00003B4E  8BDF              mov bx,di
            // 00003B50  C1E303            shl bx,byte 0x3
            // 00003B53  83BF2E1714        cmp word [bx+0x172e],0x14
            // 00003B58  7517              jnz 0x3b71
            } else if (dx >= 0 && g_actors[dx].type == T_ROT_A) {
              // 00003B5A  8B5EF0            mov bx,[bp-0x10]
              // 00003B5D  C7070E00          mov word [bx],0xe
              g_actors[ai].type = T_ROCKY_E;

              // 00003B61  FF76FC            push word [bp-0x4]
              // 00003B64  FF76FE            push word [bp-0x2]
              // 00003B67  56                push si
              // 00003B68  E89BF4            call 0x3006
              // 00003B6B  83C406            add sp,0x6
              move_actor(ai, cx, cy);

              // 00003B6E  E9EF0D            jmp 0x4960

            // 00003B71  0BFF              or di,di
            // 00003B73  7D03              jnl 0x3b78
            // 00003B75  E9E80D            jmp 0x4960
            // 00003B78  8BDF              mov bx,di
            // 00003B7A  C1E303            shl bx,byte 0x3
            // 00003B7D  83BF2E1715        cmp word [bx+0x172e],0x15
            // 00003B82  7403              jz 0x3b87
            // 00003B84  E9D90D            jmp 0x4960
            } else if (dx >= 0 && g_actors[dx].type == T_ROT_C) {
              // 00003B87  8B5EF0            mov bx,[bp-0x10]
              // 00003B8A  C7070D00          mov word [bx],0xd
              g_actors[ai].type = T_ROCKY_W;

              // 00003B8E  FF76FC            push word [bp-0x4]
              // 00003B91  FF76FE            push word [bp-0x2]
              // 00003B94  56                push si
              // 00003B95  E86EF4            call 0x3006
              // 00003B98  83C406            add sp,0x6
              move_actor(ai, cx, cy);

              // 00003B9B  E9C20D            jmp 0x4960
            }

            //#undef reg_BX2
            //#undef reg_DX2
            //#undef reg_CX1
            //#undef reg_BX1
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
              g_actors[ai].type = T_ROCKY_W;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              g_actors[ai].type = T_ROCKY_E;
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
              g_actors[ai].type = T_ROCKY_N;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              g_actors[ai].type = T_ROCKY_S;
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
              g_actors[ai].type = T_ROCKY_S;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              g_actors[ai].type = T_ROCKY_N;
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
          g_actors[ai].type = T_BOUNCER_S;
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
          g_actors[ai].type = T_BOUNCER_N;
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
          g_actors[ai].type = T_BOUNCER_E;
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
          g_actors[ai].type = T_BOUNCER_W;
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

    #if 0
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
              g_actors[ai].type = T_ROCKY_E;
              move_actor(ai, cx, cy);
            } else if (other >= 0 && g_actors[other].type == T_ROT_C) {
              g_actors[ai].type = T_ROCKY_W;
              move_actor(ai, cx, cy);
            }
          }
        }
      }
      break;
    #endif

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
  #undef other
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
