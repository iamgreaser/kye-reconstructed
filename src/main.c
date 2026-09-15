#include "kye.h"

int g_is_in_editor = 0; // DS:03F2
// DS:03F4 contains strings.

HBRUSH g_hbrBlack; // DS:2B64
HBRUSH g_hbrWhite; // DS:2B62
HBRUSH g_hbrRed;   // DS:2B60
HBRUSH g_hbrGreen; // DS:2B5E
HBRUSH g_hbrBlue;  // DS:2B5C
HPEN g_saved_hpen; // DS:2B5A
HPEN g_hpenBlack; // DS:2B58
HPEN g_hpenWhite; // DS:2B56
HPEN g_hpenRed;  // DS:2B54
HBITMAP g_hbmpKye; // DS:2B52
HBITMAP g_hbmpBlock; // DS:2B50
HBITMAP g_hbmpWall; // DS:2B4E

char g_unused_err_line_1[80]; // DS:2AFE
char g_unused_err_line_2[80]; // DS:2AAE
char g_level_name[20]; // DS:2A9A
char g_level_congrats[80]; // DS:2A4A
char g_level_hint[80]; // DS:29FA

// CS:0F78 - ***CODE MATCH!***
int FAR PASCAL_EXPORT DLG_ERR1_FUNC(HWND hDlg, WORD msg, WORD wParam, LONG lParam) {
  (void)lParam;
  // Seems to be unused?
  switch (msg) {
  case WM_COMMAND:
    switch (COMMAND_idItem(wParam, lParam)) {
    case ERR1_OK_BUTTON:
      EndDialog(hDlg, 0);
      return 1;
    default:
      return 0;
    }
    //break;

  case WM_CLOSE:
    EndDialog(hDlg, 0);
    return 1;

  case WM_INITDIALOG:
    SetDlgItemText(hDlg, ERR1_LINE1, g_unused_err_line_1);
    SetDlgItemText(hDlg, ERR1_LINE2, g_unused_err_line_2);
    SetFocus(GetDlgItem(hDlg, 1));
    return 0;
  default:
    return 0;
  }
}

// CS:1005 - ***CODE MATCH!***
void msgbox_error(char *m1, char *m2) {
  MessageBox(g_mainwnd, m2, m1, MB_ICONEXCLAMATION);
}

// CS:101D - ***CODE MATCH!***
void init_brushes_and_stuff(void) {
  g_hbrBlack = CreateSolidBrush(RGB(0x00,0x00,0x00));
  g_hbrWhite = CreateSolidBrush(RGB(0xFF,0xFF,0xFF));
  g_hbrRed   = CreateSolidBrush(RGB(0xFF,0x00,0x00));
  g_hbrGreen = CreateSolidBrush(RGB(0x00,0xFF,0x00));
  g_hbrBlue  = CreateSolidBrush(RGB(0x00,0x00,0xFF));
  g_hpenBlack = CreatePen(PS_SOLID, 1, RGB(0x00,0x00,0x00));
  g_hpenWhite = CreatePen(PS_SOLID, 1, RGB(0xFF,0xFF,0xFF));
  g_hpenRed   = CreatePen(PS_SOLID, 1, RGB(0xFF,0x00,0x00));
  g_hbmpKye   = LoadBitmap(g_hInstance, "kye");   // DS:03F4
  g_hbmpBlock = LoadBitmap(g_hInstance, "block"); // DS:03F8
  g_hbmpWall  = LoadBitmap(g_hInstance, "wall");  // DS:03FE
}

// CS:10C0 - ***CODE MATCH!***
void deinit_brushes_and_stuff(void) {
  DeleteObject(g_hbrBlack);
  DeleteObject(g_hbrWhite);
  DeleteObject(g_hbrRed);
  DeleteObject(g_hbrGreen);
  DeleteObject(g_hbrBlue);
  DeleteObject(g_hpenBlack);
  DeleteObject(g_hpenWhite);
  DeleteObject(g_hpenRed);
  DeleteObject(g_hbmpKye);
  DeleteObject(g_hbmpBlock);
  DeleteObject(g_hbmpWall);
}

// CS:1124 - ***CODE MATCH!***
void compute_window_geometry(void) {
  int textheight; // SI

  GetWindowRect(g_mainwnd, &g_r_mainfrm);
  GetClientRect(g_mainwnd, &g_r_maincli);

  // SI = 16 as a constant used twice for optimisation
  // However, we do this plus 1 in each case?
  textheight = 16;
  SetRect(
    &g_r_playfield,
    g_r_maincli.left,
    g_r_maincli.top,
    g_r_maincli.left+480,
    g_r_maincli.top+320);
  SetRect(
    &g_r_status_left,
    g_r_maincli.left,
    g_r_playfield.bottom+1,
    g_r_maincli.left+300,
    g_r_playfield.bottom+textheight+1);
  SetRect(
    &g_r_status_right,
    g_r_status_left.right+2,
    g_r_playfield.bottom+1,
    g_r_playfield.right,
    g_r_playfield.bottom+textheight+1);
}

// CS:11A3 - ***CODE MATCH!***
int mouse_is_in_playfield(int x, int y) {
  // stack: 0x4 bytes
  POINT p;

  p.x = x;
  p.y = y;
  return PtInRect(&g_r_playfield, p);
}

// CS:11C4 - ***CODE MATCH!***
void set_status_msg(const char *msg) {
  // almost BUG: Rect invalidated before updating drawing info
  // Probably fine on 3.x, but Win32 might care.
  InvalidateRect(g_mainwnd, &g_r_status_right, FALSE);
  strcpy(g_statusmsg, msg);
}

// CS:1204 - ***CODE MATCH!***
int show_text_input_dialog(const char *msg, char *buf) {
  // stack: 0x04 bytes
  FARPROC dlgproc; // BP/FC

  strcpy(g_dialog_message, msg);
  strcpy(g_dialog_text, buf);

  // BUG: Failure not checked
  dlgproc = MakeProcInstance((FARPROC)DLG_INP1_FUNC, g_hInstance);
  DialogBox(g_hInstance, "DLG_INP1", g_mainwnd, dlgproc); // DS:0403
  FreeProcInstance(dlgproc);

  strcpy(buf, g_dialog_text);
  if (g_dialog_text[0] == 0) {
    return IDCANCEL;
  }
  return 0;
}

// CS:12CB - ***CODE MATCH!***
int FAR PASCAL_EXPORT DLG_LVLDUN_FUNC(HWND hDlg, WORD msg, WORD wParam, LONG lParam) {
  (void)lParam;
  switch (msg) {
  case WM_COMMAND:
    // TODO: Name the items --GM
    switch (COMMAND_idItem(wParam, lParam)) {
    case 1:
      EndDialog(hDlg, 0);
      return 1;
    default:
      return 0;
    }
    //break;

  case WM_CLOSE:
    EndDialog(hDlg, 0);
    return 1;

  case WM_INITDIALOG:
    // TODO: Name the items --GM
    SetDlgItemText(hDlg, 104, g_level_name);
    SetDlgItemText(hDlg, 106, g_level_congrats);
    SetFocus(GetDlgItem(hDlg, 1));
    return 0;
  default:
    return 0;
  }
}

// CS:1358 - ***CODE MATCH!***
void show_level_done(void) {
  // stack: 0x04 bytes
  FARPROC dlgproc; // BP/FC

  // BUG: Failure not checked
  dlgproc = MakeProcInstance((FARPROC)DLG_LVLDUN_FUNC, g_hInstance);
  DialogBox(g_hInstance, "DLG_DUN1", g_mainwnd, dlgproc); // DS:040C
  FreeProcInstance(dlgproc);
}

// CS:1395 - ***CODE MATCH!***
int FAR PASCAL_EXPORT DLG_LVLNEW_FUNC(HWND hDlg, WORD msg, WORD wParam, LONG lParam) {
  (void)lParam;
  switch (msg) {
  case WM_COMMAND:
    // TODO: Name the items --GM
    switch (COMMAND_idItem(wParam, lParam)) {
    case 1:
      EndDialog(hDlg, 0);
      return 1;
    default:
      return 0;
    }
    //break;

  case WM_CLOSE:
    EndDialog(hDlg, 0);
    return 1;

  case WM_INITDIALOG:
    // TODO: Name the items --GM
    SetDlgItemText(hDlg, 104, g_level_name);
    SetFocus(GetDlgItem(hDlg, 1));
    return 0;
  default:
    return 0;
  }
}

// CS:1416 - ***CODE MATCH!***
void show_next_level_name(void) {
  // stack: 0x04 bytes
  FARPROC dlgproc; // BP/FC

  // BUG: Failure not checked
  dlgproc = MakeProcInstance((FARPROC)DLG_LVLNEW_FUNC, g_hInstance);
  DialogBox(g_hInstance, "DLG_NEW1", g_mainwnd, dlgproc); // DS:0415
  FreeProcInstance(dlgproc);
}

// CS:1453 - ***CODE MATCH!***
int FAR PASCAL_EXPORT DLG_KYESGONE_FUNC(HWND hDlg, WORD msg, WORD wParam, LONG lParam) {
  (void)lParam;
  switch (msg) {
  case WM_COMMAND:
    // TODO: Name the items --GM
    switch (COMMAND_idItem(wParam, lParam)) {
    case 1:
      EndDialog(hDlg, 0);
      return 1;
    default:
      return 0;
    }
    //break;

  case WM_CLOSE:
    EndDialog(hDlg, 0);
    return 1;

  case WM_INITDIALOG:
    // TODO: Name the items --GM
    SetFocus(GetDlgItem(hDlg, 1));
    break;
  }
  return 0;
}

// CS:14BA - ***CODE MATCH!***
void show_game_over(void) {
  // stack: 0x04 bytes
  FARPROC dlgproc; // BP/FC

  // BUG: Failure not checked
  dlgproc = MakeProcInstance((FARPROC)DLG_KYESGONE_FUNC, g_hInstance);
  DialogBox(g_hInstance, "DLG_GON1", g_mainwnd, dlgproc); // DS:041E
  FreeProcInstance(dlgproc);
}

// CS:14F7 - ***CODE MATCH!***
int FAR PASCAL_EXPORT DLG_INPNAM_FUNC(HWND hDlg, WORD msg, WORD wParam, LONG lParam) {
  (void)lParam;
  switch (msg) {
  case WM_COMMAND:
    // TODO: Name the items --GM
    switch (COMMAND_idItem(wParam, lParam)) {
    case 1:
      GetDlgItemText(hDlg, 101, g_dialog_text, 80);
      EndDialog(hDlg, 0);
      return 1;
    case 2:
      strcpy(g_dialog_text, "");
      EndDialog(hDlg, 0);
      return 1;
    default:
      return 0;
    }
    //break;

  case WM_CLOSE:
    EndDialog(hDlg, 0);
    return 1;

  case WM_INITDIALOG:
    // TODO: Name the items --GM
    SetDlgItemText(hDlg, 101, g_dialog_text);
    SetFocus(GetDlgItem(hDlg, 101));
    return 0;
  }
  return 0;
}

// CS:15C0 - ***CODE MATCH!***
void show_level_name_input(void) {
  // stack: 0x04 bytes
  FARPROC dlgproc; // BP/FC

  // BUG: Failure not checked
  dlgproc = MakeProcInstance((FARPROC)DLG_INPNAM_FUNC, g_hInstance);
  DialogBox(g_hInstance, "DLG_NAM1", g_mainwnd, dlgproc); // DS:0428
  FreeProcInstance(dlgproc);
}

// CS:15FD - ***CODE MATCH!***
int FAR PASCAL_EXPORT DLG_OK_FUNC(HWND hDlg, WORD msg, WORD wParam, LONG lParam) {
  (void)lParam;
  switch (msg) {
  case WM_COMMAND:
    // TODO: Name the items --GM
    switch (COMMAND_idItem(wParam, lParam)) {
    case 1:
      EndDialog(hDlg, 0);
      return 1;
    default:
      return 0;
    }
    //break;

  case WM_CLOSE:
    EndDialog(hDlg, 0);
    return 1;

  case WM_INITDIALOG:
    // TODO: Name the items --GM
    SetFocus(GetDlgItem(hDlg, 1));
    return 0;
  }
  return 0;
}

// CS:166C - ***CODE MATCH!***
void show_charity_dialog(void) {
  FARPROC dlgproc;

  // BUG: Failure not checked
  dlgproc = MakeProcInstance((FARPROC)DLG_OK_FUNC, g_hInstance);
  DialogBox(g_hInstance, "DLG_WHAT", g_mainwnd, dlgproc); // DS:0431
  FreeProcInstance(dlgproc);
}

// CS:16A9 - ***CODE MATCH!***
void show_last_level_ending(void) {
  FARPROC dlgproc;

  // BUG: Failure not checked
  dlgproc = MakeProcInstance((FARPROC)DLG_OK_FUNC, g_hInstance);
  DialogBox(g_hInstance, "DLG_LAST", g_mainwnd, dlgproc); // DS:043A
  FreeProcInstance(dlgproc);
}

// CS:16E6 - ***CODE MATCH!***
int FAR PASCAL_EXPORT DLG_INP1_FUNC(HWND hDlg, WORD msg, WORD wParam, LONG lParam) {
  (void)lParam;
  switch (msg) {
  case WM_COMMAND:
    // TODO: Name the items --GM
    switch (COMMAND_idItem(wParam, lParam)) {
    case 1:
      GetDlgItemText(hDlg, 103, g_dialog_text, 80);
      EndDialog(hDlg, 0);
      return 1;
    case 2:
      strcpy(g_dialog_text, ""); // DS:0443
      EndDialog(hDlg, 0);
      return 1;
    default:
      return 0;
    }
    //break;

  case WM_CLOSE:
    EndDialog(hDlg, 0);
    return 1;

  case WM_INITDIALOG:
    // TODO: Name the items --GM
    SetDlgItemText(hDlg, 102, g_dialog_message);
    SetDlgItemText(hDlg, 103, g_dialog_text);
    SetFocus(GetDlgItem(hDlg, 103));
    return 0;
  default:
    return 0;
  }
}

// CS:17BD
void update_window_title(void) {
  // needs 0x82 (130) extra bytes on stack
  char title[128]; // /7E BP-0x82
  //int unk00; // /FE BP-0x02 - for some reason I think we got some spillage?

  strcpy(title, "Kye"); // DS:0444
  if (strlen(g_levelset_fname) > 0) {
    strcat(title, " - ["); // DS:0448
    strcat(title, g_levelset_fname);
    strcat(title, "]"); // DS:044D
  }
  if (g_is_in_editor == 1) {
    strcat(title, " (Edit) "); // DS:044F
  }
  SetWindowText(g_mainwnd, title);
}
