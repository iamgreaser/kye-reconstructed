// vim: set syntax=cpp :

#include <windows.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include "kyetypes.h"
#include "kyeprocs.h"
#include "kyevars.h"

#include "resdefs.h"

int unused_0054[4] = {-1, -1, -1, -1}; // DS:0054
int g_mouse_captured; // DS:005C
int g_key_pressed; // DS:005E

// CS:016A - ***CODE MATCH!***
// requirements, somewhat: -k- -G -O2 -1 -W -ms
void repaint_main_playfield(void) {
  switch (g_is_in_editor) {
  case 0:
    repaint_all_walls();
    repaint_all_actors();
    if (g_kye_fizzle_in) {
      g_kye_fizzle_in = 0;
      repaint_kye(1);
    } else {
      repaint_kye(0);
    }
    return;
  case 1:
    repaint_all_walls();
    repaint_all_actors();
    repaint_kye(0);
    break;
  }
}

// CS:01AB - ***CODE MATCH!***
void actual_main_paint(void) {
  int statusmsg_slen;

  g_saved_hpen = SelectObject(g_maindc, g_hpenBlack);
  MoveTo(g_maindc, g_r_status_left.left, g_r_status_right.top-1);
  LineTo(g_maindc, g_r_status_right.right, g_r_status_right.top-1);
  MoveTo(g_maindc, g_r_status_left.right+1, g_r_status_left.top);
  LineTo(g_maindc, g_r_status_left.right+1, g_r_status_left.bottom+2);

  repaint_status_left();

  SelectObject(g_maindc, g_hpenWhite);
  statusmsg_slen = strlen(g_statusmsg);
  Rectangle(
    g_maindc,
    g_r_status_right.left,
    g_r_status_right.top,
    g_r_status_right.right,
    g_r_status_right.bottom);
  SelectObject(g_maindc, g_hpenBlack);
  TextOut(g_maindc,
    g_r_status_right.left+4,
    g_r_status_right.top,
    g_statusmsg, statusmsg_slen);

  SelectObject(g_maindc, g_saved_hpen);

  repaint_main_playfield();
}

// CS:0284 - ***CODE MATCH!***
void tick_game(void) {
  g_has_maintimer = 0;
  acquire_main_dc();

  if (g_kye_needs_update != 0) {
    tick_kye();
  }

  if (g_level_complete != 0) {
    if (g_levelidx >= g_levelcount) {
      show_last_level_ending();
      g_levelidx = 1;
    } else {
      show_level_done();
      g_levelidx = g_levelidx + 1;
    }
    load_level(g_levelidx);
    show_next_level_name();
    g_moving_by_mouse = 0;
    set_status_msg(g_level_hint);
    InvalidateRect(g_mainwnd, NULL, TRUE);
    UpdateWindow(g_mainwnd);
  } else {
    tick_game_state();
    if (g_kye_lives < 0) {
      repaint_kye(2);
      delete_and_draw_unacting_tile(g_kye_main_cx, g_kye_main_cy);
      show_game_over();
      g_moving_by_mouse = 0;
      load_level(g_levelidx);
      set_status_msg(g_level_hint);
      InvalidateRect(g_mainwnd, NULL, TRUE);
      UpdateWindow(g_mainwnd);
    } else if (g_kye_injured != 0) {
      tick_kye();
    }
  }

  release_main_dc();
  g_has_maintimer = 1;
}

// CS:035C cdecl near - ***CODE MATCH!***
void main_paint(HWND hWnd) {
  if (g_has_maindc != 0) {
    release_main_dc();
  }
  // BUG: BeginPaint result not checked for null handle
  g_maindc = BeginPaint(hWnd, &g_paint);
  g_has_maindc = 1;
  compute_window_geometry();
  actual_main_paint();
  ReleaseDC(hWnd, g_maindc);
  g_has_maindc = 0;
  EndPaint(hWnd, &g_paint);
}

// CS:03A3 - ***CODE MATCH!***
void main_wm_lbuttondown(HWND hWnd, LONG lParam, WORD wParam) {
  int x; // SI
  int y; // DI
  x = LOWORD(lParam);
  y = HIWORD(lParam);
  if (mouse_is_in_playfield(LOWORD(lParam), HIWORD(lParam))) {
    switch (g_is_in_editor) {
    case 0:
      tick_game();
      x = x / g_tile_lx;
      y = y / g_tile_ly;
      update_kye_mouse_target(x, y);
      g_kye_target_cx = x;
      g_kye_target_cy = y;
      g_kye_needs_update = 1;
      g_moving_by_mouse = 1;
      break;
    case 1:
      x = x / g_tile_lx;
      y = y / g_tile_ly;
      cs_4B7A(x, y);
      acquire_main_dc();
      paint_tile_in_editor(x, y);
      repaint_status_left();
      release_main_dc();
      break;
    }
  }
}

// CS:0432 - ***CODE MATCH!***
void main_wm_lbuttonup(HWND hWnd, LONG lParam) {
  if (g_mouse_captured) {
    ReleaseCapture();
    g_mouse_captured = 0;
  }

  switch (g_is_in_editor) {
  case 0:
    g_kye_needs_update = 0;
    g_moving_by_mouse = 0;
    clear_kye_mouse_ghost();
    break;
  }
}

// CS:045F - ***CODE MATCH!***
void main_wm_lbuttondblclick(HWND hWnd, LONG lParam, WORD wParam) {
  // VESTIGAL BUG: This does nothing but acquires and releases g_maindc.
  if (mouse_is_in_playfield(LOWORD(lParam), HIWORD(lParam))) {
    switch(g_is_in_editor) {
    case 0:
      acquire_main_dc();
      release_main_dc();
      break;
    }
  }
}

// CS:0481 - ***CODE MATCH!***
void main_wm_rbuttondown(HWND hWnd, LONG lParam, WORD wParam) {
  // stack: 0x04 bytes
  int px; // BP/FE
  int py; // BP/FC

  int cx; // SI
  int cy; // DI

  px = LOWORD(lParam);
  py = HIWORD(lParam);
  if (mouse_is_in_playfield(LOWORD(lParam), HIWORD(lParam))) {
    switch (g_is_in_editor) {
    case 0:
      if (g_moving_by_mouse) {
        g_moving_by_mouse = 0;
        clear_kye_mouse_ghost();
      } else {
        cx = px / g_tile_lx;
        cy = py / g_tile_ly;
        update_kye_mouse_target(cx, cy);
        g_kye_target_cx = cx;
        g_kye_target_cy = cy;
        g_kye_needs_update = 1;
        g_moving_by_mouse = 1;
      }
      break;
    }
  }
}

// CS:04F2 - ***CODE MATCH!***
void main_wm_mousemove(HWND hWnd, LONG lParam) {
  // stack: 0x04 bytes
  int px; // BP/FE
  int py; // BP/FC

  int cx; // SI
  int cy; // DI

  px = LOWORD(lParam);
  py = HIWORD(lParam);
  if (mouse_is_in_playfield(LOWORD(lParam), HIWORD(lParam))) {
    switch (g_is_in_editor) {
    case 0:
      SetCursor(g_hcArrow);
      cx = px / g_tile_lx;
      cy = py / g_tile_ly;
      if (g_moving_by_mouse) {
        update_kye_mouse_target(cx, cy);
        g_kye_target_cx = cx;
        g_kye_target_cy = cy;
        g_kye_needs_update = 1;
      }
      break;
    default:
      SetCursor(g_hcArrow);
      break;
    }
  } else {
    SetCursor(g_hcArrow);
  }
}

// CS:0573 - ***CODE MATCH!***
// Matching requires -a to NOT be set...?! Otherwise the switch jump table becomes misaligned. Yep, it's a compiler bug!
void on_key_press(HWND hWnd, WORD wParam) {
  (void)hWnd;

  if (g_is_in_editor != 0) return;

  // Not sure why this exists, but it seems to be required for a match.
  if(wParam != VK_LEFT && wParam != VK_RIGHT
      && wParam != VK_UP && wParam != VK_DOWN
      && wParam != VK_HOME && wParam != VK_END
      && wParam != VK_PRIOR && wParam != VK_NEXT) {
    return;
  }

  switch(wParam) {
  case VK_LEFT:
    g_kye_target_cx = g_kye_main_cx - 1;
    g_kye_target_cy = g_kye_main_cy;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    break;
  case VK_RIGHT:
    g_kye_target_cx = g_kye_main_cx + 1;
    g_kye_target_cy = g_kye_main_cy;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    break;
  case VK_UP:
    g_kye_target_cx = g_kye_main_cx;
    g_kye_target_cy = g_kye_main_cy - 1;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    break;
  case VK_DOWN:
    g_kye_target_cx = g_kye_main_cx;
    g_kye_target_cy = g_kye_main_cy + 1;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    break;
  case VK_HOME:
    g_kye_target_cx = g_kye_main_cx - 1;
    g_kye_target_cy = g_kye_main_cy - 1;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    break;
  case VK_END:
    g_kye_target_cx = g_kye_main_cx - 1;
    g_kye_target_cy = g_kye_main_cy + 1;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    break;
  case VK_PRIOR:
    g_kye_target_cx = g_kye_main_cx + 1;
    g_kye_target_cy = g_kye_main_cy - 1;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    break;
  case VK_NEXT:
    g_kye_target_cx = g_kye_main_cx + 1;
    g_kye_target_cy = g_kye_main_cy + 1;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    break;
  }
}

// CS:06AC - ***CODE MATCH!***
void update_edit_menu_item(void) {
  HMENU menu; // DX

  menu = GetMenu(g_mainwnd);
  if (g_levelcount == 1) {
    EnableMenuItem(menu, IDC_LEVEL_EDIT, MF_ENABLED);
    return;
  }
  EnableMenuItem(menu, IDC_LEVEL_EDIT, MF_GRAYED);
}

// CS:06D6 - ***CODE MATCH!***
int main_wm_command(HWND hWnd, WORD wParam) {
  // Needs 0x50 bytes of stack (BP/B0)
  char textbuf[80]; // BP/B0

  HMENU menu; // SI
  int selected_level; // DX

  switch (wParam) {
  case IDC_GAME_EXIT:
    PostQuitMessage(0);
    break;

  case IDC_GAME_NEW:
    g_levelidx = 1;
    load_level(1);
    g_moving_by_mouse = 0;
    set_status_msg(g_level_hint);
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    break;

  case IDC_LEVEL_RESTART:
    if (g_is_in_editor == 0) {
      load_level(g_levelidx);
      set_status_msg(g_level_hint);
      g_moving_by_mouse = 0;
      InvalidateRect(hWnd, NULL, TRUE);
      UpdateWindow(hWnd);
    } else {
      // Actually "Abort edit"
      g_is_in_editor = 0;
      cs_5119();
      menu = GetMenu(g_mainwnd);
      ModifyMenu(menu, IDC_LEVEL_EDIT, MF_BYCOMMAND, IDC_LEVEL_EDIT, "&Edit"); // DS:0060
      ModifyMenu(menu, IDC_LEVEL_GOTO, MF_BYCOMMAND, IDC_LEVEL_GOTO, "&Goto level..."); // DS:0066
      ModifyMenu(menu, IDC_LEVEL_RESTART, MF_BYCOMMAND, IDC_LEVEL_RESTART, "&Restart"); // DS:0075
      ModifyMenu(menu, IDC_LEVEL_FILE, MF_BYCOMMAND, IDC_LEVEL_FILE, "&File..."); // DS:007E
      EnableMenuItem(menu, IDC_GAME_NEW, MF_BYCOMMAND|MF_ENABLED);
      DrawMenuBar(hWnd);
      load_level(g_levelidx);
      set_status_msg(g_level_hint);
      g_moving_by_mouse = 0;
      g_has_maintimer = 1;
      InvalidateRect(hWnd, NULL, TRUE);
      UpdateWindow(hWnd);
      update_window_title();
    }
    break;

  case IDC_LEVEL_GOTO:
    if (g_is_in_editor == 0) {
      g_has_maintimer = 0;
      strcpy(g_dialog_text, ""); // DS:0087
      show_level_name_input();
      strupr(g_dialog_text);
      selected_level = find_level_by_name(g_dialog_text);
      if (selected_level > 0) {
        g_levelidx = selected_level;
        load_level(selected_level);
        g_moving_by_mouse = 0;
        set_status_msg(g_level_hint);
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
      } else {
        if (strlen(g_dialog_text) > 0) {
          msgbox_error("Bad luck", // DS:0088
            "No such level"); // DS:0091
        }
      }
      g_has_maintimer = 1;

    } else {
      // Actually "Edit Hint"
      strcpy(textbuf, g_level_hint);
      switch (show_text_input_dialog("Edit the hint text:", textbuf)) { // DS:009F
      case 0:
        strcpy(g_level_hint, textbuf);
        set_status_msg(g_level_hint);
        break;
      }
    }
    break;

  case IDC_LEVEL_FILE:
    if (g_is_in_editor == 0) {
      cs_2F89(g_levelset_fname);
      load_level(g_levelidx);
      set_status_msg(g_level_hint);
      update_edit_menu_item();
      g_moving_by_mouse = 0;
      InvalidateRect(hWnd, NULL, TRUE);
      UpdateWindow(hWnd);
      update_window_title();
    } else {
      // Actually "Edit Name"
      strcpy(textbuf, g_level_name);
      switch (show_text_input_dialog("Edit the Level Name:", textbuf)) { // DS:00B3
      case 0:
        if (strlen(textbuf) < 20) {
          strupr(textbuf);
          strcpy(g_level_name, textbuf);
        }
        break;
      }
    }
    break;

  case IDC_LEVEL_EDIT:
    if (g_is_in_editor == 0) {
      g_levelidx = 1;
      load_level(1);
      if (g_levelcount > 1) {
        msgbox_error("Cannot edit multi-level files.", // DS:00C8
            "Load a single level file.(eg. template.kye)"); // DS:00E7
      } else {
        g_is_in_editor = 1;
        g_has_maintimer = 0;
        cs_501F();
        menu = GetMenu(g_mainwnd);
        ModifyMenu(menu, IDC_LEVEL_EDIT, MF_BYCOMMAND, IDC_LEVEL_EDIT, "&Run"); // DS:0113
        ModifyMenu(menu, IDC_LEVEL_RESTART, MF_BYCOMMAND, IDC_LEVEL_RESTART, "&Abort Edit"); // DS:0118
        ModifyMenu(menu, IDC_LEVEL_GOTO, MF_BYCOMMAND, IDC_LEVEL_GOTO, "Edit &Hint"); // DS:0124
        ModifyMenu(menu, IDC_LEVEL_FILE, MF_BYCOMMAND, IDC_LEVEL_FILE, "Edit &Name"); // DS:012F
        EnableMenuItem(menu, IDC_GAME_NEW, MF_BYCOMMAND|MF_GRAYED);
        DrawMenuBar(hWnd);
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        update_window_title();
      }
    } else {
      g_is_in_editor = 0;
      cs_5119();
      menu = GetMenu(g_mainwnd);
      ModifyMenu(menu, IDC_LEVEL_EDIT, MF_BYCOMMAND, IDC_LEVEL_EDIT, "&Edit"); // DS:013A
      ModifyMenu(menu, IDC_LEVEL_GOTO, MF_BYCOMMAND, IDC_LEVEL_GOTO, "&Goto level..."); // DS:0140
      ModifyMenu(menu, IDC_LEVEL_FILE, MF_BYCOMMAND, IDC_LEVEL_FILE, "&File..."); // DS:014F
      ModifyMenu(menu, IDC_LEVEL_RESTART, MF_BYCOMMAND, IDC_LEVEL_RESTART, "&Restart"); // DS:0158
      EnableMenuItem(menu, IDC_GAME_NEW, MF_BYCOMMAND|MF_ENABLED);
      DrawMenuBar(hWnd);
      save_single_level_file();
      load_level(g_levelidx);
      set_status_msg(g_level_hint);
      g_moving_by_mouse = 0;
      g_has_maintimer = 1;
      InvalidateRect(hWnd, NULL, TRUE);
      UpdateWindow(hWnd);
      update_window_title();
    }
    break;

  case IDC_301:
    // Unused, not defined. Could this be sound? Who knows...
    menu = GetMenu(g_mainwnd);
    if (g_02BC != 0) {
      CheckMenuItem(menu, IDC_301, MF_BYCOMMAND | MF_UNCHECKED);
      g_02BC = 0;
    } else {
      CheckMenuItem(menu, IDC_301, MF_BYCOMMAND | MF_CHECKED);
      g_02BC = 1;
    }
    break;

  case IDC_HELP_THE_GAME:
    // Windows 3.1 calls it HELP_CONTENTS, but Windows 3.0 calls it HELP_INDEX.
    WinHelp(g_mainwnd, "kyehelp.hlp", HELP_INDEX, 0); // DS:0161
    break;

  case IDC_HELP_USING_HELP:
    WinHelp(g_mainwnd, "", HELP_HELPONHELP, 0); // DS:016D
    break;

  case IDC_HELP_ABOUT:
    show_charity_dialog();
    break;

  default:
    return 0;
  }
  return 1;
}

// CS:0C19 - ***CODE MATCH!***
LONG FAR PASCAL _export WndProc(HWND hWnd, WORD msg, WORD wParam, LONG lParam) {
  // The compiler does some funky less-than/greater-than optimisation,
  // instead of using a LUT.
  // Maybe it was aiming to work nicely on a 486?
  // That was a thing when Win 3.0 came out.

  switch(msg) {
  case WM_COMMAND:
    return main_wm_command(hWnd, wParam);
  case WM_LBUTTONDOWN:
    main_wm_lbuttondown(hWnd, lParam, wParam);
    break;
  case WM_LBUTTONUP:
    main_wm_lbuttonup(hWnd, lParam);
    break;
  case WM_LBUTTONDBLCLK:
    main_wm_lbuttondblclick(hWnd, lParam, wParam);
    break;
  case WM_RBUTTONDOWN:
    main_wm_rbuttondown(hWnd, lParam, wParam);
    break;
  case WM_MOUSEMOVE:
    main_wm_mousemove(hWnd, lParam);
    break;
  case WM_KEYDOWN:
    g_key_pressed = 1;
    on_key_press(hWnd, wParam);
    break;
  case WM_KEYUP:
    g_key_pressed = 0;
    break;
  case WM_PAINT:
    main_paint(hWnd);
    break;
  case WM_DESTROY:
    deinit_brushes_and_stuff();
    PostQuitMessage(0);
    break;
  case WM_TIMER:
    // BUG: (?) Handling of this flag is dubious.
    if (g_has_maintimer) {
      tick_game();
      animate_everything();
      g_has_maintimer = 1;
    }
    break;

  case WM_SIZE:
    return DefWindowProc(hWnd, msg, wParam, lParam);
  case WM_CREATE:
    break;
  default:
    return DefWindowProc(hWnd, msg, wParam, lParam);
  }
  return 0;
}

// CS:0D6A - Near-match, but one of the registers is incorrect.
int PASCAL _export WinMain(HANDLE hInstance,
                           HANDLE hPrevInstance,
                           LPSTR lpCmdLine,
                           int nCmdShow) {
  // needs 0x60 (96) extra bytes on stack
  HWND mainwnd; // /FE BP-0x02
  WNDCLASS maincls; // /E4 BP-0x1C
  MSG msg; // /D2 BP-0x2A
  char cmdlinebuf[50]; // /A0 BP-0x60

  int i; // SI - MISMATCH FIXME: Seems to be swapped with DI right now? --GM


  for (i = 0; lpCmdLine[i] != 0 && i < 50; i++) {
    cmdlinebuf[i] = lpCmdLine[i];
  }
  // BUG: Clobbers past buffer on edge case
  // Ends up being moot in practice,
  // as the byte in question is a MSG,
  // which gets written before we next read it.
  // But still a bug.
  cmdlinebuf[i] = 0;

  if (!hPrevInstance) {
    maincls.style = CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS; // 0x000B
    maincls.lpfnWndProc = WndProc;
    maincls.cbClsExtra = 0;
    maincls.cbWndExtra = 0;
    maincls.hInstance = hInstance;
    maincls.hIcon = LoadIcon(hInstance, "IDI_KYE"); // DS:016E
    maincls.hCursor = NULL;
    maincls.hbrBackground = GetStockObject(WHITE_BRUSH); // 0x0000
    maincls.lpszMenuName = "Kye"; // DS:0176
    maincls.lpszClassName = "Kye"; // DS:017A
    if (!RegisterClass(&maincls)) {
      // BUG: Returns success on failure
      return 0;
    }
  }
  mainwnd = CreateWindow(
    "Kye", // DS:017E
    "Kye", // DS:0182
    WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU|WS_SYSMENU, // 0x00CA0000
    50, // CW_USEDEFAULT would be better of course
    50, // Ditto... and yes, it *is* in Windows 3.0.
    // BUG: Different-sized window borders break things.
    // - Windows 3.1 JP edition has a bad time with this.
    // - Wine with large fonts completely gobbles the status bar.
    //   - Speaking of Wine, 16-bit code runs really nicely on 64-bit OSes.
    //     - Hey Microsoft, have you tried not sucking? No? OK then.
    1+480+1, // 482
    320+59,  // 379 - TODO work out status bar + NC geometry --GM
    NULL,
    NULL,
    hInstance,
    NULL);
  if (!mainwnd) {
    // BUG: Returns success on failure
    return 0;
  }
  g_hInstance = hInstance;
  g_mainwnd = mainwnd;
  g_has_maindc = 0;
  g_is_in_editor = 0;
  g_mouse_captured = 0;

  g_hcArrow = LoadCursor(NULL, IDC_ARROW);
  g_hcCross = LoadCursor(NULL, IDC_CROSS);

  update_window_title();
  ShowWindow(mainwnd, nCmdShow);

  init_brushes_and_stuff();
  acquire_main_dc();
  compute_window_geometry();
  release_main_dc();

  srand(time(NULL));

  init_timer();
  OpenSound(); // SOUND by ordinal, Ordinal 1?
  g_tick_counter = 0;
  clear_level();

  if (!registration_check(cmdlinebuf)) {
    strcpy(g_levelset_fname, "border.kye"); // DS:0186
    load_level(g_levelidx);
    set_status_msg(g_level_hint);
    update_edit_menu_item();
    g_moving_by_mouse = 0;
    InvalidateRect(mainwnd, NULL, TRUE);
    UpdateWindow(mainwnd);
    show_charity_dialog();
  }

  strcpy(g_levelset_fname, "default.kye"); // DS:0191
  load_level(g_levelidx);
  set_status_msg(g_level_hint);
  update_edit_menu_item();
  g_moving_by_mouse = 0;
  InvalidateRect(mainwnd, NULL, TRUE);
  UpdateWindow(mainwnd);
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  deinit_timer();
  CloseSound(); // SOUND by ordinal, Ordinal 2?
  return msg.wParam;
}
