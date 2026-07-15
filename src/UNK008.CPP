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

// CS:51EE - FIXME: This likes to spill the abs() inputs into never-read stack locals! Using some hacks to get it in sync. --GM
void update_kye_mouse_target(int cx, int cy) {
  register int t0, t1;
  // SI = cx
  // DI = cy
  if (g_02AA == 0 || g_kye_mouse_cx != cx || g_kye_mouse_cy != cy) {
    clear_kye_mouse_ghost();
    g_kye_mouse_cx = cx;
    g_kye_mouse_cy = cy;
    // Actual behaviour is something like:
    // if (abs(cx - g_kye_main_cx) > 1 || abs(cy - g_kye_main_cy) > 1) { }
    t0 = cx - g_kye_main_cx;
    t1 = cy - g_kye_main_cy;
    if ((abs(t0) | abs(t1)) > 1) {
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

// CS:5395
void try_move_kye(int dx, int dy) {
  // Stack: 0x2E bytes (BP/D2 start)
  int i; // BP/FE
  int ai; // BP/FC
  // BP/FA
  int blocked; // BP/F8
  int pushdiagblocked; // BP/F6
  int newx; // BP/F4
  register int newy; // CX
  int pushx; // BP/F2
  int pushy; // BP/F0
  int didmove; // BP/EE
  int foundmousedir; // BP/EC
  int distsqr; // BP/EA
  int circle_x[4] = {1,0,-1,0}; // BP/E2 const from DS:0D5A
  int circle_y[4] = {0,1,0,-1}; // BP/DA const from DS:0D62
  //int v28; // BP/D8 - pointer premultiplier
  // BP/D6 - pointer premultiplier
  int dx2; // BP/D4
  int dy2; // BP/D2
  // BP+0x00
  register int newdx; // DI
  register int newdy; // SI

  // DX = dx;
  // BX = dy;
  didmove = 0;
  blocked = 0;
  pushdiagblocked = 0;
  // DI = dx;
  // SI = dy;
  newdx = dx;
  newdy = dy;
  newx = g_kye_main_cx + newdx;
  newy = g_kye_main_cy + newdy;

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
  // 00005730  833EB20200        cmp word [0x2b2],0x0
  // 00005735  7503              jnz 0x573a
  // 00005737  E9B700            jmp 0x57f1
  // 0000573A  837EEE00          cmp word [bp-0x12],0x0
  // 0000573E  7403              jz 0x5743
  // 00005740  E9AE00            jmp 0x57f1
  if (g_moving_by_mouse != 0 && !didmove) {
    // 00005743  A1AC02            mov ax,[0x2ac]
    // 00005746  2B06F829          sub ax,[0x29f8]
    // 0000574A  8946D4            mov [bp-0x2c],ax
    dx2 = g_kye_target_cx - g_kye_main_cx;
    // 0000574D  F76ED4            imul word [bp-0x2c]
    // 00005750  50                push ax
      // 00005751  A1AE02            mov ax,[0x2ae]
      // 00005754  2B06F629          sub ax,[0x29f6]
      // 00005758  8946D2            mov [bp-0x2e],ax
    dy2 = g_kye_target_cy - g_kye_main_cy;
      // 0000575B  F76ED2            imul word [bp-0x2e]
    // 0000575E  5A                pop dx
    // 0000575F  03D0              add dx,ax
    // 00005761  8956EA            mov [bp-0x16],dx
    distsqr = (dx2*dx2)+(dy2*dy2);
    // 00005764  C746EC0000        mov word [bp-0x14],0x0
    foundmousedir = 0;
    // 00005769  C746FE0000        mov word [bp-0x2],0x0
    // 0000576E  C746D80000        mov word [bp-0x28],0x0
    // 00005773  EB62              jmp 0x57d7
    // --------
    // 000057D7  837EEC00          cmp word [bp-0x14],0x0
    // 000057DB  7506              jnz 0x57e3
    // 000057DD  837EFE04          cmp word [bp-0x2],0x4
    // 000057E1  7C92              jl 0x5775
    // --------
    // 000057CC  8B46FE            mov ax,[bp-0x2]
    // 000057CF  8346D802          add word [bp-0x28],0x2
    // 000057D3  40                inc ax
    // 000057D4  8946FE            mov [bp-0x2],ax
    for (i = 0; foundmousedir && i < 4; i++) {
      // 00005775  8B5ED8            mov bx,[bp-0x28]
      // 00005778  8D46E2            lea ax,[bp-0x1e]
      // 0000577B  03D8              add bx,ax
      // 0000577D  8B3F              mov di,[bx]
      newdx = circle_x[i];
      // 0000577F  8B5ED8            mov bx,[bp-0x28]
      // 00005782  8D46DA            lea ax,[bp-0x26]
      // 00005785  03D8              add bx,ax
      // 00005787  8B37              mov si,[bx]
      newdy = circle_y[i];
      // 00005789  A1F829            mov ax,[0x29f8]
      // 0000578C  03C7              add ax,di
      // 0000578E  8946F4            mov [bp-0xc],ax
      newx = g_kye_main_cx + newdx;
      // 00005791  8B0EF629          mov cx,[0x29f6]
      // 00005795  03CE              add cx,si
      newy = g_kye_main_cy + newdy;
      // 00005797  BA2800            mov dx,0x28
      // 0000579A  F7EA              imul dx
      // 0000579C  8BD1              mov dx,cx
      // 0000579E  D1E2              shl dx,1
      // 000057A0  03C2              add ax,dx
      // 000057A2  8BD8              mov bx,ax
      // 000057A4  83BF7E12FF        cmp word [bx+0x127e],0xffffffffffffffff
      // 000057A9  7521              jnz 0x57cc
      if (g_level_tiles[newx][newy] == T_EMPTY) {
        // 000057AB  A1AC02            mov ax,[0x2ac]
        // 000057AE  2B46F4            sub ax,[bp-0xc]
        // 000057B1  8BD0              mov dx,ax
        // 000057B3  F7EA              imul dx
        // 000057B5  50                push ax
        // 000057B6  A1AE02            mov ax,[0x2ae]
        // 000057B9  2BC1              sub ax,cx
        // 000057BB  8BD0              mov dx,ax
        // 000057BD  F7EA              imul dx
        // 000057BF  5B                pop bx
        // 000057C0  03D8              add bx,ax
        // 000057C2  3B5EEA            cmp bx,[bp-0x16]
        // 000057C5  7D05              jnl 0x57cc
        if (((g_kye_target_cx-newx)*(g_kye_target_cx-newx)) + ((g_kye_target_cy-newy)*(g_kye_target_cy-newy)) < distsqr) {
          // 000057C7  C746EC0100        mov word [bp-0x14],0x1
            foundmousedir = 1;
        }
      }
    }

    // 000057E3  837EEC00          cmp word [bp-0x14],0x0
    // 000057E7  7408              jz 0x57f1
    if (foundmousedir != 0) {
      // 000057E9  56                push si
      // 000057EA  57                push di
      // 000057EB  E8FCFA            call 0x52ea
      // 000057EE  83C404            add sp,0x4
      move_kye(newdx, newdy);
    }
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
          if (random(2) == 0) {
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
