#include <windows.h>
#include <string.h>

HCURSOR g_hcCross; // DS:2D32
HCURSOR g_hcArrow; // DS:2D34

HANDLE g_hInstance; // DS:2D5A
HWND g_mainwnd; // DS:2D5C

void update_window_title(void);

// CS:0C19
LONG FAR PASCAL _export WndProc(HWND hWnd, WORD msg, WORD wParam, LONG lParam) {
  return DefWindowProc(hWnd, msg, wParam, lParam);
}

// CS:0D6A
int PASCAL _export WinMain(HANDLE hInstance,
                           HANDLE hPrevInstance,
                           LPSTR lpCmdLine,
                           int nCmdShow) {
  // needs 0x60 (96) extra bytes on stack
  char cmdlinebuf[50]; // /A0 BP-0x60
  // TODO /D2 BP-0x2A
  WNDCLASS maincls; // /E4 BP-0x1C
  HWND mainwnd; // /FE BP-0x02
  // BP-0x00

  register int si;
  register char FAR *di;

  (void)nCmdShow;

  for (di = lpCmdLine, si = 0; *di != 0 && si < 50; di++, si++) {
    cmdlinebuf[si] = *di;
  }
  // BUG: Clobbers past buffer on edge case
  cmdlinebuf[si] = 0;

  if (!hPrevInstance) {
    maincls.style = CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS; // 0x000B
    maincls.lpfnWndProc = WndProc; // CS:0C19
    maincls.cbClsExtra = 0;
    maincls.cbWndExtra = 0;
    maincls.hInstance = hInstance;
    maincls.hIcon = LoadIcon(hInstance, "IDI_KYE"); // str DS:0x16E
    maincls.hCursor = NULL;
    maincls.hbrBackground = GetStockObject(WHITE_BRUSH); // 0x0000
    maincls.lpszMenuName = "Kye"; // str DS:0176
    maincls.lpszClassName = "Kye"; // str DS:017A
    if (!RegisterClass(&maincls)) {
      // BUG: Returns success on failure
      return 0;
    }
  }
  mainwnd = CreateWindow(
    "Kye", // DS:017E
    "Kye", // DS:0182
    WS_MINIMIZEBOX|WS_SYSMENU|WS_DLGFRAME|WS_SYSMENU, // 0x00CA0000
    50, // CW_USEDEFAULT would be better of course
    50, // Ditto... and yes, it *is* in Windows 3.0.
    // BUG: Different-sized window borders break things.
    // - Windows 3.1 JP edition has a bad time with this.
    // - Wine with large fonts completely gobbles the status bar.
    //   - Speaking of Wine, 16-bit code runs really nicely on 64-bit OSes.
    //     - Hey Microsoft, have you tried not sucking? No? OK then.
    482,
    379,
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
  // DS:2D56 = 0x0000;
  // DS:03F2 = 0x0000;
  // DS:005C = 0x0000;

  g_hcArrow = LoadCursor(NULL, IDC_ARROW);
  g_hcCross = LoadCursor(NULL, IDC_CROSS);

  update_window_title();
  ShowWindow(mainwnd, nCmdShow);

  MessageBox(NULL, "TODO", "Not Kye Yet", MB_ICONINFORMATION);
  return 0;
}

// CS:17BD
void update_window_title(void) {
  // needs 0x82 (130) extra bytes on stack
  char title[128] // /7E BP-0x82
  // TODO /FE BP-0x02
  // BP-0x00

  // DI = window_title;
  // SI=0x444, CX=0x2, REP MOVSW
  //
  // TODO! --GM
}
