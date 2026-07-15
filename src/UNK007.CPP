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
  {3, 0, 1, "kye"},
  {1, T_WALL1, 1, "wall1"},
  {1, T_WALL2, 1, "wall2"},
  {1, T_WALL3, 1, "wall3"},
  {1, T_WALL4, 1, "wall4"},
  {1, T_WALL5, 1, "wall5"},
  {1, T_WALL6, 1, "wall6"},
  {1, T_WALL7, 1, "wall7"},
  {1, T_WALL8, 1, "wall8"},
  {1, T_WALL9, 1, "wall9"},
  {1, T_DIAMOND, 1, "diamond"},
  {1, T_EARTH, 1, "earth"},
  {1, T_DOOR_W_E, 1, "door l->r"},
  {1, T_DOOR_E_W, 1, "door r->l"},
  {1, T_DOOR_N_S, 1, "door u->d"},
  {1, T_DOOR_S_N, 1, "door d->u"},
  {2, T_SBLOCK, 1, "block"},
  {2, T_SLIDER_E, 1, "slider rgt"},
  {2, T_SLIDER_N, 1, "slider up"},
  {2, T_SLIDER_W, 1, "slider lft"},
  {2, T_SLIDER_S, 1, "slider dwn"},
  {2, T_STICKY_V, 1, "sticker tb"},
  {2, T_STICKY_H, 1, "sticker lr"},
  {2, T_BOUNCER_E, 1, "bouncer rgt"},
  {2, T_BOUNCER_N, 1, "bouncer up"},
  {2, T_BOUNCER_W, 1, "bouncer lft"},
  {2, T_BOUNCER_S, 1, "bouncer dwn"},
  {2, T_ROCKY_E, 1, "rocky rgt"},
  {2, T_ROCKY_N, 1, "rocky up"},
  {2, T_ROCKY_W, 1, "rocky lft"},
  {2, T_ROCKY_S, 1, "rocky dwn"},
  {0, 0, 1, ""},
  {2, T_ROT_A, 1, "aclocker"},
  {2, T_ROT_C, 1, "clocker"},
  {2, T_RBLOCK, 1, "round block"},
  {2, T_AUTOSLIDER1, 1, "auto slider"},
  {2, T_AUTOROCKY1, 1, "auto rocky"},
  {2, T_BLACKY1, 1, "blacky"},
  {2, T_TIMER_9, 1, "timer 9"},
  {2, T_TIMER_8, 1, "timer 8"},
  {2, T_TIMER_7, 1, "timer 7"},
  {2, T_TIMER_6, 1, "timer 6"},
  {2, T_TIMER_5, 1, "timer 5"},
  {2, T_TIMER_4, 1, "timer 4"},
  {2, T_TIMER_3, 1, "timer 3"},
  {0, 0, 1, ""},
  {0, 0, 1, ""},
  {0, 0, 1, ""},
  {2, T_B_GNASHER, 1, "gnasher"},
  {2, T_B_BLOB, 1, "blob"},
  {2, T_B_VIRUS, 1, "virus"},
  {2, T_B_SPIKE, 1, "spike"},
  {2, T_B_TWISTER, 1, "twister"},

  {0, 0, 1, ""},
  {0, 0, 1, ""},
  {0, 0, 1, ""},
  {0, 0, 1, ""},
  {0, 0, 1, ""},
  {0, 0, 1, ""},
  {0, 0, 1, ""},
  {0, 0, 1, ""},
  {0, 0, 1, ""},
  {0, 0, 1, ""},
  {0, 0, 1, ""},
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
