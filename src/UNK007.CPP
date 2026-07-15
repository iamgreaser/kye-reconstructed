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
    cx*(TILE_LX+(2*2))+TILE_LX-2,
    cy*(TILE_LY+(2*2))+TILE_LY-2);
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

// CS:4EB5
void cs_4EB5(int a0, int a1, int a2) {
  // TODO! --GM
  // 00004EB5  55                push bp
  // 00004EB6  8BEC              mov bp,sp
  // 00004EB8  83EC16            sub sp,0x16
  // 00004EBB  56                push si
  // 00004EBC  57                push di
  // 00004EBD  8B7E04            mov di,[bp+0x4]
  // 00004EC0  8B4606            mov ax,[bp+0x6]
  // 00004EC3  8946EC            mov [bp-0x14],ax
  // 00004EC6  8B4608            mov ax,[bp+0x8]
  // 00004EC9  8946EE            mov [bp-0x12],ax
  // 00004ECC  57                push di
  // 00004ECD  16                push ss
  // 00004ECE  8D46F0            lea ax,[bp-0x10]
  // 00004ED1  50                push ax
  // 00004ED2  9AFFFF0000        call word 0x0:word 0xffff
  // 00004ED7  16                push ss
  // 00004ED8  8D46F0            lea ax,[bp-0x10]
  // 00004EDB  50                push ax
  // 00004EDC  FF76EE            push word [bp-0x12]
  // 00004EDF  FF76EC            push word [bp-0x14]
  // 00004EE2  9AFFFF0000        call word 0x0:word 0xffff
  // 00004EE7  0BC0              or ax,ax
  // 00004EE9  7503              jnz 0x4eee
  // 00004EEB  E9B400            jmp 0x4fa2
  // 00004EEE  57                push di
  // 00004EEF  9AFFFF0000        call word 0x0:word 0xffff
  // 00004EF4  8BF0              mov si,ax
  // 00004EF6  50                push ax
  // 00004EF7  9AFFFF0000        call word 0x0:word 0xffff
  // 00004EFC  8946EA            mov [bp-0x16],ax
  // 00004EFF  A1380D            mov ax,[0xd38]
  // 00004F02  8946FA            mov [bp-0x6],ax
  // 00004F05  8B46EC            mov ax,[bp-0x14]
  // 00004F08  BB1400            mov bx,0x14
  // 00004F0B  99                cwd
  // 00004F0C  F7FB              idiv bx
  // 00004F0E  8946FE            mov [bp-0x2],ax
  // 00004F11  8B46EE            mov ax,[bp-0x12]
  // 00004F14  99                cwd
  // 00004F15  F7FB              idiv bx
  // 00004F17  8946FC            mov [bp-0x4],ax
  // 00004F1A  8B46FE            mov ax,[bp-0x2]
  // 00004F1D  C1E004            shl ax,byte 0x4
  // 00004F20  0346FC            add ax,[bp-0x4]
  // 00004F23  8946F8            mov [bp-0x8],ax
  // 00004F26  BA1A00            mov dx,0x1a
  // 00004F29  F7EA              imul dx
  // 00004F2B  8BD8              mov bx,ax
  // 00004F2D  83BFBC0600        cmp word [bx+0x6bc],0x0
  // 00004F32  7424              jz 0x4f58
  // 00004F34  C706380DFFFF      mov word [0xd38],0xffff
  // 00004F3A  FF76EA            push word [bp-0x16]
  // 00004F3D  56                push si
  // 00004F3E  FF76FA            push word [bp-0x6]
  // 00004F41  E8F3FC            call 0x4c37
  // 00004F44  83C406            add sp,0x6
  // 00004F47  8B46F8            mov ax,[bp-0x8]
  // 00004F4A  A3380D            mov [0xd38],ax
  // 00004F4D  FF76EA            push word [bp-0x16]
  // 00004F50  56                push si
  // 00004F51  50                push ax
  // 00004F52  E8E2FC            call 0x4c37
  // 00004F55  83C406            add sp,0x6
  // 00004F58  FF76EA            push word [bp-0x16]
  // 00004F5B  9AFFFF0000        call word 0x0:word 0xffff
  // 00004F60  57                push di
  // 00004F61  56                push si
  // 00004F62  9AFFFF0000        call word 0x0:word 0xffff
  // 00004F67  A1380D            mov ax,[0xd38]
  // 00004F6A  BA1A00            mov dx,0x1a
  // 00004F6D  F7EA              imul dx
  // 00004F6F  05BE06            add ax,0x6be
  // 00004F72  BF5E2D            mov di,0x2d5e
  // 00004F75  FC                cld
  // 00004F76  1E                push ds
  // 00004F77  07                pop es
  // 00004F78  57                push di
  // 00004F79  8BF8              mov di,ax
  // 00004F7B  33C0              xor ax,ax
  // 00004F7D  B9FFFF            mov cx,0xffff
  // 00004F80  F2AE              repne scasb
  // 00004F82  F7D1              not cx
  // 00004F84  2BF9              sub di,cx
  // 00004F86  D1E9              shr cx,1
  // 00004F88  57                push di
  // 00004F89  06                push es
  // 00004F8A  1E                push ds
  // 00004F8B  07                pop es
  // 00004F8C  58                pop ax
  // 00004F8D  5E                pop si
  // 00004F8E  5F                pop di
  // 00004F8F  1E                push ds
  // 00004F90  8ED8              mov ds,ax
  // 00004F92  F3A5              rep movsw
  // 00004F94  13C9              adc cx,cx
  // 00004F96  F3A4              rep movsb
  // 00004F98  1F                pop ds
  // 00004F99  E8D0C8            call 0x186c
  // 00004F9C  E8A8D8            call 0x2847
  // 00004F9F  E8E4C8            call 0x1886
  // 00004FA2  5F                pop di
  // 00004FA3  5E                pop si
  // 00004FA4  C9                leave
  // 00004FA5  C3                ret
  // 00004EB5  55                push bp
  // 00004EB6  8BEC              mov bp,sp
  // 00004EB8  83EC16            sub sp,0x16
  // 00004EBB  56                push si
  // 00004EBC  57                push di
  // 00004EBD  8B7E04            mov di,[bp+0x4]
  // 00004EC0  8B4606            mov ax,[bp+0x6]
  // 00004EC3  8946EC            mov [bp-0x14],ax
  // 00004EC6  8B4608            mov ax,[bp+0x8]
  // 00004EC9  8946EE            mov [bp-0x12],ax
  // 00004ECC  57                push di
  // 00004ECD  16                push ss
  // 00004ECE  8D46F0            lea ax,[bp-0x10]
  // 00004ED1  50                push ax
  // 00004ED2  9AFFFF0000        call word 0x0:word 0xffff
  // 00004ED7  16                push ss
  // 00004ED8  8D46F0            lea ax,[bp-0x10]
  // 00004EDB  50                push ax
  // 00004EDC  FF76EE            push word [bp-0x12]
  // 00004EDF  FF76EC            push word [bp-0x14]
  // 00004EE2  9AFFFF0000        call word 0x0:word 0xffff
  // 00004EE7  0BC0              or ax,ax
  // 00004EE9  7503              jnz 0x4eee
  // 00004EEB  E9B400            jmp 0x4fa2
  // 00004EEE  57                push di
  // 00004EEF  9AFFFF0000        call word 0x0:word 0xffff
  // 00004EF4  8BF0              mov si,ax
  // 00004EF6  50                push ax
  // 00004EF7  9AFFFF0000        call word 0x0:word 0xffff
  // 00004EFC  8946EA            mov [bp-0x16],ax
  // 00004EFF  A1380D            mov ax,[0xd38]
  // 00004F02  8946FA            mov [bp-0x6],ax
  // 00004F05  8B46EC            mov ax,[bp-0x14]
  // 00004F08  BB1400            mov bx,0x14
  // 00004F0B  99                cwd
  // 00004F0C  F7FB              idiv bx
  // 00004F0E  8946FE            mov [bp-0x2],ax
  // 00004F11  8B46EE            mov ax,[bp-0x12]
  // 00004F14  99                cwd
  // 00004F15  F7FB              idiv bx
  // 00004F17  8946FC            mov [bp-0x4],ax
  // 00004F1A  8B46FE            mov ax,[bp-0x2]
  // 00004F1D  C1E004            shl ax,byte 0x4
  // 00004F20  0346FC            add ax,[bp-0x4]
  // 00004F23  8946F8            mov [bp-0x8],ax
  // 00004F26  BA1A00            mov dx,0x1a
  // 00004F29  F7EA              imul dx
  // 00004F2B  8BD8              mov bx,ax
  // 00004F2D  83BFBC0600        cmp word [bx+0x6bc],0x0
  // 00004F32  7424              jz 0x4f58
  // 00004F34  C706380DFFFF      mov word [0xd38],0xffff
  // 00004F3A  FF76EA            push word [bp-0x16]
  // 00004F3D  56                push si
  // 00004F3E  FF76FA            push word [bp-0x6]
  // 00004F41  E8F3FC            call 0x4c37
  // 00004F44  83C406            add sp,0x6
  // 00004F47  8B46F8            mov ax,[bp-0x8]
  // 00004F4A  A3380D            mov [0xd38],ax
  // 00004F4D  FF76EA            push word [bp-0x16]
  // 00004F50  56                push si
  // 00004F51  50                push ax
  // 00004F52  E8E2FC            call 0x4c37
  // 00004F55  83C406            add sp,0x6
  // 00004F58  FF76EA            push word [bp-0x16]
  // 00004F5B  9AFFFF0000        call word 0x0:word 0xffff
  // 00004F60  57                push di
  // 00004F61  56                push si
  // 00004F62  9AFFFF0000        call word 0x0:word 0xffff
  // 00004F67  A1380D            mov ax,[0xd38]
  // 00004F6A  BA1A00            mov dx,0x1a
  // 00004F6D  F7EA              imul dx
  // 00004F6F  05BE06            add ax,0x6be
  // 00004F72  BF5E2D            mov di,0x2d5e
  // 00004F75  FC                cld
  // 00004F76  1E                push ds
  // 00004F77  07                pop es
  // 00004F78  57                push di
  // 00004F79  8BF8              mov di,ax
  // 00004F7B  33C0              xor ax,ax
  // 00004F7D  B9FFFF            mov cx,0xffff
  // 00004F80  F2AE              repne scasb
  // 00004F82  F7D1              not cx
  // 00004F84  2BF9              sub di,cx
  // 00004F86  D1E9              shr cx,1
  // 00004F88  57                push di
  // 00004F89  06                push es
  // 00004F8A  1E                push ds
  // 00004F8B  07                pop es
  // 00004F8C  58                pop ax
  // 00004F8D  5E                pop si
  // 00004F8E  5F                pop di
  // 00004F8F  1E                push ds
  // 00004F90  8ED8              mov ds,ax
  // 00004F92  F3A5              rep movsw
  // 00004F94  13C9              adc cx,cx
  // 00004F96  F3A4              rep movsb
  // 00004F98  1F                pop ds
  // 00004F99  E8D0C8            call 0x186c
  // 00004F9C  E8A8D8            call 0x2847
  // 00004F9F  E8E4C8            call 0x1886
  // 00004FA2  5F                pop di
  // 00004FA3  5E                pop si
  // 00004FA4  C9                leave
  // 00004FA5  C3                ret
}

// TODO: This far callback function --GM
// ToolboxWndProc(), Ordinal 10
// 00004FA6  8CD8              mov ax,ds
// 00004FA8  90                nop
// 00004FA9  45                inc bp
// 00004FAA  55                push bp
// 00004FAB  8BEC              mov bp,sp
// 00004FAD  1E                push ds
// 00004FAE  8ED8              mov ds,ax
// 00004FB0  8B560E            mov dx,[bp+0xe]
// 00004FB3  8B5E0C            mov bx,[bp+0xc]
// 00004FB6  8B4E0A            mov cx,[bp+0xa]
// 00004FB9  8BC3              mov ax,bx
// 00004FBB  3D0F00            cmp ax,0xf
// 00004FBE  7428              jz 0x4fe8
// 00004FC0  770C              ja 0x4fce
// 00004FC2  3D0100            cmp ax,0x1
// 00004FC5  744E              jz 0x5015
// 00004FC7  3D0200            cmp ax,0x2
// 00004FCA  7421              jz 0x4fed
// 00004FCC  EB33              jmp 0x5001
// 00004FCE  3D0002            cmp ax,0x200
// 00004FD1  7423              jz 0x4ff6
// 00004FD3  3D0102            cmp ax,0x201
// 00004FD6  7529              jnz 0x5001
// 00004FD8  51                push cx
// 00004FD9  FF7608            push word [bp+0x8]
// 00004FDC  FF7606            push word [bp+0x6]
// 00004FDF  52                push dx
// 00004FE0  E8D2FE            call 0x4eb5
// 00004FE3  83C408            add sp,0x8
// 00004FE6  EB2D              jmp 0x5015
// 00004FE8  E887FE            call 0x4e72
// 00004FEB  EB28              jmp 0x5015
// 00004FED  6A00              push word 0x0
// 00004FEF  9AFFFF0000        call word 0x0:word 0xffff
// 00004FF4  EB1F              jmp 0x5015
// 00004FF6  FF36342D          push word [0x2d34]
// 00004FFA  9AFFFF0000        call word 0x0:word 0xffff
// 00004FFF  EB14              jmp 0x5015
// 00005001  52                push dx
// 00005002  53                push bx
// 00005003  51                push cx
// 00005004  FF7608            push word [bp+0x8]
// 00005007  FF7606            push word [bp+0x6]
// 0000500A  9AFFFF0000        call word 0x0:word 0xffff
// 0000500F  1F                pop ds
// 00005010  5D                pop bp
// 00005011  4D                dec bp
// 00005012  CA0A00            retf word 0xa
// 00005015  33D2              xor dx,dx
// 00005017  33C0              xor ax,ax
// 00005019  1F                pop ds
// 0000501A  5D                pop bp
// 0000501B  4D                dec bp
// 0000501C  CA0A00            retf word 0xa

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
