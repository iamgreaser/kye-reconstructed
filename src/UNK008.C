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
  if (g_02AA != 0 && g_level_tiles[g_kye_mouse_cx][g_kye_mouse_cy] == T_EMPTY) {
    delete_and_draw_unacting_tile(g_kye_mouse_cx, g_kye_mouse_cy);
  }
  g_02AA = 0;
}

// CS:51EE - ***CODE MATCH!***
void update_kye_mouse_target(int cx, int cy) {
  register int t0, t1;
  // SI = cx
  // DI = cy
  if (g_02AA == 0 || g_kye_mouse_cx != cx || g_kye_mouse_cy != cy) {
    clear_kye_mouse_ghost();
    g_kye_mouse_cx = cx;
    g_kye_mouse_cy = cy;
    if (abs(cx - g_kye_main_cx) > 1 || abs(cy - g_kye_main_cy) > 1) {
      if (g_level_tiles[g_kye_mouse_cx][g_kye_mouse_cy] == T_EMPTY) {
        draw_kye_mouse_ghost();
        g_02AA = 1;
      }
    }
  }
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
        if (((g_kye_target_cx-newx)*(g_kye_target_cx-newx)) + ((g_kye_target_cy-newy)*(g_kye_target_cy-newy)) < distsqr) {
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

// CS:5A54 - FIXME: Register autoallocation allocates registers to the wrong place --GM
void animate_autospawners(void) {
  int spawn_type; // [bp-0x2].w
  int ai; // [bp-0x4].w
  int cx; // [bp-0x6].w
  int cy; // [bp-0x8].w -- it likes to assign a register to this, and spill into a stack local. Not sure why. Planetary alignment failure, perhaps?
  int spawn_x; // [bp-0xA].w
  int spawn_y; // [bp-0xC].w
  //int v0E; // [bp-0xE].w - auto offset

  // FIXME: Fix the borked register allocation and then remove this resyncing hack --GM
  g_tick_counter++; g_tick_counter++; g_tick_counter++; g_tick_counter-=3;

  // 00005A54  55                push bp
  // 00005A55  8BEC              mov bp,sp
  // 00005A57  83EC0E            sub sp,0xe
  // 00005A5A  56                push si
  // 00005A5B  57                push di

  // 00005A5C  A1BE02            mov ax,[0x2be]
  // 00005A5F  BB0700            mov bx,0x7
  // 00005A62  99                cwd
  // 00005A63  F7FB              idiv bx
  // 00005A65  0BD2              or dx,dx
  // 00005A67  7403              jz 0x5a6c
  // 00005A69  E90702            jmp 0x5c73
  if ((g_tick_counter % 7) == 0) {
    // 00005A6C  C746FC0000        mov word [bp-0x4],0x0
    // 00005A71  C746F20000        mov word [bp-0xe],0x0
    // 00005A76  BE2E17            mov si,0x172e = &g_actors[ai].type
    // 00005A79  BF3417            mov di,0x1734 = &g_actors[ai].var0
    // --------
    // 00005A7C  8B46FC            mov ax,[bp-0x4]
    // 00005A7F  3B06BA02          cmp ax,[0x2ba]
    // 00005A83  7C03              jl 0x5a88
    // 00005A85  E9EB01            jmp 0x5c73
    // --------
      // appears at end:
      // 00005C5A  8346F208          add word [bp-0xe],0x8
      // 00005C5E  83C608            add si,0x8
      // 00005C61  83C708            add di,0x8
      // 00005C64  FF46FC            inc word [bp-0x4]
    for (ai = 0; ai < g_actor_count; ai++) {
      // 00005A88  833C17            cmp word [si],0x17
      // 00005A8B  7D03              jnl 0x5a90
      // 00005A8D  E9E400            jmp 0x5b74
      // 00005A90  833C1A            cmp word [si],0x1a
      // 00005A93  7E03              jng 0x5a98
      // 00005A95  E9DC00            jmp 0x5b74
      if (g_actors[ai].type >= T_AUTOSLIDER1 && g_actors[ai].type <= T_AUTOSLIDER4) {
        // 00005A98  8B5EF2            mov bx,[bp-0xe]
        // 00005A9B  8B873017          mov ax,[bx+0x1730]  = g_actors[ai].x
        // 00005A9F  8946FA            mov [bp-0x6],ax
        cx = g_actors[ai].cx;
        // 00005AA2  8B873217          mov ax,[bx+0x1732]  = g_actors[ai].y
        // 00005AA6  8946F8            mov [bp-0x8],ax
        cy = g_actors[ai].cy;

        // 00005AA9  8B04              mov ax,[si]
        // 00005AAB  40                inc ax
        // 00005AAC  8904              mov [si],ax
        g_actors[ai].type = g_actors[ai].type + 1;
        // 00005AAE  833C1A            cmp word [si],0x1a
        // 00005AB1  7E04              jng 0x5ab7
        if (g_actors[ai].type > T_AUTOSLIDER4) {
          // 00005AB3  C7041700          mov word [si],0x17
          g_actors[ai].type = T_AUTOSLIDER1;
        }

        // 00005AB7  FF76F8            push word [bp-0x8]
        // 00005ABA  FF76FA            push word [bp-0x6]
        // 00005ABD  FF76FC            push word [bp-0x4]
        // 00005AC0  E843D5            call 0x3006
        // 00005AC3  83C406            add sp,0x6
        move_actor(ai, cx, cy);

        // 00005AC6  8B05              mov ax,[di]
        // 00005AC8  8BD0              mov dx,ax
        // 00005ACA  3B46F8            cmp ax,[bp-0x8]
        // 00005ACD  7D06              jnl 0x5ad5
        if (g_actors[ai].var0 < cy) {
          // 00005ACF  40                inc ax
          // 00005AD0  8905              mov [di],ax
          // 00005AD2  E98501            jmp 0x5c5a
          g_actors[ai].var0 = g_actors[ai].var0 + 1;

        } else {
          // 00005AD5  8B1C              mov bx,[si]
          // 00005AD7  83EB17            sub bx,0x17
          // 00005ADA  83FB03            cmp bx,0x3
          // 00005ADD  7755              ja 0x5b34
          // 00005ADF  D1E3              shl bx,1
          // 00005AE1  2EFFA7825C        jmp word near [cs:bx+0x5c82]
          switch (g_actors[ai].type) {
              // 00: 5AE6
              // 01: 5AFA
              // 02: 5B0E
              // 03: 5B22

            case T_AUTOSLIDER1:
              // 00005AE6  C746FE0400        mov word [bp-0x2],0x4
              spawn_type = T_SLIDER_E;
              // 00005AEB  8B46FA            mov ax,[bp-0x6]
              // 00005AEE  40                inc ax
              // 00005AEF  8946F6            mov [bp-0xa],ax
              // 00005AF2  8B46F8            mov ax,[bp-0x8]
              // 00005AF5  8946F4            mov [bp-0xc],ax
              // 00005AF8  EB3A              jmp 0x5b34
              spawn_x = cx + 1;
              spawn_y = cy;
              break;

            case T_AUTOSLIDER2:
              // 00005AFA  C746FE0100        mov word [bp-0x2],0x1
              spawn_type = T_SLIDER_N;
              // 00005AFF  8B46FA            mov ax,[bp-0x6]
              // 00005B02  8946F6            mov [bp-0xa],ax
              // 00005B05  8B46F8            mov ax,[bp-0x8]
              // 00005B08  48                dec ax
              // 00005B09  8946F4            mov [bp-0xc],ax
              // 00005B0C  EB26              jmp 0x5b34
              spawn_x = cx;
              spawn_y = cy - 1;
              break;

            case T_AUTOSLIDER3:
              // 00005B0E  C746FE0300        mov word [bp-0x2],0x3
              spawn_type = T_SLIDER_W;
              // 00005B13  8B46FA            mov ax,[bp-0x6]
              // 00005B16  48                dec ax
              // 00005B17  8946F6            mov [bp-0xa],ax
              // 00005B1A  8B46F8            mov ax,[bp-0x8]
              // 00005B1D  8946F4            mov [bp-0xc],ax
              // 00005B20  EB12              jmp 0x5b34
              spawn_x = cx - 1;
              spawn_y = cy;
              break;

            case T_AUTOSLIDER4:
              // 00005B22  C746FE0200        mov word [bp-0x2],0x2
              spawn_type = T_SLIDER_S;
              // 00005B27  8B46FA            mov ax,[bp-0x6]
              // 00005B2A  8946F6            mov [bp-0xa],ax
              // 00005B2D  8B46F8            mov ax,[bp-0x8]
              // 00005B30  40                inc ax
              // 00005B31  8946F4            mov [bp-0xc],ax
              spawn_x = cx;
              spawn_y = cy + 1;
              break;
          }
          // 00005B34  8B46F6            mov ax,[bp-0xa]
          // 00005B37  BA2800            mov dx,0x28
          // 00005B3A  F7EA              imul dx
          // 00005B3C  8B56F4            mov dx,[bp-0xc]
          // 00005B3F  D1E2              shl dx,1
          // 00005B41  03C2              add ax,dx
          // 00005B43  8BD8              mov bx,ax
          // 00005B45  83BF7E12FF        cmp word [bx+0x127e],0xffffffffffffffff
          // 00005B4A  7403              jz 0x5b4f
          // 00005B4C  E90B01            jmp 0x5c5a
          if (g_level_tiles[spawn_x][spawn_y] == T_EMPTY) {
            // 00005B4F  C7050000          mov word [di],0x0
            g_actors[ai].var0 = 0;
            // 00005B53  FF76F4            push word [bp-0xc]
            // 00005B56  FF76F6            push word [bp-0xa]
            // 00005B59  FF76FE            push word [bp-0x2]
            // 00005B5C  E873C1            call 0x1cd2
            // 00005B5F  83C406            add sp,0x6
            create_actor(spawn_type, spawn_x, spawn_y);
            // 00005B62  8BD0              mov dx,ax
            // 00005B64  FF76F4            push word [bp-0xc]
            // 00005B67  FF76F6            push word [bp-0xa]
            // 00005B6A  50                push ax
            // 00005B6B  E898D4            call 0x3006
            // 00005B6E  83C406            add sp,0x6
            // 00005B71  E9E600            jmp 0x5c5a
            move_actor(ai, cx, cy);
          }
        }

      // 00005B74  833C1B            cmp word [si],0x1b
      // 00005B77  7D03              jnl 0x5b7c
      // 00005B79  E9DE00            jmp 0x5c5a
      // 00005B7C  833C1E            cmp word [si],0x1e
      // 00005B7F  7E03              jng 0x5b84
      // 00005B81  E9D600            jmp 0x5c5a
      } else if (g_actors[ai].type >= T_AUTOROCKY1 && g_actors[ai].type <= T_AUTOROCKY4) {
        // 00005B84  8B5EF2            mov bx,[bp-0xe]
        // 00005B87  8B873017          mov ax,[bx+0x1730]
        // 00005B8B  8946FA            mov [bp-0x6],ax
        cx = g_actors[ai].cx;
        // 00005B8E  8B873217          mov ax,[bx+0x1732]
        // 00005B92  8946F8            mov [bp-0x8],ax
        cy = g_actors[ai].cy;

        // 00005B95  8B04              mov ax,[si]
        // 00005B97  40                inc ax
        // 00005B98  8904              mov [si],ax
        g_actors[ai].type = g_actors[ai].type + 1;
        // 00005B9A  833C1E            cmp word [si],0x1e
        // 00005B9D  7E04              jng 0x5ba3
        if (g_actors[ai].type > T_AUTOROCKY4) {
          // 00005B9F  C7041B00          mov word [si],0x1b
          g_actors[ai].type = T_AUTOROCKY1;
        }

        // 00005BA3  FF76F8            push word [bp-0x8]
        // 00005BA6  FF76FA            push word [bp-0x6]
        // 00005BA9  FF76FC            push word [bp-0x4]
        // 00005BAC  E857D4            call 0x3006
        // 00005BAF  83C406            add sp,0x6
        move_actor(ai, cx, cy);

        // 00005BB2  8B05              mov ax,[di]
        // 00005BB4  8BD0              mov dx,ax
        // 00005BB6  3B46F8            cmp ax,[bp-0x8]
        // 00005BB9  7D06              jnl 0x5bc1
        if (g_actors[ai].var0 < cy) {
          // 00005BBB  40                inc ax
          // 00005BBC  8905              mov [di],ax
          // 00005BBE  E99900            jmp 0x5c5a
          g_actors[ai].var0 = g_actors[ai].var0 + 1;

        } else {
          // 00005BC1  8B1C              mov bx,[si]
          // 00005BC3  83EB1B            sub bx,0x1b
          // 00005BC6  83FB03            cmp bx,0x3
          // 00005BC9  7755              ja 0x5c20
          // 00005BCB  D1E3              shl bx,1
          // 00005BCD  2EFFA77A5C        jmp word near [cs:bx+0x5c7a]
            // 00: 5BD2
            // 01: 5BE6
            // 02: 5BFA
            // 03: 5C0E
          switch (g_actors[ai].type) {
            case T_AUTOROCKY1:
              // 00005BD2  C746FE0E00        mov word [bp-0x2],0xe
              spawn_type = T_ROCKY_E;
              // 00005BD7  8B46FA            mov ax,[bp-0x6]
              // 00005BDA  40                inc ax
              // 00005BDB  8946F6            mov [bp-0xa],ax
              // 00005BDE  8B46F8            mov ax,[bp-0x8]
              // 00005BE1  8946F4            mov [bp-0xc],ax
              // 00005BE4  EB3A              jmp 0x5c20
              spawn_x = cx + 1;
              spawn_y = cy;
              break;

            case T_AUTOROCKY2:
              // 00005BE6  C746FE0B00        mov word [bp-0x2],0xb
              spawn_type = T_ROCKY_N;
              // 00005BEB  8B46FA            mov ax,[bp-0x6]
              // 00005BEE  8946F6            mov [bp-0xa],ax
              // 00005BF1  8B46F8            mov ax,[bp-0x8]
              // 00005BF4  48                dec ax
              // 00005BF5  8946F4            mov [bp-0xc],ax
              // 00005BF8  EB26              jmp 0x5c20
              spawn_x = cx;
              spawn_y = cy - 1;
              break;

            case T_AUTOROCKY3:
              // 00005BFA  C746FE0D00        mov word [bp-0x2],0xd
              spawn_type = T_ROCKY_W;
              // 00005BFF  8B46FA            mov ax,[bp-0x6]
              // 00005C02  48                dec ax
              // 00005C03  8946F6            mov [bp-0xa],ax
              // 00005C06  8B46F8            mov ax,[bp-0x8]
              // 00005C09  8946F4            mov [bp-0xc],ax
              // 00005C0C  EB12              jmp 0x5c20
              spawn_x = cx - 1;
              spawn_y = cy;
              break;

            case T_AUTOROCKY4:
              // 00005C0E  C746FE0C00        mov word [bp-0x2],0xc
              spawn_type = T_ROCKY_S;
              // 00005C13  8B46FA            mov ax,[bp-0x6]
              // 00005C16  8946F6            mov [bp-0xa],ax
              // 00005C19  8B46F8            mov ax,[bp-0x8]
              // 00005C1C  40                inc ax
              // 00005C1D  8946F4            mov [bp-0xc],ax
              spawn_x = cx;
              spawn_y = cy + 1;
              break;
          }

          // 00005C20  8B46F6            mov ax,[bp-0xa]
          // 00005C23  BA2800            mov dx,0x28
          // 00005C26  F7EA              imul dx
          // 00005C28  8B56F4            mov dx,[bp-0xc]
          // 00005C2B  D1E2              shl dx,1
          // 00005C2D  03C2              add ax,dx
          // 00005C2F  8BD8              mov bx,ax
          // 00005C31  83BF7E12FF        cmp word [bx+0x127e],0xffffffffffffffff
          // 00005C36  7522              jnz 0x5c5a
          if (g_level_tiles[spawn_x][spawn_y] == T_EMPTY) {
            // 00005C38  C7050000          mov word [di],0x0
            g_actors[ai].var0 = 0;
            // 00005C3C  FF76F4            push word [bp-0xc]
            // 00005C3F  FF76F6            push word [bp-0xa]
            // 00005C42  FF76FE            push word [bp-0x2]
            // 00005C45  E88AC0            call 0x1cd2
            // 00005C48  83C406            add sp,0x6
            create_actor(spawn_type, spawn_x, spawn_y);
            // 00005C4B  8BD0              mov dx,ax
            // 00005C4D  FF76F4            push word [bp-0xc]
            // 00005C50  FF76F6            push word [bp-0xa]
            // 00005C53  50                push ax
            // 00005C54  E8AFD3            call 0x3006
            // 00005C57  83C406            add sp,0x6
            move_actor(ai, cx, cy);
          }
        }
      }
      // handled above:
        // 00005C5A  8346F208          add word [bp-0xe],0x8
        // 00005C5E  83C608            add si,0x8
        // 00005C61  83C708            add di,0x8
        // 00005C64  FF46FC            inc word [bp-0x4]
      // 00005C67  8B46FC            mov ax,[bp-0x4]
      // 00005C6A  3B06BA02          cmp ax,[0x2ba]
      // 00005C6E  7D03              jnl 0x5c73
      // 00005C70  E915FE            jmp 0x5a88
    }
  }

  // 00005C73  E8DBC0            call 0x1d51
  garbage_collect_actors();

  // 00005C76  5F                pop di
  // 00005C77  5E                pop si
  // 00005C78  C9                leave
  // 00005C79  C3                ret
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
