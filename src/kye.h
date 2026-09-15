// in TinyCC, `WIN32` seems to be defined in windows.h.
#include <windows.h>
#ifdef WIN32
  // Win32 port.
  #include <stdio.h>

  #include <stdlib.h>
  #include <string.h>
  #include <time.h>

  #define FAR
  #define PASCAL_EXPORT WINAPI
  #define PASCAL_NOEXPORT WINAPI
  #define HANDLE_HINSTANCE HINSTANCE

  #define CloseSound()
  #define DlgDirSelect(dlg, buf, id) DlgDirSelectEx((dlg), (buf), sizeof((buf)), (id))
  #define MoveTo(dc, x, y) MoveToEx((dc), (x), (y), NULL)
  #define OpenSound()

  // Wrapper for possibly the most infamous Win16-Win32 API breakage.
  // WM_COMMAND changed how wParam and lParam are encoded, so we have to wrap around the differences.
  #define COMMAND_idItem(wParam, lParam) (LOWORD(wParam))
  #define COMMAND_hWndCtl(wParam, lParam) ((HWND)(lParam))
  #define COMMAND_wNotifyCode(wParam, lParam) (HIWORD(wParam))

  // These bugfixes are needed for the 32-bit build to function correctly, they are:
  // - use-after-free of g_maindc which results in CreateCompatibleDC() failing and animations not working
  #ifndef APPLY_BUGFIXES
    #define APPLY_BUGFIXES 1
  #endif

  // A wrapper around MSVCRT.
  #include "compat/libc95.h"

  // TODO: Probably not quite right, this should actually reimplement the Borland PRNG. --GM
  #define random(x) (rand()/((RAND_MAX/(x))-(x)))

#else
  // Original Win16 code.
  #include <stdio.h>

  #include <stdlib.h>
  #include <string.h>
  #include <time.h>

  #define PASCAL_EXPORT PASCAL _export
  #define PASCAL_NOEXPORT PASCAL
  #define HANDLE_HINSTANCE HANDLE

  #define COMMAND_idItem(wParam, lParam) (wParam)
  #define COMMAND_hWndCtl(wParam, lParam) ((HWND)(LOWORD(lParam)))
  #define COMMAND_wNotifyCode(wParam, lParam) (HIWORD(lParam))

  #ifndef APPLY_BUGFIXES
    #define APPLY_BUGFIXES 0
  #endif

#endif

#include "kyetypes.h"
#include "kyeprocs.h"
#include "kyevars.h"

#include "resdefs.h"
