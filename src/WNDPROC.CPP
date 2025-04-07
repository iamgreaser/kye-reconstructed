// vim: set syntax=cpp :

#include <windows.h>
#include <stdio.h>

#include <string.h>

#include "kyetypes.h"
#include "kyeprocs.h"
#include "kyevars.h"

#include "resdefs.h"

int unused_0054[4] = {-1, -1, -1, -1};
int g_mouse_captured;
int g_key_pressed;

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
    return; // FIXME: Double-RET in output --GM
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
  if (mouse_is_in_playfield(lParam)) {
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
      cs_2791(x, y);
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
  if (mouse_is_in_playfield(lParam)) {
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
  if (mouse_is_in_playfield(lParam)) {
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
  if (mouse_is_in_playfield(lParam)) {
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

// CS:0573
void on_key_press(HWND hWnd, WORD wParam) {
  (void)hWnd;

  if (g_is_in_editor != 0) return;

  switch(wParam) {
  case VK_PRIOR:
    g_kye_target_cx = g_kye_main_cx + 1;
    g_kye_target_cy = g_kye_main_cy - 1;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    return;
  case VK_NEXT:
    g_kye_target_cx = g_kye_main_cx + 1;
    g_kye_target_cy = g_kye_main_cy + 1;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    return;
  case VK_END:
    g_kye_target_cx = g_kye_main_cx - 1;
    g_kye_target_cy = g_kye_main_cy + 1;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    return;
  case VK_HOME:
    g_kye_target_cx = g_kye_main_cx - 1;
    g_kye_target_cy = g_kye_main_cy - 1;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    return;
  case VK_LEFT:
    g_kye_target_cx = g_kye_main_cx - 1;
    g_kye_target_cy = g_kye_main_cy;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    return;
  case VK_UP:
    g_kye_target_cx = g_kye_main_cx;
    g_kye_target_cy = g_kye_main_cy - 1;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    return;
  case VK_RIGHT:
    g_kye_target_cx = g_kye_main_cx + 1;
    g_kye_target_cy = g_kye_main_cy;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    return;
  case VK_DOWN:
    g_kye_target_cx = g_kye_main_cx;
    g_kye_target_cy = g_kye_main_cy + 1;
    g_kye_needs_update = 1;
    tick_game();
    animate_everything();
    return;
  }
}

// CS:06AC
void update_edit_menu_item(void) {
  // TODO CHECK OPTIMISATIONS --GM
  HMENU menu; // DX
  menu = GetMenu(g_mainwnd);
  if (g_levelcount == 1) {
    EnableMenuItem(menu, IDC_LEVEL_EDIT, MF_ENABLED);
    return;
  }
  EnableMenuItem(menu, IDC_LEVEL_EDIT, MF_GRAYED);
}

// CS:06D6
int main_wm_command(HWND hWnd, WORD wParam) {
  // Needs 0x50 bytes of stack (BP/B0)

  HMENU menu; // SI

  switch (wParam) {
  case IDC_GAME_NEW:
    g_levelidx = 1;
    load_level(1);
    g_moving_by_mouse = 0;
    set_status_msg(g_level_hint);
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    return 1;

  case IDC_GAME_EXIT:
    PostQuitMessage(0);
    return 1;

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
      // CS:5119();
      menu = GetMenu(g_mainwnd);
      ModifyMenu(menu,
        IDC_LEVEL_EDIT,
        MF_BYCOMMAND,
        IDC_LEVEL_EDIT,
        "&Edit"); // DS:0060
      ModifyMenu(menu,
        IDC_LEVEL_GOTO,
        MF_BYCOMMAND,
        IDC_LEVEL_GOTO,
        "&Goto level..."); // DS:0066
      ModifyMenu(menu,
        IDC_LEVEL_RESTART,
        MF_BYCOMMAND,
        IDC_LEVEL_RESTART,
        "&Restart"); // DS:0075
      ModifyMenu(menu,
        IDC_LEVEL_FILE,
        MF_BYCOMMAND,
        IDC_LEVEL_FILE,
        "&File..."); // DS:007E
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
    return 1;

  case IDC_LEVEL_GOTO:
    if (g_is_in_editor == 0) {
      g_has_maintimer = 0;
      strcpy(g_dialog_text, "");
      // CS:15C0();
      strupr(g_dialog_text);
      // if (CS:1FC5() < 1) {
      //
      // } else {
      // }
      // HACK: Advance level anyway --GM
      g_level_complete = 1;

    } else {
      // Actually "Edit Hint"
      // TODO! --GM
    }
    return 1;

  case IDC_LEVEL_FILE:
    // TODO! --GM
    return 1;

  case IDC_LEVEL_EDIT:
    if (g_is_in_editor == 0) {
      g_levelidx = 1;
      load_level(1);
      if (g_levelcount < 2) {
        g_is_in_editor = 1;
        g_has_maintimer = 0;
        // CS:501F();
        menu = GetMenu(g_mainwnd);
        ModifyMenu(menu,
          IDC_LEVEL_EDIT,
          MF_BYCOMMAND,
          IDC_LEVEL_EDIT,
          "&Run"); // DS:0113
        ModifyMenu(menu,
          IDC_LEVEL_RESTART,
          MF_BYCOMMAND,
          IDC_LEVEL_RESTART,
          "&Abort Edit"); // DS:0118
        ModifyMenu(menu,
          IDC_LEVEL_GOTO,
          MF_BYCOMMAND,
          IDC_LEVEL_GOTO,
          "Edit &Hint"); // DS:0124
        ModifyMenu(menu,
          IDC_LEVEL_FILE,
          MF_BYCOMMAND,
          IDC_LEVEL_FILE,
          "Edit &Name"); // DS:012F
        EnableMenuItem(menu, IDC_GAME_NEW, MF_BYCOMMAND|MF_GRAYED);
        DrawMenuBar(hWnd);
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        update_window_title();
      } else {
        msgbox_error("Cannot edit multi-level files.", // DS:00C8
            "Load a single level file.(eg. template.kye)"); // DS:00E7
      }
    } else {
      g_is_in_editor = 0;
      // CS:5119();
      menu = GetMenu(g_mainwnd);
      ModifyMenu(menu,
        IDC_LEVEL_EDIT,
        MF_BYCOMMAND,
        IDC_LEVEL_EDIT,
        "&Edit"); // DS:013A
      ModifyMenu(menu,
        IDC_LEVEL_GOTO,
        MF_BYCOMMAND,
        IDC_LEVEL_GOTO,
        "&Goto level..."); // DS:0140
      ModifyMenu(menu,
        IDC_LEVEL_FILE,
        MF_BYCOMMAND,
        IDC_LEVEL_FILE,
        "&File"); // DS:014F
      ModifyMenu(menu,
        IDC_LEVEL_RESTART,
        MF_BYCOMMAND,
        IDC_LEVEL_RESTART,
        "&Restart"); // DS:0158
      EnableMenuItem(menu, IDC_GAME_NEW, MF_BYCOMMAND|MF_ENABLED);
      DrawMenuBar(hWnd);
      // CS:2241();
      load_level(g_levelidx);
      set_status_msg(g_level_hint);
      g_moving_by_mouse = 0;
      g_has_maintimer = 1;
      InvalidateRect(hWnd, NULL, TRUE);
      UpdateWindow(hWnd);
      update_window_title();
    }
    return 1;

  case IDC_HELP_ABOUT:
    show_charity_dialog();
    return 1;
  }
  return 0;
}

// CS:0C19
LONG FAR PASCAL _export WndProc(HWND hWnd, WORD msg, WORD wParam, LONG lParam) {
  // The compiler does some funky less-than/greater-than optimisation,
  // instead of using a LUT.
  // Maybe it was aiming to work nicely on a 486?
  // That was a thing when Win 3.0 came out.

  // TODO: Most of these are dummies... --GM
  // TODO: Handle the special-case no-ops of WM_SIZE and WM_CREATE --GM
  switch(msg) {
  case WM_COMMAND:
    return main_wm_command(hWnd, wParam);
  case WM_PAINT:
    main_paint(hWnd);
    break;
  case WM_DESTROY:
    deinit_brushes_and_stuff();
    PostQuitMessage(0);
    break;
  case WM_KEYDOWN:
    g_key_pressed = 1;
    on_key_press(hWnd, wParam);
    break;
  case WM_KEYUP:
    g_key_pressed = 0;
    break;
  case WM_LBUTTONUP:
    main_wm_lbuttonup(hWnd, lParam);
    break;
  case WM_TIMER:
    // BUG: (?) Handling of this flag is dubious.
    if (g_has_maintimer) {
      tick_game();
      animate_everything();
      g_has_maintimer = 1;
    }
    break;
  case WM_MOUSEMOVE:
    main_wm_mousemove(hWnd, lParam);
    break;
  case WM_LBUTTONDOWN:
    main_wm_lbuttondown(hWnd, lParam, wParam);
    break;
  case WM_LBUTTONDBLCLK:
    main_wm_lbuttondblclick(hWnd, lParam, wParam);
    break;
  case WM_RBUTTONDOWN:
    main_wm_rbuttondown(hWnd, lParam, wParam);
    break;
  default:
    return DefWindowProc(hWnd, msg, wParam, lParam);
  }
  return 0;
}
