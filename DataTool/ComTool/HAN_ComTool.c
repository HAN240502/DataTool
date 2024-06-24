#include <float.h>

#include "HAN_ComTool.h"
#include "ComToolModel\GlobalVariables\HAN_ComToolGlobalVariables.h"
#include "ComToolModel\Consle\HAN_ComToolConsle.h"
#include "..\..\HAN_Lib\HAN_wingdi.h"

static LRESULT CALLBACK ComToolWndProc(HWND hComTool, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT CreateAction(HWND hComTool, LPARAM lParam);
static void SizeAction(PCOMTOOLEXTRA ctInfo);

void RegisterHANComTool(HINSTANCE hInst)
{
    WNDCLASSEX wcex = {
        .cbSize         = sizeof(WNDCLASSEX),
        .style          = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc    = ComToolWndProc,
        .cbClsExtra     = 0,
        .cbWndExtra     = sizeof(PCOMTOOLEXTRA),
        .hInstance      = hInst,
        .hIcon          = LoadIcon(NULL,IDI_APPLICATION),
        .hCursor        = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName   = NULL,
        .lpszClassName  = HAN_COM_TOOL_CLASS,
        .hIconSm        = NULL,
    };
    RegisterClassEx(&wcex);
}

void ReadComToolIniFile(HANPCSTR pIniPath, void* pParam)
{
    ReadComToolConsleIniFile(pIniPath, pParam);
}

void WriteComToolIniFile(HANPCSTR pIniPath, HWND hComTool)
{
    WriteComToolConsleIniFile(pIniPath, hComTool);
}

static LRESULT CALLBACK ComToolWndProc(HWND hComTool, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lWndProcRet = 0;

    // ¶ÁÈ¡ÊôÐÔ
    PCOMTOOLEXTRA ctInfo = (PCOMTOOLEXTRA)GetWindowLongPtr(hComTool, 0);

    switch (message) {
        case WM_CREATE: {
            lWndProcRet = CreateAction(hComTool, lParam);
        } break;
        case WM_SIZE: {
            SizeAction(ctInfo);
        } break;
        case WM_CTLCOLORSTATIC: {
            lWndProcRet = (INT_PTR)GetStockObject(WHITE_BRUSH);
        } break;

        default: {
            lWndProcRet = DefWindowProc(hComTool, message, wParam, lParam);
        } break;
    }

    return lWndProcRet;
}

static LRESULT CreateAction(HWND hComTool, LPARAM lParam)
{
    LRESULT lWndProcRet = 0;
    PCOMTOOLEXTRA ctInfo;
    HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;
    PCOMTOOLCFG pCtConfig = ((LPCREATESTRUCT)lParam)->lpCreateParams;
    RECT rcClientSize;

    HANDLE hHeap = GetProcessHeap();
    if (NULL == hHeap) { lWndProcRet = -1; }
    if (-1 != lWndProcRet)
    {
        ctInfo = (PCOMTOOLEXTRA)HANWinHeapAlloc(hHeap, NULL, sizeof(COMTOOLEXTRA));
        if (NULL == ctInfo) { lWndProcRet = -1; }
    }

    if (-1 != lWndProcRet)
    {
        int nWinX = COMTOOL_WINDOW_DX;
        int nWinY = COMTOOL_WINDOW_DY;

        SetWindowLongPtr(hComTool, 0, (LONG_PTR)ctInfo);

        *ctInfo = g_ftDefaultCfg;
        ctInfo->hHeap = hHeap;
        ctInfo->hInst = hInst;
        ctInfo->hSelf = hComTool;
        if (NULL != pCtConfig)
        {
            ctInfo->pCtConfig = *pCtConfig;
        }

        GetClientRect(hComTool, &rcClientSize);

        RegisterHANComToolConsle(hInst);

        ctInfo->hFont.hHex = CreateFontIndirect(&g_lfHexFont);
        ctInfo->hFont.hSys = CreateFontIndirect(&g_lfSysFont);

        ctInfo->printData.printData.hText = CreateWindow(TEXT("edit"), NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_READONLY | ES_MULTILINE,
            nWinX, nWinY, COMTOOL_TEXT_WINDOW_MIN_W, COMTOOL_TEXT_WINDOW_MIN_H,
            hComTool, (HMENU)WID_COMTOOL_TEXT, hInst, NULL);
        
        nWinX = COMTOOL_WINDOW_DX;
        nWinY += COMTOOL_TEXT_WINDOW_MIN_H + COMTOOL_WINDOW_DY;
        ctInfo->hConsle = CreateComToolWindow(nWinX, nWinY, hComTool, (HMENU)WID_COMTOOL_CONSOLE, hInst, ctInfo);

        SendMessage(ctInfo->printData.printData.hText, WM_SETFONT, (WPARAM)(ctInfo->hFont.hHex), (LPARAM)TRUE);

        SendMessage(ctInfo->printData.printData.hText, EM_SETLIMITTEXT, COMTOOL_TEXT_WINDOW_LIMIT, 0);

        SizeAction(ctInfo);
    }

    return lWndProcRet;
}
static void SizeAction(PCOMTOOLEXTRA ctInfo)
{
    RECT rcClientSize;
    HANINT nWinW;
    HANINT nWinH;

    GetClientRect(ctInfo->hSelf, &rcClientSize);
    nWinW = GetRectW(&rcClientSize) - (2 * COMTOOL_WINDOW_DX);
    nWinH = GetRectH(&rcClientSize) - (3 * COMTOOL_WINDOW_DY) - COMTOOL_CONSLE_WINDOW_H;

    if (nWinW < COMTOOL_TEXT_WINDOW_MIN_W) { nWinW = COMTOOL_TEXT_WINDOW_MIN_W; }
    if (nWinH < COMTOOL_TEXT_WINDOW_MIN_H) { nWinH = COMTOOL_TEXT_WINDOW_MIN_H; }

    MoveWindow(ctInfo->printData.printData.hText,
        COMTOOL_WINDOW_DX, COMTOOL_WINDOW_DY,
        nWinW, nWinH,
        TRUE
    );
    MoveWindow(ctInfo->hConsle,
        COMTOOL_WINDOW_DX, nWinH + (2 * COMTOOL_WINDOW_DY),
        COMTOOL_CONSLE_WINDOW_W, COMTOOL_CONSLE_WINDOW_H,
        TRUE
    );
}
