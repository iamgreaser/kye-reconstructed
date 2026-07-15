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
tilespec_s g_06B8[64] = {
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
int g_0D38; // DS:0D38
int g_0D3A; // DS:0D3A
char g_0D3C[20] = "Kye-Tools"; // DS:0D3C

// CS:49F2 - ***CODE MATCH!***
int kye_is_in_level(void) {
  int x, y;
  for (x = 0; x < LEVEL_LX; x++) {
    for (y = 0; y < LEVEL_LY; y++) {
      if (g_level_tiles[x][y] == T_KYE) {
        return 1;
      }
    }
  }
  return 0;
}

// CS:4A1F - ***CODE MATCH!***
void cs_4A1F(void) {
  RECT r;

  if (kye_is_in_level()) {
    g_06B8[0].tx = 0;
  } else {
    g_06B8[0].tx = 1;
  }

  GetClientRect(g_2E90, &r);
  InvalidateRect(g_2E90, &r, 0);
  UpdateWindow(g_2E90);
}

// CS:4A63 - ***CODE MATCH!***
void cs_4A63(int mode, int type, int cx, int cy) {
  int on_border = (cx <= 0 || cx >= LEVEL_LX-1 || cy <= 0 || cy >= LEVEL_LY-1); // DX

  switch (mode) {
    default:
      return;

    case 1:
      if (type >= T_WALL9 && type <= T_WALL1) {
        g_level_tiles[cx][cy] = type;
        return;
      }
      if (!on_border) {
        g_level_tiles[cx][cy] = type;
        return;
      }
      break;

    case 2:
      if (!on_border) {
        create_actor(type, cx, cy);
        return;
      }
      break;

    case 3:
      if (!on_border) {
        g_level_tiles[cx][cy] = T_KYE;
        g_kye_main_cx = cx;
        g_kye_main_cy = cy;
        cs_4A1F();
      }
      break;
  }
}

// CS:4B17 - ***CODE MATCH!***
void cs_4B17(int cx, int cy) {
  int tile; // DI

  tile = g_level_tiles[cx][cy];
  // BUG? This seems to be an off-by-one and won't delete the first actor?
  if (tile > 0) {
    mark_actor_for_deletion(tile);
    g_level_tiles[cx][cy] = T_EMPTY;
  } else {
    g_level_tiles[cx][cy] = T_EMPTY;
  }
  garbage_collect_actors();

  if (tile == T_KYE) {
    cs_4A1F();
  }
}

// CS:4B7A - ***CODE MATCH!***
void cs_4B7A(int cx, int cy) {
  if (g_level_tiles[cx][cy] == T_EMPTY) {
    if (g_06B8[g_0D38].tx != 0) {
      cs_4A63(g_06B8[g_0D38].mode, g_06B8[g_0D38].type, cx, cy);
    }
    return;
  } else {
    cs_4B17(cx, cy);
  }
}

// CS:4BCE - ***CODE MATCH!***
void cs_4BCE(int idx, HDC dc) {
  int cx; // [BP-0x2].w
  int cy; // [BP-0x4].w
  // [BP-0x6].w is a spilled value
  // [BP-0x8].w is a spilled value

  cx = idx/16;
  cy = idx%16;

  SelectObject(dc, g_hpenWhite);
  SelectObject(dc, g_hbrWhite);
  Rectangle(dc,
    cx*(TILE_LX+(2*2))+2,
    cy*(TILE_LY+(2*2))+2,
    cx*(TILE_LX+(2*2))+TILE_LX+(2*2)-2,
    cy*(TILE_LY+(2*2))+TILE_LY+(2*2)-2);
}

// CS:4C37 - ***CODE MATCH!***
void cs_4C37(int idx, HDC dc, HDC bmpDC) {
  // Some of these are probably spilt values.
  // Either way, it compiles to the correct code.
  int mode; // [bp-0x2].w
  int type; // DX
  int px; // [bp-0x4].w
  int py; // [bp-0x6].w
  int cy; // [bp-0x8].w
  int cx; // [bp-0xA].w
  RECT r; // [bp-0x12]/0x8
  HBRUSH brush; // [bp-0x14].w
  int src_px; // SI
  int src_py; // DI

  cx = idx/16;
  cy = idx%16;
  px = cx*(TILE_LX+(2*2))+2;
  py = cy*(TILE_LY+(2*2))+2;

  if (idx == g_0D38) {
    brush = g_hbrBlack;
  } else {
    brush = g_hbrWhite;
  }

  SetRect(&r,
    cx*(TILE_LX+(2*2)),
    cy*(TILE_LX+(2*2)),
    (cx*(TILE_LX+(2*2)))+(TILE_LX+(2*2)),
    (cy*(TILE_LX+(2*2)))+(TILE_LX+(2*2)));
  FrameRect(dc, &r, brush);
  InflateRect(&r, -1, -1);
  FrameRect(dc, &r, brush);

  if (g_06B8[idx].tx == 0) {
    cs_4BCE(idx, dc);
    return;
  }

  mode = g_06B8[idx].mode;
  type = (g_06B8[idx].type);
  switch (mode) {
    case 0:
      cs_4BCE(idx, dc);
      return;

    case 1:
      if (type <= T_WALL1 && type >= T_WALL9) {
        src_px = (abs(type) - abs(T_WALL1) + 3)*TILE_LX;
        src_py = 0*TILE_LY;
      } else if (type == T_EARTH) {
        src_px = 0*TILE_LY;
        src_py = 0*TILE_LY;
      } else if (type == T_DIAMOND) {
        src_px = 12*TILE_LY;
        src_py = 0*TILE_LY;
      } else if (type == T_DOOR_W_E || type == T_DOOR_E_W) {
        src_px = 14*TILE_LY;
        src_py = 0*TILE_LY;
      } else if (type == T_DOOR_N_S || type == T_DOOR_S_N) {
        src_px = 13*TILE_LY;
        src_py = 0*TILE_LY;
      }
      SelectObject(bmpDC, g_hbmpWall);
      BitBlt(dc, px, py, TILE_LX, TILE_LY, bmpDC, src_px, src_py, SRCCOPY);
      return;

    case 2:
      if (type >= T_AUTOSLIDER1) {
        src_px = (type-23)*TILE_LX;
        src_py = 4*TILE_LY;
      } else {
        src_px = type*TILE_LX;
        src_py = 0*TILE_LY;
      }

      if (type >= T_TIMER_0 && type <= T_TIMER_9) {
        src_px = (type-50)*TILE_LX;
        src_py = 1*TILE_LY;
      }

      SelectObject(bmpDC, g_hbmpBlock);
      BitBlt(dc, px, py, TILE_LX, TILE_LY, bmpDC, src_px, src_py, SRCCOPY);
      return;

    case 3:
      SelectObject(bmpDC, g_hbmpKye);
      BitBlt(dc, px, py, TILE_LX, TILE_LY, bmpDC, 0, 0, SRCCOPY);
      return;
  }
}

// CS:4E3F - ***CODE MATCH!***
void cs_4E3F(HDC dc) {
  HDC compatDC; // [bp-0x2].w
  int i; // SI

  compatDC = CreateCompatibleDC(dc);

  for (i = 0; i < 4*16; i++) {
    cs_4C37(i, dc, compatDC);
  }

  DeleteDC(compatDC);
}

// CS:4E72 - ***CODE MATCH!***
void cs_4E72(void) {
  HDC dc; // SI
  PAINTSTRUCT paint; // [bp-0x20]/0020

  dc = BeginPaint(g_2E90, &paint);
  SelectObject(dc, g_hpenWhite);

  cs_4E3F(dc);

  ReleaseDC(g_2E90, dc);
  EndPaint(g_2E90, &paint);
}

// CS:4EB5 - ***CODE MATCH!***
void cs_4EB5(HWND hWnd, int mouse_x, int mouse_y, int wParam) {
  int cx; // [bp-0x2].w
  int cy; // [bp-0x4].w
  int old_idx; // [bp-0x6].w
  int new_idx; // [bp-0x8].w
  RECT r; // [bp-0x10]/0008
  POINT p; // [bp-0x14]/0004
  HDC dc; // SI
  HDC compatDC; // [bp-0x16].w

  (void)wParam;

  p.x = mouse_x;
  p.y = mouse_y;

  GetClientRect(hWnd, &r);

  if (PtInRect(&r, p)) {
    dc = GetDC(hWnd);
    compatDC = CreateCompatibleDC(dc);

    old_idx = g_0D38;
    cx = p.x/20;
    cy = p.y/20;
    new_idx = (cx*16)+cy;

    if (g_06B8[new_idx].tx != 0) {
      g_0D38 = -1;
      cs_4C37(old_idx, dc, compatDC);
      g_0D38 = new_idx;
      cs_4C37(new_idx, dc, compatDC);
    }

    DeleteDC(compatDC);
    ReleaseDC(hWnd, dc);

    strcpy(g_toolname, g_06B8[g_0D38].name);

    acquire_main_dc();
    repaint_status_left();
    release_main_dc();
  }
}

// CS:4FA6 - ToolboxWndProc(), Ordinal 10 - ***CODE MATCH!***
LONG FAR PASCAL _export ToolboxWndProc(HWND hWnd, WORD msg, WORD wParam, LONG lParam) {
  switch (msg) {
    case WM_CREATE:
      break;

    case WM_LBUTTONDOWN:
      cs_4EB5(hWnd, LOWORD(lParam), HIWORD(lParam), wParam);
      break;

    case WM_PAINT:
      cs_4E72();
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    case WM_MOUSEMOVE:
      SetCursor(g_hcArrow);
      break;

    default:
      return DefWindowProc(hWnd, msg, wParam, lParam);
  }
  return 0;
}

// CS:501F - ***CODE MATCH!***
int cs_501F(void) {
  WNDCLASS wcls; // [bp-0x1a]/001A

  if (g_0D3A == 0) {
    wcls.style = CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS; // 0x000B
    wcls.lpfnWndProc = ToolboxWndProc;
    wcls.cbClsExtra = 0;
    wcls.cbWndExtra = 0;
    wcls.hInstance = g_hInstance;
    wcls.hIcon = 0;
    wcls.hCursor = 0;
    wcls.hbrBackground = GetStockObject(WHITE_BRUSH);
    wcls.lpszMenuName = NULL;
    wcls.lpszClassName = g_0D3C; // DS:0D3C
    if (RegisterClass(&wcls) == 0) {
      return 0;
    }

    g_2E90 = CreateWindow(
      g_0D3C, // DS:0D3C
      "Kye-Tools", // DS:0D50
      WS_POPUP|WS_CAPTION, // 0x80C00000
      50,
      90,
      ((TILE_LX+(2*2))*4)+2,
      ((TILE_LY+(2*2))*16)+20,
      g_mainwnd,
      NULL,
      g_hInstance,
      NULL);
    if (g_2E90 == 0) {
      return 0;
    }
    g_0D3A = 1;
  }

  cs_4A1F();

  ShowWindow(g_2E90, SW_SHOW);

  strcpy(g_toolname, g_06B8[g_0D38].name);

  acquire_main_dc();
  repaint_status_left();
  release_main_dc();

  return 1;
}

// CS:5119 - ***CODE MATCH!***
int cs_5119(void) {
  ShowWindow(g_2E90, SW_HIDE);
  return 1;
}

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
