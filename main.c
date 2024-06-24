#include <stdio.h>
#include <Windows.h>
#include <CommCtrl.h>
#include <shlwapi.h>

#include "GlobalVariables.h"
#include "HAN_Lib\HAN_windows.h"
#include "DataTool\DataToolInfo.h"
#include "DataTool\DataCmd\HAN_DataCmd.h"

#define TAB_TITLE_SIZE_W        50
#define TAB_TITLE_SIZE_H        28

#define CFG_INI_FILE_NAME       TEXT("HANDataCfg.ini")

#define INI_MAIN_APP_NAME       TEXT("mainConfig")

typedef enum {
    WID_WINDOW_TAB,
    WID_DATA_TOOL_BASE,
} MAINWID;

static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static void GetIniFileFullPath(HANPSTR pIniFullPath, HANSIZE nLen);
static void ReadCfgIniFile(void);
static void WriteCfgIniFile(void);

static DATATOOLID   sg_nLastChosenToolId = 0;
static HWND         g_hTab = NULL;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, HANPSTR lpCmdLine, int nCmdShow)
{
    (void)hPrevInstance;

    int ret = 0;

    ReadCfgIniFile();

    if (FALSE == HanDataCmd(lpCmdLine))
    {
        // 注册窗口类
        WNDCLASSEX wcex = {
            .cbSize         = sizeof(WNDCLASSEX),
            .style          = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc    = MainWndProc,
            .cbClsExtra     = 0,
            .cbWndExtra     = 0,
            .hInstance      = hInstance,
            .hIcon          = LoadIcon(NULL,IDI_APPLICATION),
            .hCursor        = LoadCursor(NULL, IDC_ARROW),
            .hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1),
            .lpszMenuName   = NULL,
            .lpszClassName  = TEXT("MainHexView"),
            .hIconSm        = NULL,
        };
        RegisterClassEx(&wcex);

        // 创建窗口
        HWND hWnd = CreateWindow(TEXT("MainHexView"), TEXT("HAN_HexView"),
            WS_OVERLAPPEDWINDOW | WS_SYSMENU,
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
            NULL, NULL, hInstance, NULL);

        // 显示窗口
        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);

        // 主消息循环
        MSG msg;
        while (0 != GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        ret = (int)msg.wParam;
    }

    return ret;
}

static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lWndProcRet = 0;

    switch (message) {
        case WM_CREATE: {
            HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;

            RECT rcClient;
            GetClientRect(hWnd, &rcClient);

            INITCOMMONCONTROLSEX icexInitTab;
            TCITEM tiItem;

            icexInitTab.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icexInitTab.dwICC = ICC_TAB_CLASSES;
            InitCommonControlsEx(&icexInitTab);

            g_hTab = CreateWindow(WC_TABCONTROL, TEXT(""), WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                0, 0, rcClient.right, rcClient.bottom,
                hWnd, (HMENU)WID_WINDOW_TAB, hInst, NULL);
            HFONT hFont = CreateFontIndirect(&g_lfSysFont);
            SendMessage(g_hTab, WM_SETFONT, (WPARAM)hFont, TRUE);
            tiItem.mask = TCIF_TEXT;

            DWORD dwWindowStyle;
            HMENU hDataToolMenu = (HMENU)WID_DATA_TOOL_BASE;
            for (DATATOOLID i = 0; i < DATA_TOOL_ID_CNT_MAX; i++)
            {
                dwWindowStyle = WS_CHILD;
                if (sg_nLastChosenToolId == i) { dwWindowStyle |= WS_VISIBLE; }

                tiItem.pszText = g_tiTabInfo[i].pTitle;
                TabCtrl_InsertItem(g_hTab, i, &tiItem);
                g_tiTabInfo[i].RegisterHANTabWindow(hInst);
                g_tiTabInfo[i].hItem = CreateWindow(g_tiTabInfo[i].pClass, NULL, dwWindowStyle,
                    0, TAB_TITLE_SIZE_H, rcClient.right, rcClient.bottom - TAB_TITLE_SIZE_H,
                    g_hTab, hDataToolMenu, hInst, g_tiTabInfo[i].pIni);
                hDataToolMenu++;
            }
            TabCtrl_SetItemSize(g_hTab, TAB_TITLE_SIZE_W, TAB_TITLE_SIZE_H);
            TabCtrl_SetCurSel(g_hTab, sg_nLastChosenToolId);
        } break;
        case WM_NOTIFY: {
            LPNMHDR pNMHDR = (LPNMHDR)lParam;
            if (WID_WINDOW_TAB == pNMHDR->idFrom)
            {
                switch (((LPNMHDR)lParam)->code) {
                    case TCN_SELCHANGE: {
                        int nPage = TabCtrl_GetCurSel(g_hTab);
                        for (int i = 0; i < DATA_TOOL_ID_CNT_MAX; i++)
                        {
                            if (nPage == i) { ShowWindow(g_tiTabInfo[i].hItem, SW_SHOW); }
                            else { ShowWindow(g_tiTabInfo[i].hItem, SW_HIDE); }
                        }
                        sg_nLastChosenToolId = nPage;
                        break;
                    }
                }
            }
        } break;
        case WM_SIZE: {
            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            MoveWindow(g_hTab, 0, 0, rcClient.right, rcClient.bottom, TRUE);
            for (DATATOOLID iLoop = 0; iLoop < DATA_TOOL_ID_CNT_MAX; iLoop++)
            {
                MoveWindow(g_tiTabInfo[iLoop].hItem,
                    0, TAB_TITLE_SIZE_H, rcClient.right,
                    rcClient.bottom - TAB_TITLE_SIZE_H,
                    TRUE
                );
            }
        } break;

        case WM_CLOSE: {
            WriteCfgIniFile();
            return DefWindowProc(hWnd, message, wParam, lParam);
        } break;

        case WM_DESTROY: {
            
            PostQuitMessage(0);
            break;
        }
        default: {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }

    return lWndProcRet;
}

static void GetIniFileFullPath(HANPSTR pIniFullPath, HANSIZE nLen)
{
    GetModuleFileName(NULL, pIniFullPath, nLen);
    
    PathRemoveFileSpec(pIniFullPath);
    HAN_strcat(pIniFullPath, TEXT("\\") CFG_INI_FILE_NAME);
}
static void ReadCfgIniFile(void)
{
    HANCHAR pIniPath[PATH_STR_SIZE];
    HANCHAR pText[INI_MAIN_VALUE_STR_SIZE];
    DATATOOLID iLoop;

    GetIniFileFullPath(pIniPath, PATH_STR_SIZE);

    GetPrivateProfileString(
        INI_MAIN_APP_NAME, TEXT("LastChosenTool"), TEXT("0"),
        pText, INI_MAIN_VALUE_STR_SIZE, pIniPath
    );
    sg_nLastChosenToolId = HAN_strtoul(pText, NULL, 10);

    for (iLoop = 0; iLoop < DATA_TOOL_ID_CNT_MAX; iLoop++)
    {
        if (NULL != g_tiTabInfo[iLoop].ReadIniFile)
        {
            g_tiTabInfo[iLoop].ReadIniFile(pIniPath, g_tiTabInfo[iLoop].pIni);
        }
    }
}
static void WriteCfgIniFile(void)
{
    HANCHAR pIniPath[PATH_STR_SIZE];
    HANCHAR pText[INI_MAIN_VALUE_STR_SIZE];
    DATATOOLID iLoop;

    GetIniFileFullPath(pIniPath, PATH_STR_SIZE);

    HAN_snprintf(pText, INI_MAIN_VALUE_STR_SIZE, TEXT("%u"), sg_nLastChosenToolId);

    WritePrivateProfileString(INI_MAIN_APP_NAME, TEXT("LastChosenTool"), pText, pIniPath);
    
    for (iLoop = 0; iLoop < DATA_TOOL_ID_CNT_MAX; iLoop++)
    {
        if (NULL != g_tiTabInfo[iLoop].ReadIniFile)
        {
            g_tiTabInfo[iLoop].WriteIniFile(pIniPath, g_tiTabInfo[iLoop].hItem);
        }
    }
}
