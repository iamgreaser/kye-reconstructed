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
void cs_4BCE(int idx, HWND hWnd) {
  int cx; // [BP-0x2].w
  int cy; // [BP-0x4].w
  // [BP-0x6].w is a spilled value
  // [BP-0x8].w is a spilled value

  cx = idx/16;
  cy = idx%16;

  SelectObject(hWnd, g_hpenWhite);
  SelectObject(hWnd, g_hbrWhite);
  Rectangle(hWnd,
    cx*(TILE_LX+(2*2))+2,
    cy*(TILE_LY+(2*2))+2,
    cx*(TILE_LX+(2*2))+TILE_LX-2,
    cy*(TILE_LY+(2*2))+TILE_LY-2);
}

// CS:4C37
void cs_4C37(int a0, int a1, int a2) {
  // TODO! --GM
  // 00004C37  55                push bp
  // 00004C38  8BEC              mov bp,sp
  // 00004C3A  83EC14            sub sp,0x14
  // 00004C3D  56                push si
  // 00004C3E  57                push di
  // 00004C3F  8B4604            mov ax,[bp+0x4]
  // 00004C42  BB1000            mov bx,0x10
  // 00004C45  99                cwd
  // 00004C46  F7FB              idiv bx
  // 00004C48  8946F6            mov [bp-0xa],ax
  // 00004C4B  8B4604            mov ax,[bp+0x4]
  // 00004C4E  99                cwd
  // 00004C4F  F7FB              idiv bx
  // 00004C51  8956F8            mov [bp-0x8],dx
  // 00004C54  8B46F6            mov ax,[bp-0xa]
  // 00004C57  BA1400            mov dx,0x14
  // 00004C5A  F7EA              imul dx
  // 00004C5C  8BD8              mov bx,ax
  // 00004C5E  050200            add ax,0x2
  // 00004C61  8946FC            mov [bp-0x4],ax
  // 00004C64  8B46F8            mov ax,[bp-0x8]
  // 00004C67  BA1400            mov dx,0x14
  // 00004C6A  F7EA              imul dx
  // 00004C6C  8BD0              mov dx,ax
  // 00004C6E  050200            add ax,0x2
  // 00004C71  8946FA            mov [bp-0x6],ax
  // 00004C74  8B4604            mov ax,[bp+0x4]
  // 00004C77  3B06380D          cmp ax,[0xd38]
  // 00004C7B  7508              jnz 0x4c85
  // 00004C7D  A1642B            mov ax,[0x2b64]
  // 00004C80  8946EC            mov [bp-0x14],ax
  // 00004C83  EB06              jmp 0x4c8b
  // 00004C85  A1622B            mov ax,[0x2b62]
  // 00004C88  8946EC            mov [bp-0x14],ax
  // 00004C8B  16                push ss
  // 00004C8C  8D46EE            lea ax,[bp-0x12]
  // 00004C8F  50                push ax
  // 00004C90  53                push bx
  // 00004C91  52                push dx
  // 00004C92  8BC3              mov ax,bx
  // 00004C94  051400            add ax,0x14
  // 00004C97  50                push ax
  // 00004C98  8BC2              mov ax,dx
  // 00004C9A  051400            add ax,0x14
  // 00004C9D  50                push ax
  // 00004C9E  9AFFFF0000        call word 0x0:word 0xffff
  // 00004CA3  FF7606            push word [bp+0x6]
  // 00004CA6  16                push ss
  // 00004CA7  8D46EE            lea ax,[bp-0x12]
  // 00004CAA  50                push ax
  // 00004CAB  FF76EC            push word [bp-0x14]
  // 00004CAE  9AFFFF0000        call word 0x0:word 0xffff
  // 00004CB3  16                push ss
  // 00004CB4  8D46EE            lea ax,[bp-0x12]
  // 00004CB7  50                push ax
  // 00004CB8  6AFF              push word 0xffffffffffffffff
  // 00004CBA  6AFF              push word 0xffffffffffffffff
  // 00004CBC  9AFFFF0000        call word 0x0:word 0xffff
  // 00004CC1  FF7606            push word [bp+0x6]
  // 00004CC4  16                push ss
  // 00004CC5  8D46EE            lea ax,[bp-0x12]
  // 00004CC8  50                push ax
  // 00004CC9  FF76EC            push word [bp-0x14]
  // 00004CCC  9AFFFF0000        call word 0x0:word 0xffff
  // 00004CD1  8B4604            mov ax,[bp+0x4]
  // 00004CD4  BA1A00            mov dx,0x1a
  // 00004CD7  F7EA              imul dx
  // 00004CD9  8BD0              mov dx,ax
  // 00004CDB  8BD8              mov bx,ax
  // 00004CDD  83BFBC0600        cmp word [bx+0x6bc],0x0
  // 00004CE2  7510              jnz 0x4cf4
  // 00004CE4  FF7606            push word [bp+0x6]
  // 00004CE7  FF7604            push word [bp+0x4]
  // 00004CEA  E8E1FE            call 0x4bce
  // 00004CED  83C404            add sp,0x4
  // 00004CF0  5F                pop di
  // 00004CF1  5E                pop si
  // 00004CF2  C9                leave
  // 00004CF3  C3                ret
  // 00004CF4  8BDA              mov bx,dx
  // 00004CF6  8B87B806          mov ax,[bx+0x6b8]
  // 00004CFA  8946FE            mov [bp-0x2],ax
  // 00004CFD  8B97BA06          mov dx,[bx+0x6ba]
  // 00004D01  8B5EFE            mov bx,[bp-0x2]
  // 00004D04  83FB03            cmp bx,0x3
  // 00004D07  7603              jna 0x4d0c
  // 00004D09  E92701            jmp 0x4e33
  // 00004D0C  D1E3              shl bx,1
  // 00004D0E  2EFFA7374E        jmp word near [cs:bx+0x4e37]
  // 00004D13  FF7606            push word [bp+0x6]
  // 00004D16  FF7604            push word [bp+0x4]
  // 00004D19  E8B2FE            call 0x4bce
  // 00004D1C  83C404            add sp,0x4
  // 00004D1F  5F                pop di
  // 00004D20  5E                pop si
  // 00004D21  C9                leave
  // 00004D22  C3                ret
  // 00004D23  83FAFD            cmp dx,0xfffffffffffffffd
  // 00004D26  7F22              jg 0x4d4a
  // 00004D28  83FAF5            cmp dx,0xfffffffffffffff5
  // 00004D2B  7C1D              jl 0x4d4a
  // 00004D2D  8BC2              mov ax,dx
  // 00004D2F  99                cwd
  // 00004D30  33C2              xor ax,dx
  // 00004D32  2BC2              sub ax,dx
  // 00004D34  50                push ax
  // 00004D35  B8FDFF            mov ax,0xfffd
  // 00004D38  99                cwd
  // 00004D39  33C2              xor ax,dx
  // 00004D3B  2BC2              sub ax,dx
  // 00004D3D  5E                pop si
  // 00004D3E  2BF0              sub si,ax
  // 00004D40  C1E604            shl si,byte 0x4
  // 00004D43  83C630            add si,0x30
  // 00004D46  33FF              xor di,di
  // 00004D48  EB37              jmp 0x4d81
  // 00004D4A  83FAF4            cmp dx,0xfffffffffffffff4
  // 00004D4D  7506              jnz 0x4d55
  // 00004D4F  33F6              xor si,si
  // 00004D51  33FF              xor di,di
  // 00004D53  EB2C              jmp 0x4d81
  // 00004D55  83FAF3            cmp dx,0xfffffffffffffff3
  // 00004D58  7507              jnz 0x4d61
  // 00004D5A  BEC000            mov si,0xc0
  // 00004D5D  33FF              xor di,di
  // 00004D5F  EB20              jmp 0x4d81
  // 00004D61  83FAF2            cmp dx,0xfffffffffffffff2
  // 00004D64  7405              jz 0x4d6b
  // 00004D66  83FAF1            cmp dx,0xfffffffffffffff1
  // 00004D69  7507              jnz 0x4d72
  // 00004D6B  BEE000            mov si,0xe0
  // 00004D6E  33FF              xor di,di
  // 00004D70  EB0F              jmp 0x4d81
  // 00004D72  83FAF0            cmp dx,0xfffffffffffffff0
  // 00004D75  7405              jz 0x4d7c
  // 00004D77  83FAEF            cmp dx,0xffffffffffffffef
  // 00004D7A  7505              jnz 0x4d81
  // 00004D7C  BED000            mov si,0xd0
  // 00004D7F  33FF              xor di,di
  // 00004D81  FF7608            push word [bp+0x8]
  // 00004D84  FF364E2B          push word [0x2b4e]
  // 00004D88  9AFFFF0000        call word 0x0:word 0xffff
  // 00004D8D  FF7606            push word [bp+0x6]
  // 00004D90  FF76FC            push word [bp-0x4]
  // 00004D93  FF76FA            push word [bp-0x6]
  // 00004D96  6A10              push word 0x10
  // 00004D98  6A10              push word 0x10
  // 00004D9A  FF7608            push word [bp+0x8]
  // 00004D9D  56                push si
  // 00004D9E  57                push di
  // 00004D9F  68CC00            push word 0xcc
  // 00004DA2  6A20              push word 0x20
  // 00004DA4  9AFFFF0000        call word 0x0:word 0xffff
  // 00004DA9  5F                pop di
  // 00004DAA  5E                pop si
  // 00004DAB  C9                leave
  // 00004DAC  C3                ret
  // 00004DAD  83FA17            cmp dx,0x17
  // 00004DB0  7C0E              jl 0x4dc0
  // 00004DB2  8BF2              mov si,dx
  // 00004DB4  C1E604            shl si,byte 0x4
  // 00004DB7  81C690FE          add si,0xfe90
  // 00004DBB  BF4000            mov di,0x40
  // 00004DBE  EB07              jmp 0x4dc7
  // 00004DC0  8BF2              mov si,dx
  // 00004DC2  C1E604            shl si,byte 0x4
  // 00004DC5  33FF              xor di,di
  // 00004DC7  83FA32            cmp dx,0x32
  // 00004DCA  7C11              jl 0x4ddd
  // 00004DCC  83FA3B            cmp dx,0x3b
  // 00004DCF  7F0C              jg 0x4ddd
  // 00004DD1  8BF2              mov si,dx
  // 00004DD3  C1E604            shl si,byte 0x4
  // 00004DD6  81C6E0FC          add si,0xfce0
  // 00004DDA  BF1000            mov di,0x10
  // 00004DDD  FF7608            push word [bp+0x8]
  // 00004DE0  FF36502B          push word [0x2b50]
  // 00004DE4  9AFFFF0000        call word 0x0:word 0xffff
  // 00004DE9  FF7606            push word [bp+0x6]
  // 00004DEC  FF76FC            push word [bp-0x4]
  // 00004DEF  FF76FA            push word [bp-0x6]
  // 00004DF2  6A10              push word 0x10
  // 00004DF4  6A10              push word 0x10
  // 00004DF6  FF7608            push word [bp+0x8]
  // 00004DF9  56                push si
  // 00004DFA  57                push di
  // 00004DFB  68CC00            push word 0xcc
  // 00004DFE  6A20              push word 0x20
  // 00004E00  9AFFFF0000        call word 0x0:word 0xffff
  // 00004E05  5F                pop di
  // 00004E06  5E                pop si
  // 00004E07  C9                leave
  // 00004E08  C3                ret
  // 00004E09  FF7608            push word [bp+0x8]
  // 00004E0C  FF36522B          push word [0x2b52]
  // 00004E10  9AFFFF0000        call word 0x0:word 0xffff
  // 00004E15  FF7606            push word [bp+0x6]
  // 00004E18  FF76FC            push word [bp-0x4]
  // 00004E1B  FF76FA            push word [bp-0x6]
  // 00004E1E  6A10              push word 0x10
  // 00004E20  6A10              push word 0x10
  // 00004E22  FF7608            push word [bp+0x8]
  // 00004E25  6A00              push word 0x0
  // 00004E27  6A00              push word 0x0
  // 00004E29  68CC00            push word 0xcc
  // 00004E2C  6A20              push word 0x20
  // 00004E2E  9AFFFF0000        call word 0x0:word 0xffff
  // 00004E33  5F                pop di
  // 00004E34  5E                pop si
  // 00004E35  C9                leave
  // 00004E36  C3                ret
}
// A 3-entry branch table follows.

// CS:4E3F
void cs_4E3F(int a0) {
  // TODO! --GM
  // 00004E3F  55                push bp
  // 00004E40  8BEC              mov bp,sp
  // 00004E42  83EC02            sub sp,0x2
  // 00004E45  56                push si
  // 00004E46  57                push di
  // 00004E47  8B7E04            mov di,[bp+0x4]
  // 00004E4A  57                push di
  // 00004E4B  9AFFFF0000        call word 0x0:word 0xffff
  // 00004E50  8946FE            mov [bp-0x2],ax
  // 00004E53  33F6              xor si,si
  // 00004E55  FF76FE            push word [bp-0x2]
  // 00004E58  57                push di
  // 00004E59  56                push si
  // 00004E5A  E8DAFD            call 0x4c37
  // 00004E5D  83C406            add sp,0x6
  // 00004E60  46                inc si
  // 00004E61  83FE40            cmp si,0x40
  // 00004E64  7CEF              jl 0x4e55
  // 00004E66  FF76FE            push word [bp-0x2]
  // 00004E69  9AFFFF0000        call word 0x0:word 0xffff
  // 00004E6E  5F                pop di
  // 00004E6F  5E                pop si
  // 00004E70  C9                leave
  // 00004E71  C3                ret
}

// CS:4E72
void cs_4E72(void) {
  // TODO! --GM
  // 00004E72  55                push bp
  // 00004E73  8BEC              mov bp,sp
  // 00004E75  83EC20            sub sp,0x20
  // 00004E78  56                push si
  // 00004E79  FF36902E          push word [0x2e90]
  // 00004E7D  16                push ss
  // 00004E7E  8D46E0            lea ax,[bp-0x20]
  // 00004E81  50                push ax
  // 00004E82  9AFFFF0000        call word 0x0:word 0xffff
  // BeginPaint();

  // 00004E87  8BF0              mov si,ax
  // 00004E89  50                push ax
  // 00004E8A  FF36562B          push word [0x2b56]
  // 00004E8E  9AFFFF0000        call word 0x0:word 0xffff
  // SelectObject();

  // 00004E93  56                push si
  // 00004E94  E8A8FF            call 0x4e3f
  // 00004E97  83C402            add sp,0x2
  // 00004E9A  FF36902E          push word [0x2e90]
  // 00004E9E  56                push si
  // 00004E9F  9AFFFF0000        call word 0x0:word 0xffff
  // ReleaseDC();

  // 00004EA4  FF36902E          push word [0x2e90]
  // 00004EA8  16                push ss
  // 00004EA9  8D46E0            lea ax,[bp-0x20]
  // 00004EAC  50                push ax
  // 00004EAD  9AFFFF0000        call word 0x0:word 0xffff
  // EndPaint();

  // 00004EB2  5E                pop si
  // 00004EB3  C9                leave
  // 00004EB4  C3                ret
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
