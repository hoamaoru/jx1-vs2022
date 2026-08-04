#pragma once

// Child control IDs
#define IDC_LISTVIEW_SERVICES   1001
#define IDC_TAB_LOGS            1002
#define IDC_EDIT_LOG_BASE       1100   // + service id (0..5)
#define IDC_BTN_START           1201
#define IDC_BTN_STOP            1202
#define IDC_BTN_RESTART         1203
#define IDC_BTN_STARTALL        1204
#define IDC_BTN_STOPALL         1205
#define IDC_BTN_FORCESTOP       1206
#define IDC_BTN_EXPORTLOG       1207
#define IDC_STATUSBAR           1301

// System tray context menu
#define IDM_TRAY_OPEN           1401
#define IDM_TRAY_STARTALL       1402
#define IDM_TRAY_STOPALL        1403
#define IDM_TRAY_EXIT           1404

#define ID_TIMER_TICK           2001
#define TICK_INTERVAL_MS        1000

// Custom window messages posted by CServiceManager to the main window.
// WM_APP_LOGLINE   : wParam = service id, lParam = char* (heap, allocated with _strdup, receiver must free())
// WM_APP_STATUSCHANGED : wParam = service id, lParam = unused
#define WM_APP_LOGLINE          (WM_APP + 1)
#define WM_APP_STATUSCHANGED    (WM_APP + 2)
#define WM_APP_TRAYICON         (WM_APP + 3)

#define SERVICE_COUNT           6
