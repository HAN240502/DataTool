#include <stdio.h>
#include <Windows.h>
#include <CommCtrl.h>

#include "HAN_Lib\HAN_windows.h"
#include "GlobalVariables.h"
#include "DataTool\DataCmd\HAN_DataCmd.h"
#include "DataTool\FileConversion\HAN_FileConversionWindow.h"
#include "DataTool\HexView\HAN_Hex.h"
#include "DataTool\BinView\HAN_Bin.h"
#include "DataTool\SrecView\HAN_Srec.h"
#include "DataTool\AscView\HAN_Asc.h"
#include "DataTool\ComTool\HAN_ComTool.h"

#define TAB_TITLE_SIZE_W        50
#define TAB_TITLE_SIZE_H        28

typedef enum {
    WID_WINDOW_TAB,
    WID_WINDOW_FILE_CONVERSION,
    WID_WINDOW_HEX_VIEW,
    WID_WINDOW_BIN_VIEW,
    WID_WINDOW_SREC_VIEW,
    WID_WINDOW_ASC_VIEW,
    WID_WINDOW_COM_TOOL,
} ENUMWID;
typedef enum {
    TAB_ID_FILE_CONVERSION,
    TAB_ID_HEX_VIEW,
    TAB_ID_BIN_VIEW,
    TAB_ID_SREC_VIEW,
    TAB_ID_ASC_VIEW,
    TAB_ID_COM_TOOL,
    TAB_WIN_CNT,
} ENUMTABID;

typedef struct tagTABINFO {
    HANCHAR     pTitle[256];
    HANPCSTR    pClass;
    HWND        hItem;
    HMENU       nWinId;
    void        (*RegisterHANTabWindow)(HINSTANCE hInst);
    void*       pIni;
} TABINFO;

static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void ReadCfgIniFile(void);
static void WriteCfgIniFile(void);

static HWND         g_hTab = NULL;
static TABINFO      g_tiTabInfo[TAB_WIN_CNT] = {
    [TAB_ID_FILE_CONVERSION] = {
        .pTitle = TEXT("文件转换"),
        .pClass = HAN_FILE_CONVERSION_CLASS,
        .nWinId = (HMENU)WID_WINDOW_FILE_CONVERSION,
        .RegisterHANTabWindow = RegisterHANFileConversion,
        .pIni = &g_cfgHANData.cfgFileConversion,
    },
    [TAB_ID_HEX_VIEW] = {
        .pTitle = TEXT("hex"),
        .pClass = HAN_HEX_VIEW_CLASS,
        .nWinId = (HMENU)WID_WINDOW_HEX_VIEW,
        .RegisterHANTabWindow = RegisterHANHexView,
        .pIni = NULL,
    },
    [TAB_ID_SREC_VIEW] = {
        .pTitle = TEXT("srec"),
        .pClass = HAN_SREC_VIEW_CLASS,
        .nWinId = (HMENU)WID_WINDOW_SREC_VIEW,
        .RegisterHANTabWindow = RegisterHANSrecView,
        .pIni = NULL,
    },
    [TAB_ID_BIN_VIEW] = {
        .pTitle = TEXT("bin"),
        .pClass = HAN_BIN_VIEW_CLASS,
        .nWinId = (HMENU)WID_WINDOW_BIN_VIEW,
        .RegisterHANTabWindow = RegisterHANBinView,
        .pIni = NULL,
    },
    [TAB_ID_ASC_VIEW] = {
        .pTitle = TEXT("asc"),
        .pClass = HAN_ASC_VIEW_CLASS,
        .nWinId = (HMENU)WID_WINDOW_ASC_VIEW,
        .RegisterHANTabWindow = RegisterHANAscView,
        .pIni = NULL,
    },
    [TAB_ID_COM_TOOL] = {
        .pTitle = TEXT("串口工具"),
        .pClass = HAN_COM_TOOL_CLASS,
        .nWinId = (HMENU)WID_WINDOW_ASC_VIEW,
        .RegisterHANTabWindow = RegisterHANComTool,
        .pIni = NULL,
    },
};

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, HANPSTR lpCmdLine, int nCmdShow)
{
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

            DWORD dwWindowStyle = WS_CHILD | WS_VISIBLE;
            for (int i = 0; i < TAB_WIN_CNT; i++)
            {
                tiItem.pszText = g_tiTabInfo[i].pTitle;
                TabCtrl_InsertItem(g_hTab, i, &tiItem);
                g_tiTabInfo[i].RegisterHANTabWindow(hInst);
                g_tiTabInfo[i].hItem = CreateWindow(g_tiTabInfo[i].pClass, NULL, dwWindowStyle,
                    0, TAB_TITLE_SIZE_H, rcClient.right, rcClient.bottom - TAB_TITLE_SIZE_H,
                    g_hTab, g_tiTabInfo[i].nWinId, hInst, g_tiTabInfo[i].pIni);
                dwWindowStyle = WS_CHILD;
            }
            TabCtrl_SetItemSize(g_hTab, TAB_TITLE_SIZE_W, TAB_TITLE_SIZE_H);
            TabCtrl_SetCurSel(g_hTab, 0);

            break;
        }
        case WM_NOTIFY: {
            LPNMHDR pNMHDR = (LPNMHDR)lParam;
            if (WID_WINDOW_TAB == pNMHDR->idFrom)
            {
                switch (((LPNMHDR)lParam)->code) {
                    case TCN_SELCHANGE: {
                        int nPage = TabCtrl_GetCurSel(g_hTab);
                        for (int i = 0; i < ArrLen(g_tiTabInfo); i++)
                        {
                            if (nPage == i) { ShowWindow(g_tiTabInfo[i].hItem, SW_SHOW); }
                            else { ShowWindow(g_tiTabInfo[i].hItem, SW_HIDE); }
                        }
                        break;
                    }
                }
            }
            break;
        }

        case WM_DESTROY: {
            WriteCfgIniFile();
            PostQuitMessage(0);
            break;
        }
        default: {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }

    return lWndProcRet;
}

static void ReadCfgIniFile(void)
{
    // HANCHAR pIniPath[PATH_STR_SIZE];
    // GetCurrentDirectory(PATH_STR_SIZE, pIniPath);
    // HAN_strcat(pIniPath, TEXT("\\") CFG_INI_FILE_NAME);

    // ReadConvertFileIniFile(&(g_cfgHANData.cfgFileConversion), pIniPath);

    GetCurrentDirectory(PATH_STR_SIZE, g_pIniFileName);
    HAN_strcat(g_pIniFileName, TEXT("\\") CFG_INI_FILE_NAME);

    ReadConvertFileIniFile(&(g_cfgHANData.cfgFileConversion), g_pIniFileName);
}
static void WriteCfgIniFile(void)
{
    HANCHAR pIniPath[PATH_STR_SIZE];
    GetCurrentDirectory(PATH_STR_SIZE, pIniPath);
    HAN_strcat(pIniPath, TEXT("\\") CFG_INI_FILE_NAME);
    
    WriteFileConversionCfg(g_tiTabInfo[TAB_ID_FILE_CONVERSION].hItem, pIniPath);
}
