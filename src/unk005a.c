// vim: set syntax=cpp :
#include <windows.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kyetypes.h"
#include "kyeprocs.h"
#include "kyevars.h"

#include "resdefs.h"

// DS: string constant addresses vs variable addresses mandates that somewhere around here starts this module.

// DS:050C pertains to some kind of file open dialogue?
char g_050C[128] = ""; // DS:050C
char g_058C[128] = ""; // DS:058C
char g_060C[128] = ""; // DS:060C

char g_kye_file_pattern[13] = "*.kye"; // DS:068C
char g_kye_file_extension[5] = ".kye"; // DS:0699
char g_069E[11] = "~~~~~~~~.*"; // DS:069E

char g_2D90[256]; // DS:2D90

// CS:2A48 - ***CODE MATCH!***
void reinit_fileopen_dialog(HWND hDlg) {
  // [BP-2].w = temporary storage for segment for strcat

  strcpy(g_2D90, g_060C);
  strcat(g_2D90, g_kye_file_pattern);
  DlgDirList(hDlg, g_2D90, OPENFILE_404, OPENFILE_403, 0);

  // FIXME: 0x4010 is DDL_DRIVES | DDL_DIRECTORY, this seems to be missing from BC++3.0 headers! --GM
  DlgDirList(hDlg, g_069E, OPENFILE_405, OPENFILE_403, 0x4010); // DS:069E

  SetDlgItemText(hDlg, OPENFILE_401, g_kye_file_pattern); // DS:068C
}

// CS:2AE1 - ***CODE MATCH!***
void cs_2AE1(HWND hDlg, LPSTR a1, LPSTR a3, LPSTR a5) {
  LPSTR fol;
  char tmpc0;

  (void)hDlg;

  fol = a5 + lstrlen(a5);
  while (*fol != ':' && *fol != '\\' && fol > a5) {
    fol = AnsiPrev(a5, fol);
  }

  if (*fol != ':' && *fol != '\\') {
    lstrcpy(a3, a5);
    *a1 = 0;
    return;
  }

  lstrcpy(a3, fol+1);
  tmpc0 = fol[1];
  lstrcpy(a1, a5);
  fol[1] = tmpc0;
  a1[fol-a5+1] = 0;
}

// CS:2BA6 - ***CODE MATCH!***
void cs_2BA6(char *a0, const char *a1) {
  const char *fol; // SI

  for (fol = a1; *fol != 0 && *fol != '.'; fol++) {
    // Do nothing, just search
  }

  if (*fol != 0) {
    if (strchr(fol, '*') == NULL) {
      if (strchr(fol, '?') == NULL) {
        strcpy(a0, fol);
      }
    }
  }
}

// CS:2C31 - ***CODE MATCH!***
void cs_2C31(char *a0, const char *a1) {
  char *fol; // SI

  for (fol = a0; *fol != 0 && *fol != '.'; fol++) {
    // Do nothing, just search
  }

  if (*fol != '.') {
    strcat(a0, a1);
  }
}

// CS:2C83 - ***CODE MATCH!***
void cs_2C83(char *a0, const char *a1) {
  int tmp0; // AX

  tmp0 = strlen(a0);
  if (tmp0 > 0 && a0[tmp0-1] != '\\' && a0[tmp0-1] != ':') {
    strcat(a0, "\\"); // DS:06A9
  }

  strcat(a0, a1);
}

// CS:2CFB - ***CODE MATCH!***
int FAR PASCAL_EXPORT DLG_FOPEN_FUNC(HWND hDlg, WORD msg, WORD wParam, LONG lParam) {
  switch (msg) {
    case WM_COMMAND:
      switch (wParam) {
        case OPENFILE_404: // 2D42
          switch (HIWORD(lParam)) {
            case 1:
              if (!DlgDirSelect(hDlg, g_2D90, OPENFILE_404)) {
                SetDlgItemText(hDlg, OPENFILE_401, g_2D90);
                SendDlgItemMessage(hDlg, OPENFILE_401, WM_USER + 1, 0x0000, 0x7FFF0000UL);
              }
              break;

            case 2:
              goto unk_2E1D;
          }
          return 1;

        case OPENFILE_405: // 2D99
          switch (HIWORD(lParam)) {
            case 1: // 2DA8
              if (!DlgDirSelect(hDlg, g_2D90, OPENFILE_405)) {
                break;
              }

              strcat(g_2D90, g_kye_file_pattern);
              SetDlgItemText(hDlg, OPENFILE_401, g_2D90);
              SendDlgItemMessage(hDlg, OPENFILE_401, WM_USER + 1, 0x0000, 0x7FFF0000UL);
              break;

            case 2:
              goto unk_2E1D;
          }

          return 1;

        case OPENFILE_OK:
        unk_2E1D:
          GetDlgItemText(hDlg, OPENFILE_401, g_050C, 128);
          if (strchr(g_050C, '*') != NULL || strchr(g_050C, '?') != NULL) {
            cs_2AE1(hDlg, g_2D90, g_kye_file_pattern, g_050C);
            if (g_2D90[0] != 0) {
              strcpy(g_060C, g_2D90);
            }
            cs_2BA6(g_kye_file_extension, g_kye_file_pattern);
            reinit_fileopen_dialog(hDlg);
            return 1;
          }

          cs_2C31(g_050C, g_kye_file_extension);
          GetDlgItemText(hDlg, OPENFILE_403, g_058C, 128);
          EndDialog(hDlg, 0);
          return 1;

        case OPENFILE_CANCEL: // 2F14
          EndDialog(hDlg, 0);
          g_050C[0] = 0;
          return 1;
      }
      break;

    case WM_INITDIALOG:
      reinit_fileopen_dialog(hDlg);
      SetDlgItemText(hDlg, OPENFILE_401, g_kye_file_pattern); // DS:068C
      SendDlgItemMessage(hDlg, OPENFILE_401, WM_USER + 1, 0x0000, 0x7FFF0000UL);
      SetFocus(GetDlgItem(hDlg, OPENFILE_401));
      return 0;
  }

  return 0;
}

// CS:2F89 - ***CODE MATCH!***
void show_file_open_dialog(char *fname_buf) {
  FARPROC dlgproc;

  dlgproc = MakeProcInstance((FARPROC)DLG_FOPEN_FUNC, g_hInstance);
  DialogBox(g_hInstance, "DLG_OPENFILE", g_mainwnd, dlgproc); // DS:06AB
  FreeProcInstance(dlgproc);

  if (g_050C[0] != 0) {
    strcpy(fname_buf, g_058C);
    cs_2C83(fname_buf, g_050C);
  }
}

