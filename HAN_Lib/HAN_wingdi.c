#include <stdio.h>

#include "HAN_wingdi.h"

#define HANSTATIC

#if 1 /******************** HANInput ********************/
#define HSTBT_WID_LEFT_TEXT     0
#define HSTBT_WID_INPUT         1
#define HSTBT_WID_RIGHT_TEXT    2

typedef struct tagHANINPUT {
    HANDLE              hHeap;
    HWND                hLeftText;
    HWND                hInput;
    HWND                hRightText;
} HANINPUT, * HANPINPUT;

// WndProc & ʵ�� ����
HANSTATIC LRESULT CALLBACK HANInputWndProc(HWND hInput, UINT message, WPARAM wParam, LPARAM lParam);
HANSTATIC void s_HANInputMoveWindow(HWND hWnd, RECT* rcWin, BOOL bRepaint);
// �ӿڶ���
void RegisterHANInput(HINSTANCE hInstance)
{
    WNDCLASSEX wcexHANInput = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_PARENTDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = HANInputWndProc,
        .cbClsExtra = 0,
        .cbWndExtra = sizeof(HANINPUT),
        .hInstance = hInstance,
        .hIcon = 0,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName = NULL,
        .lpszClassName = HINPUT_CLASS,
        .hIconSm = NULL
    };
    RegisterClassEx(&wcexHANInput);
}
void HANInputSetLeftTextFont(HWND hInput, HFONT hFOnt, BOOL bRepaint)
{
    SendMessage(hInput, HIPM_SETLEFTTEXTFONT, (WPARAM)hFOnt, (LPARAM)bRepaint);
}
void HANInputSetInputFont(HWND hInput, HFONT hFOnt, BOOL bRepaint)
{
    SendMessage(hInput, HIPM_SETINPUTFONT, (WPARAM)hFOnt, (LPARAM)bRepaint);
}
void HANInputSetRightTextFont(HWND hInput, HFONT hFOnt, BOOL bRepaint)
{
    SendMessage(hInput, HIPM_SETRIGHTTEXTFONT, (WPARAM)hFOnt, (LPARAM)bRepaint);
}
void HANInputMoveLeftText(HWND hInput, int x, int y, int w, int h, BOOL bRepaint)
{
    RECT rcWin = {
        .left = x,
        .top = y,
        .right = x + w,
        .bottom = y + h,
    };
    SendMessage(hInput, HIPM_MOVELEFTTEXT, (WPARAM)bRepaint, (LPARAM)(&rcWin));
}
void HANInputMoveInput(HWND hInput, int x, int y, int w, int h, BOOL bRepaint)
{
    RECT rcWin = {
        .left = x,
        .top = y,
        .right = x + w,
        .bottom = y + h,
    };
    SendMessage(hInput, HIPM_MOVEINPUT, (WPARAM)bRepaint, (LPARAM)(&rcWin));
}
void HANInputMoveRightText(HWND hInput, int x, int y, int w, int h, BOOL bRepaint)
{
    RECT rcWin = {
        .left = x,
        .top = y,
        .right = x + w,
        .bottom = y + h,
    };
    SendMessage(hInput, HIPM_MOVERIGHTTEXT, (WPARAM)bRepaint, (LPARAM)(&rcWin));
}
void HANInputSetLeftTextText(HWND hInput, HANPCSTR pText)
{
    SendMessage(hInput, HIPM_SETLEFTTEXTTEXT, 0, (LPARAM)pText);
}
void HANInputSetInputText(HWND hInput, HANPCSTR pText)
{
    SendMessage(hInput, HIPM_SETINPUTTEXT, 0, (LPARAM)pText);
}
void HANInputSetRightTextText(HWND hInput, HANPCSTR pText)
{
    SendMessage(hInput, HIPM_SETRIGHTTEXTTEXT, 0, (LPARAM)pText);
}
void HANInputGetLeftTextText(HWND hInput, HANPSTR pText, int nMaxCnt)
{
    SendMessage(hInput, HIPM_GETLEFTTEXTTEXT, (WPARAM)nMaxCnt, (LPARAM)pText);
}
void HANInputGetInputText(HWND hInput, HANPSTR pText, int nMaxCnt)
{
    SendMessage(hInput, HIPM_GETINPUTTEXT, (WPARAM)nMaxCnt, (LPARAM)pText);
}
void HANInputGetRightTextText(HWND hInput, HANPSTR pText, int nMaxCnt)
{
    SendMessage(hInput, HIPM_GETRIGHTTEXTTEXT, (WPARAM)nMaxCnt, (LPARAM)pText);
}
// WndProc & ʵ�� ����
HANSTATIC LRESULT CALLBACK HANInputWndProc(HWND hInput, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lWndProcRet = 0;

    // ��ȡ����
    HANPINPUT ipInfo = (HANPINPUT)GetWindowLongPtr(hInput, 0);

    switch(message) {
        case WM_CREATE: {
            HANDLE hHeap = GetProcessHeap();
            if (NULL == hHeap) { lWndProcRet = -1; break; }
            ipInfo = (HANPINPUT)HANWinHeapAlloc(hHeap, NULL, sizeof(HANINPUT));
            if (NULL == ipInfo) { lWndProcRet = -1; break; }
            SetWindowLongPtr(hInput, 0, (LONG_PTR)ipInfo);
            ipInfo->hHeap = hHeap;

            // ��ȡ���ڴ�С
            RECT rcWin;
            GetClientRect(hInput, &rcWin);
            LONG nWinW = GetRectW(&rcWin);
            LONG nWinH = GetRectH(&rcWin);

            int xInput = nWinW / 3;
            int xRightText = nWinW * 2 / 3;

            // �����Ӵ���
            HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;
            ipInfo->hLeftText = CreateWindow(
                TEXT("static"), NULL, WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE,
                0, 0, xInput, nWinH, hInput, (HMENU)HSTBT_WID_LEFT_TEXT, hInst, NULL);
            ipInfo->hInput = CreateWindow(
                TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
                xInput, 0, xRightText - xInput, nWinH, hInput, (HMENU)HSTBT_WID_LEFT_TEXT, hInst, NULL);
            ipInfo->hRightText = CreateWindow(
                TEXT("static"), NULL, WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
                xRightText, 0, nWinW - xRightText, nWinH, hInput, (HMENU)HSTBT_WID_RIGHT_TEXT, hInst, NULL);

            break;
        }
        case WM_CTLCOLORSTATIC: {
            lWndProcRet = (INT_PTR)GetStockObject(NULL_BRUSH);
            break;
        }
        case HIPM_SETLEFTTEXTFONT: {
            SendMessage(ipInfo->hLeftText, WM_SETFONT, wParam, lParam);
            break;
        }
        case HIPM_SETINPUTFONT: {
            SendMessage(ipInfo->hInput, WM_SETFONT, wParam, lParam);
            break;
        }
        case HIPM_SETRIGHTTEXTFONT: {
            SendMessage(ipInfo->hRightText, WM_SETFONT, wParam, lParam);
            break;
        }
        case HIPM_MOVELEFTTEXT: {
            s_HANInputMoveWindow(ipInfo->hLeftText, (RECT*)lParam, (BOOL)wParam);
            break;
        }
        case HIPM_MOVEINPUT: {
            s_HANInputMoveWindow(ipInfo->hInput, (RECT*)lParam, (BOOL)wParam);
            break;
        }
        case HIPM_MOVERIGHTTEXT: {
            s_HANInputMoveWindow(ipInfo->hRightText, (RECT*)lParam, (BOOL)wParam);
            break;
        }
        case HIPM_SETLEFTTEXTTEXT: {
            SetWindowText(ipInfo->hLeftText, (HANPCSTR)lParam);
            break;
        }
        case HIPM_SETINPUTTEXT: {
            SetWindowText(ipInfo->hInput, (HANPCSTR)lParam);
            break;
        }
        case HIPM_SETRIGHTTEXTTEXT: {
            SetWindowText(ipInfo->hRightText, (HANPCSTR)lParam);
            break;
        }
        case HIPM_GETLEFTTEXTTEXT: {
            GetWindowText(ipInfo->hLeftText, (HANPSTR)lParam, (int)wParam);
            break;
        }
        case HIPM_GETINPUTTEXT: {
            GetWindowText(ipInfo->hInput, (HANPSTR)lParam, (int)wParam);
            break;
        }
        case HIPM_GETRIGHTTEXTTEXT: {
            GetWindowText(ipInfo->hRightText, (HANPSTR)lParam, (int)wParam);
            break;
        }

        default: {
            lWndProcRet = DefWindowProc(hInput, message, wParam, lParam);
            break;
        }
    }

    return lWndProcRet;
}
HANSTATIC void s_HANInputMoveWindow(HWND hWnd, RECT* rcWin, BOOL bRepaint)
{
    MoveWindow(hWnd, rcWin->left, rcWin->top, GetRectW(rcWin), GetRectH(rcWin), bRepaint);
}
#endif

#if 1 /******************** HANStateButton ********************/
#define HSTBT_WID_BUTTON          0

typedef struct tagHANSTBTSTATELIST HANSTBTSTATELIST, * HANPSTBTSTATELIST;
typedef const HANSTBTSTATELIST* HANPCSTBTSTATELIST;
struct tagHANSTBTSTATELIST {
    HSTBTSTATE          sbsState;
    HANPSTBTSTATELIST   pNext;
};
typedef struct tagHANSTATEBUTTON {
    HANDLE              hHeap;
    HWND                hButton;
    HANPSTBTSTATELIST   pHead;
    HANPSTBTSTATELIST   pTarget;
    HANSIZE             nListLen;
    void*               pParam;
} HANSTATEBUTTON, * HANPSTATEBUTTON;
typedef const HANSTATEBUTTON* HANPCSTATEBUTTON;

// �ڲ���������
HANSTATIC void HANStateButtonButtonAction(HWND hStateButton, HANPSTATEBUTTON sbInfo);
// WndProc & ʵ�� ����
HANSTATIC LRESULT CALLBACK HANStateButtonWndProc(HWND hStateButton, UINT message, WPARAM wParam, LPARAM lParam);
HANSTATIC HAN_errno_t s_HANStateButtonAddState(HANPSTATEBUTTON sbInfo, HPCSTBTSTATE pState);
HANSTATIC void s_HANStateButtonClick(HANPCSTATEBUTTON sbInfo);
// �ӿڶ���
void RegisterHANStateButton(HINSTANCE hInstance)
{
    WNDCLASSEX wcexHANStateButton = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_PARENTDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = HANStateButtonWndProc,
        .cbClsExtra = 0,
        .cbWndExtra = sizeof(HANPSTATEBUTTON),
        .hInstance = hInstance,
        .hIcon = 0,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName = NULL,
        .lpszClassName = HSTBT_CLASS,
        .hIconSm = NULL
    };
    RegisterClassEx(&wcexHANStateButton);
}
HAN_errno_t HANStateButtonAddState(HWND hStateButton, HPCSTBTSTATE pState)
{
    return SendMessage(hStateButton, HSTBTM_ADDSTATE, 0, (LPARAM)pState);
}
void HANStateButtonClick(HWND hStateButton)
{
    SendMessage(hStateButton, HSTBTM_CLICK, 0, 0);
}
// �ڲ���������
HANSTATIC void HANStateButtonButtonAction(HWND hStateButton, HANPSTATEBUTTON sbInfo)
{
    if (NULL != sbInfo->pTarget)
    {
        HPCSTBTSTATE pState = &(sbInfo->pTarget->sbsState);
        HANSIZE nNextCnt = 1;
        if (NULL != pState->Action) { nNextCnt = pState->Action(hStateButton, sbInfo->hButton, sbInfo->pParam); }
        for (HANSIZE i = 0; i < nNextCnt; i++) { sbInfo->pTarget = sbInfo->pTarget->pNext; }
        SetWindowText(sbInfo->hButton, sbInfo->pTarget->sbsState.pText);
    }
}
// WndProc & ʵ�� ����
HANSTATIC LRESULT CALLBACK HANStateButtonWndProc(HWND hStateButton, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lWndProcRet = 0;

    // ��ȡ����
    HANPSTATEBUTTON sbInfo = (HANPSTATEBUTTON)GetWindowLongPtr(hStateButton, 0);

    switch(message) {
        case WM_CREATE: {
            HANDLE hHeap = GetProcessHeap();
            if (NULL == hHeap) { lWndProcRet = -1; break; }
            sbInfo = (HANPSTATEBUTTON)HANWinHeapAlloc(hHeap, NULL, sizeof(HANSTATEBUTTON));
            if (NULL == sbInfo) { lWndProcRet = -1; break; }
            SetWindowLongPtr(hStateButton, 0, (LONG_PTR)sbInfo);
            sbInfo->hHeap = hHeap;

            // ��ȡ���ڴ�С
            RECT rcWin;
            GetClientRect(hStateButton, &rcWin);
            LONG nWinW = GetRectW(&rcWin);
            LONG nWinH = GetRectH(&rcWin);

            // �����Ӵ���
            HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;
            sbInfo->hButton = CreateWindow(
                TEXT("button"), NULL, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                0, 0, nWinW, nWinH, hStateButton, (HMENU)HSTBT_WID_BUTTON, hInst, NULL);

            sbInfo->pParam = ((LPCREATESTRUCT)lParam)->lpCreateParams;

            break;
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case HSTBT_WID_BUTTON: {
                    HANStateButtonButtonAction(hStateButton, sbInfo);
                    break;
                }

                default: {
                    break;
                }
            }
            break;
        }
        case HSTBTM_ADDSTATE: {
            lWndProcRet = s_HANStateButtonAddState(sbInfo, (HPCSTBTSTATE)lParam);
            break;
        }
        case HSTBTM_CLICK: {
            s_HANStateButtonClick(sbInfo);
            break;
        }

        default: {
            lWndProcRet = DefWindowProc(hStateButton, message, wParam, lParam);
            break;
        }
    }

    return lWndProcRet;
}
HANSTATIC HAN_errno_t s_HANStateButtonAddState(HANPSTATEBUTTON sbInfo, HPCSTBTSTATE pState)
{
    HAN_errno_t eRet = RET_OK;
    HANPSTBTSTATELIST pNewStateNode = (HANPSTBTSTATELIST)HANWinHeapAlloc(sbInfo->hHeap, NULL, sizeof(HANSTBTSTATELIST));
    if (NULL == pNewStateNode) { eRet = NOT_ENOUGH_MEMORY; }
    if (RET_OK == eRet)
    {
        pNewStateNode->sbsState = *pState;
        if (NULL == sbInfo->pHead)
        {
            sbInfo->pTarget = pNewStateNode;
            sbInfo->pHead = pNewStateNode;
            pNewStateNode->pNext = pNewStateNode;
            SetWindowText(sbInfo->hButton, pState->pText);
        }
        else
        {
            HANPSTBTSTATELIST pNode = sbInfo->pHead;
            for (HANSIZE i = 0; i < (sbInfo->nListLen - (HANSIZE)1); i++)
            {
                pNode = pNode->pNext;
            }
            pNode->pNext = pNewStateNode;
            pNewStateNode->pNext = sbInfo->pHead;
        }
        sbInfo->nListLen++;
    }
    return eRet;
}
HANSTATIC void s_HANStateButtonClick(HANPCSTATEBUTTON sbInfo)
{
    SendMessage(sbInfo->hButton, BM_CLICK, 0, 0);
}

#endif

#if 1 // �˿�
#define HCOM_WID_OPEN_COM       0
#define HCOM_WID_PORT_ID        1
#define HCOM_WID_BAUDRATE       2
#define HCOM_WID_BYTEBITS       3
#define HCOM_WID_PARITY         4
#define HCOM_WID_STOPBITS       5

#define HCOM_CID_TIMER          0
#define HCOM_DEF_TIMER_PERIOD   50          // Ĭ�϶�ʱ�����ڣ�ms

static HANPCSTR g_pBaudrate[] = {
    TEXT("110"),
    TEXT("300"),
    TEXT("600"),
    TEXT("1200"),
    TEXT("2400"),
    TEXT("4800"),
    TEXT("9600"),
    TEXT("14400"),
    TEXT("19200"),
    TEXT("38400"),
    TEXT("56000"),
    TEXT("57600"),
    TEXT("115200"),
    TEXT("128000"),
    TEXT("230400"),
    TEXT("256000"),
    TEXT("460800"),
    TEXT("500000"),
    TEXT("512000"),
    TEXT("600000"),
    TEXT("750000"),
    TEXT("921600"),
    TEXT("1000000"),
    TEXT("1500000"),
    TEXT("2000000"),
};
static HANPCSTR g_pByteBits[] = {
    TEXT("5"),
    TEXT("6"),
    TEXT("7"),
    TEXT("8"),
};
static HANPCSTR g_pParity[] = {
    [NOPARITY] = TEXT("NONE"),
    [ODDPARITY] = TEXT("ODD"),
    [EVENPARITY] = TEXT("EVEN"),
    [MARKPARITY] = TEXT("MARK"),
    [SPACEPARITY] = TEXT("SPACE"),
};
static HANPCSTR g_pStopBits[] = {
    [ONESTOPBIT] = TEXT("1"),
    [ONE5STOPBITS] = TEXT("1.5"),
    [TWOSTOPBITS] = TEXT("2"),
};
static COMCFG g_cfgDefCOM = {
    .id = 3,
    .dwFlagsAndAttributes = FILE_FLAG_OVERLAPPED,
    .dwInQueue = 100000,
    .dwOutQueue = 100000,
    .ReadIntervalTimeout = 0,
    .ReadTotalTimeoutMultiplier = 0,
    .ReadTotalTimeoutConstant = 0,
    .WriteTotalTimeoutMultiplier = 0,
    .WriteTotalTimeoutConstant = 0,
    .BaudRate = 115200,
    .ByteSize = 8,
    .Parity = NOPARITY,
    .StopBits = ONESTOPBIT,
};

typedef struct tagHANCOM {
    HANDLE      hHeap;
    HANDLE      hCOM;
    HMENU       hMenu;
    HWND        hParent;
    HWND        hSelf;
    HWND        hOpenCOM;
    HWND        hPortID;
    HWND        hBaudrate;
    HWND        hByteSize;
    HWND        hParity;
    HWND        hStopBits;
    COMCFG      cfgCOM;
    UINT        tTimerPeriod;
    DWORD       nBufSize;
    DWORD       nDataLen;
    DWORD       nFrameLen;
    BOOL        bReceiving;
    OVERLAPPED  olRead;
    OVERLAPPED  olWrite;
} HANCOM, * HANPCOM;
typedef const HANCOM* HANPCCOM;

// �ڲ���������
HANSTATIC HANSIZE HANCOMOpenCOMAction(HWND hStateButton, HWND hChildButton, HANPCOM hcInfo);
HANSTATIC HANSIZE HANCOMCloseCOMAction(HWND hStateButton, HWND hChildButton, HANPCOM hcInfo);
HANSTATIC void HANCOMUpdateCOMCfg(HANPCOM hcInfo);
HANSTATIC BOOL HANCOMUpdatePortList(HANPCOM hcInfo, uint16_t nTargetId);
HANSTATIC void HANCOMSweepCOMAction(const LPBYTE nCOMId, HANPCSTR pCOMName, DWORD nCount, HANPCOM hcInfo);
HANSTATIC void HANCOMPortIdAction(HANPCOM hcInfo, WPARAM wParam);
HANSTATIC void HANCOMBaudrateAction(HANPCOM hcInfo, WPARAM wParam);
HANSTATIC void HANCOMByteSizeAction(HANPCOM hcInfo, WPARAM wParam);
HANSTATIC void HANCOMParityAction(HANPCOM hcInfo, WPARAM wParam);
HANSTATIC void HANCOMStopBitsAction(HANPCOM hcInfo, WPARAM wParam);
HANSTATIC void HANCOMTimerAction(HANPCOM hcInfo);
// WndProc & ʵ�� ����
HANSTATIC LRESULT CALLBACK HANCOMWndProc(HWND hCOM, UINT message, WPARAM wParam, LPARAM lParam);
HANSTATIC void s_HANCOMShowWindow(HWND hWnd, BOOL bShow);
HANSTATIC void s_HANCOMMoveWindow(HWND hWnd, RECT* rcWin, BOOL bRepaint);
HANSTATIC BOOL s_HANCOMOpenCOM(HANPCOM hcInfo);
HANSTATIC HAN_errno_t s_HANCOMSetPortId(HANPCOM hcInfo, uint16_t nPortId);
HANSTATIC BOOL s_HANCOMSetBaudrate(HANPCOM hcInfo, DWORD nBaudrate);
HANSTATIC BOOL s_HANCOMSetByteSize(HANPCOM hcInfo, BYTE nByteSize);
HANSTATIC BOOL s_HANCOMSetParity(HANPCOM hcInfo, BYTE nParity);
HANSTATIC BOOL s_HANCOMSetStopBits(HANPCOM hcInfo, BYTE nStopBits);
HANSTATIC HAN_errno_t s_HANCOMGetRevLen(HANPCCOM hcInfo, DWORD* pLen);
HANSTATIC HAN_errno_t s_HANCOMReadData(HANPCOM hcInfo, HANPCOMREADDATA rdReadData);
HANSTATIC HAN_errno_t s_HANCOMWriteData(HANPCOM hcInfo, void* pData, DWORD nLen);
// �ӿڶ���
void RegisterHANCOM(HINSTANCE hInstance)
{
    WNDCLASSEX wcexHANCOM = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_PARENTDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = HANCOMWndProc,
        .cbClsExtra = 0,
        .cbWndExtra = sizeof(HANPCOM),
        .hInstance = hInstance,
        .hIcon = 0,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName = NULL,
        .lpszClassName = HCOM_CLASS,
        .hIconSm = NULL
    };
    RegisterClassEx(&wcexHANCOM);
}
void HANCOMShowOpenCOM(HWND hCOM, BOOL bShow)
{
    SendMessage(hCOM, HCOMM_SHOWOPENCOM, (WPARAM)bShow, 0);
}
void HANCOMShowPortId(HWND hCOM, BOOL bShow)
{
    SendMessage(hCOM, HCOMM_SHOWPORTID, (WPARAM)bShow, 0);
}
void HANCOMShowBaudrate(HWND hCOM, BOOL bShow)
{
    SendMessage(hCOM, HCOMM_SHOWBAUDRATE, (WPARAM)bShow, 0);
}
void HANCOMShowByteSize(HWND hCOM, BOOL bShow)
{
    SendMessage(hCOM, HCOMM_SHOWBYTESIZE, (WPARAM)bShow, 0);
}
void HANCOMShowParity(HWND hCOM, BOOL bShow)
{
    SendMessage(hCOM, HCOMM_SHOWPARITY, (WPARAM)bShow, 0);
}
void HANCOMShowStopBits(HWND hCOM, BOOL bShow)
{
    SendMessage(hCOM, HCOMM_SHOWSTOPBITS, (WPARAM)bShow, 0);
}
void HANCOMMoveOpenCOM(HWND hCOM, int x, int y, int w, int h, BOOL bRepaint)
{
    RECT rcWin = {
        .left = x,
        .top = y,
        .right = x + w,
        .bottom = y + h,
    };
    SendMessage(hCOM, HCOMM_MOVEOPENCOM, (WPARAM)bRepaint, (LPARAM)(&rcWin));
}
void HANCOMMovePortId(HWND hCOM, int x, int y, int w, int h, BOOL bRepaint)
{
    RECT rcWin = {
        .left = x,
        .top = y,
        .right = x + w,
        .bottom = y + h,
    };
    SendMessage(hCOM, HCOMM_MOVEPORTID, (WPARAM)bRepaint, (LPARAM)(&rcWin));
}
void HANCOMMoveBaudrate(HWND hCOM, int x, int y, int w, int h, BOOL bRepaint)
{
    RECT rcWin = {
        .left = x,
        .top = y,
        .right = x + w,
        .bottom = y + h,
    };
    SendMessage(hCOM, HCOMM_MOVEBAUDRATE, (WPARAM)bRepaint, (LPARAM)(&rcWin));
}
void HANCOMMoveByteSize(HWND hCOM, int x, int y, int w, int h, BOOL bRepaint)
{
    RECT rcWin = {
        .left = x,
        .top = y,
        .right = x + w,
        .bottom = y + h,
    };
    SendMessage(hCOM, HCOMM_MOVEBYTESIZE, (WPARAM)bRepaint, (LPARAM)(&rcWin));
}
void HANCOMMoveParity(HWND hCOM, int x, int y, int w, int h, BOOL bRepaint)
{
    RECT rcWin = {
        .left = x,
        .top = y,
        .right = x + w,
        .bottom = y + h,
    };
    SendMessage(hCOM, HCOMM_MOVEPARITY, (WPARAM)bRepaint, (LPARAM)(&rcWin));
}
void HANCOMMoveStopBits(HWND hCOM, int x, int y, int w, int h, BOOL bRepaint)
{
    RECT rcWin = {
        .left = x,
        .top = y,
        .right = x + w,
        .bottom = y + h,
    };
    SendMessage(hCOM, HCOMM_MOVESTOPBITS, (WPARAM)bRepaint, (LPARAM)(&rcWin));
}
BOOL HANCOMOpenCOM(HWND hCOM)
{
    return SendMessage(hCOM, HCOMM_OPENCOM, 0, 0);
}
HAN_errno_t HANCOMSetPortId(HWND hCOM, uint16_t nPortId)
{
    return SendMessage(hCOM, HCOMM_SETPORTID, (WPARAM)nPortId, 0);
}
BOOL HANCOMSetBaudrate(HWND hCOM, DWORD nBaudrate)
{
    return SendMessage(hCOM, HCOMM_SETBAUDRATE, (WPARAM)nBaudrate, 0);
}
BOOL HANCOMSetByteSize(HWND hCOM, BYTE nByteSize)
{
    return SendMessage(hCOM, HCOMM_SETBYTESIZE, (WPARAM)nByteSize, 0);
}
BOOL HANCOMSetParity(HWND hCOM, BYTE nParity)
{
    return SendMessage(hCOM, HCOMM_SETPARITY, (WPARAM)nParity, 0);
}
BOOL HANCOMSetStopBits(HWND hCOM, BYTE nStopBits)
{
    return SendMessage(hCOM, HCOMM_SETSTOPBITS, (WPARAM)nStopBits, 0);
}
HAN_errno_t HANCOMGetRevLen(HWND hCOM, DWORD* pLen)
{
    return SendMessage(hCOM, HCOMM_GETREVLEN, 0, (LPARAM)pLen);
}
HAN_errno_t HANCOMReadData(HWND hCOM, void* pData, DWORD nBufSize, DWORD* nRevSize)
{
    HANCOMREADDATA rdReadData = {
        .pData = pData,
        .nBufSize = nBufSize,
        .nRevSize = nRevSize,
    };
    return SendMessage(hCOM, HCOMM_READDATA, 0, (LPARAM)(&rdReadData));
}
HAN_errno_t HANCOMWriteData(HWND hCOM, const void* pData, DWORD nLen)
{
    return SendMessage(hCOM, HCOMM_WRITEDATA, (WPARAM)nLen, (LPARAM)pData);
}
// �ڲ���������
HANSTATIC HANSIZE HANCOMOpenCOMAction(HWND hStateButton, HWND hChildButton, HANPCOM hcInfo)
{
    (void)hStateButton;
    (void)hChildButton;

    HANSIZE nRet = 1;
    DWORD style = GetWindowStyle(hcInfo->hSelf);
    
    HANCOMUpdateCOMCfg(hcInfo);
    if (RET_OK != OpenCOM(&(hcInfo->hCOM), &hcInfo->cfgCOM))
    {
        if (0 != (style & HCOMMS_MSG_BOX))
        {
            MessageBox(NULL, TEXT("�򿪴���ʧ�ܣ�����������ԭ��\r\n1. �˿ں�ѡ�����\r\n2. ��������\r\n3. �˿ڱ���������ռ��"), NULL, 0);
        }
        nRet = 0;
    }
    else
    {
        SetTimer(hcInfo->hSelf, HCOM_CID_TIMER, hcInfo->tTimerPeriod, NULL);
    }

    return nRet;
}
HANSTATIC HANSIZE HANCOMCloseCOMAction(HWND hStateButton, HWND hChildButton, HANPCOM hcInfo)
{
    (void)hStateButton;
    (void)hChildButton;

    KillTimer(hcInfo->hSelf, HCOM_CID_TIMER);
    CloseHandle(hcInfo->hCOM);
    hcInfo->hCOM = INVALID_HANDLE_VALUE;

    return 1;
}
HANSTATIC void HANCOMUpdateCOMCfg(HANPCOM hcInfo)
{
    HANCHAR pPortIdText[PATH_STR_SIZE];
    HANCHAR pNumText[HCOM_NUM_TEXT_SIZE];
    HANPCSTR pId = pPortIdText;

    // �˿ں�
    GetWindowText(hcInfo->hPortID, pPortIdText, PATH_STR_SIZE);
    if (pPortIdText[0] != TEXT('\0'))
    {
        while ((!HAN_isdigit(*pId)) && (*pId != TEXT('\0'))) { pId++; }
        hcInfo->cfgCOM.id = HAN_strtol(pId, NULL, 10);
    }

    GetWindowText(hcInfo->hBaudrate, pNumText, HCOM_NUM_TEXT_SIZE);
    hcInfo->cfgCOM.BaudRate = HAN_strtol(pNumText, NULL, 10);
    
    GetWindowText(hcInfo->hByteSize, pNumText, HCOM_NUM_TEXT_SIZE);
    hcInfo->cfgCOM.ByteSize = HAN_strtol(pNumText, NULL, 10);

    hcInfo->cfgCOM.Parity = SendMessage(hcInfo->hParity, CB_GETCURSEL, 0, 0);

    hcInfo->cfgCOM.StopBits = SendMessage(hcInfo->hStopBits, CB_GETCURSEL, 0, 0);
}
HANSTATIC BOOL HANCOMUpdatePortList(HANPCOM hcInfo, uint16_t nTargetId)
{
    BOOL bRet = TRUE;
    HANCHAR pPortId[PATH_STR_SIZE];
    while (SendMessage(hcInfo->hPortID, CB_DELETESTRING, 0, 0) > 0) { }
    SweepCOMFromRegedit((void (*)(const LPBYTE, HANPCSTR, DWORD, void*))HANCOMSweepCOMAction, hcInfo);
    HAN_sprintf(pPortId, "COM%u", nTargetId);
    int box_id = SendMessage(hcInfo->hPortID, CB_FINDSTRING, -1, (LPARAM)pPortId);
    if (box_id != CB_ERR)
    {
        (void)ComboBoxSetCursel(hcInfo->hPortID, box_id);
    }
    else
    {
        bRet = FALSE;
    }
    return bRet;
}
HANSTATIC void HANCOMSweepCOMAction(const LPBYTE nCOMId, HANPCSTR pCOMName, DWORD nCount, HANPCOM hcInfo)
{
    (void)nCount;
    HANCHAR pPortIdText[PATH_STR_SIZE];
    HANPCSTR pCOMNameId = pCOMName;
    if (pCOMName == HAN_strstr(pCOMName, TEXT("\\Device\\"))) { pCOMNameId = &pCOMName[HAN_strlen(TEXT("\\Device\\"))]; }
    (void)HAN_sprintf(pPortIdText, TEXT("%s: %s"), nCOMId, pCOMNameId);
    (void)ComboBoxAddString(hcInfo->hPortID, pPortIdText);
}
HANSTATIC void HANCOMPortIdAction(HANPCOM hcInfo, WPARAM wParam)
{
    switch (HIWORD(wParam)) {
        case CBN_DROPDOWN: {
            (void)HANCOMUpdatePortList(hcInfo, hcInfo->cfgCOM.id);
            break;
        }
        case CBN_SELCHANGE: {
            HANCHAR pPortId[PATH_STR_SIZE];
            HANPCSTR pId = pPortId;
            
            GetWindowText(hcInfo->hPortID, pPortId, ArrLen(pPortId));
            if (pPortId[0] != TEXT('\0'))
            {
                while (!HAN_isdigit(*pId) && (*pId != TEXT('\0'))) { pId++; }
                uint16_t nPortId = HAN_strtol(pId, NULL, 10);
                if (RET_OK != s_HANCOMSetPortId(hcInfo, nPortId))
                {
                    if (0 != (GetWindowStyle(hcInfo->hSelf) & HCOMMS_MSG_BOX))
                    {
                        MessageBox(NULL, TEXT("�򿪴���ʧ��"), NULL, 0);
                    }
                    HANStateButtonClick(hcInfo->hOpenCOM);
                }
            }
            break;
        }

        default: {
            break;
        }
    }
}
HANSTATIC void HANCOMBaudrateAction(HANPCOM hcInfo, WPARAM wParam)
{
    switch (HIWORD(wParam)) {
        case CBN_SELCHANGE: {
            DWORD nBaudrate;
            HANCHAR pText[HCOM_NUM_TEXT_SIZE];

            GetWindowText(hcInfo->hBaudrate, pText, HCOM_NUM_TEXT_SIZE);
            nBaudrate = HAN_strtoul(pText, NULL, 10);

            s_HANCOMSetBaudrate(hcInfo, nBaudrate);
            break;
        }

        default: {
            break;
        }
    }
}
HANSTATIC void HANCOMByteSizeAction(HANPCOM hcInfo, WPARAM wParam)
{
    switch (HIWORD(wParam)) {
        case CBN_SELCHANGE: {
            BYTE nByteSize;
            HANCHAR pText[HCOM_NUM_TEXT_SIZE];

            GetWindowText(hcInfo->hByteSize, pText, HCOM_NUM_TEXT_SIZE);
            nByteSize = HAN_strtoul(pText, NULL, 10);

            if ((5 <= nByteSize) && (nByteSize <= 8)) { s_HANCOMSetByteSize(hcInfo, nByteSize); }
            break;
        }

        default: {
            break;
        }
    }
}
HANSTATIC void HANCOMParityAction(HANPCOM hcInfo, WPARAM wParam)
{
    switch (HIWORD(wParam)) {
        case CBN_SELCHANGE: {
            BYTE nParity = ComboBoxGetCursel(hcInfo->hParity);
            if (CB_ERR != nParity) { s_HANCOMSetParity(hcInfo, nParity); }
            break;
        }

        default: {
            break;
        }
    }
}
HANSTATIC void HANCOMStopBitsAction(HANPCOM hcInfo, WPARAM wParam)
{
    switch (HIWORD(wParam)) {
        case CBN_SELCHANGE: {
            BYTE nStopBits = ComboBoxGetCursel(hcInfo->hStopBits);
            if (CB_ERR != nStopBits) { s_HANCOMSetByteSize(hcInfo, nStopBits); }
            break;
        }

        default: {
            break;
        }
    }
}
HANSTATIC void HANCOMTimerAction(HANPCOM hcInfo)
{
    NMHDR nmh = { .idFrom = GetDlgCtrlID(hcInfo->hSelf), .hwndFrom = hcInfo->hSelf, };
    HWND hParent = hcInfo->hParent;
    DWORD nBufLen = GetCOMDataRevCount(hcInfo->hCOM);
    
    hcInfo->nFrameLen = nBufLen - hcInfo->nDataLen;
    hcInfo->nDataLen = nBufLen;
    if (0 < hcInfo->nFrameLen)
    {
        hcInfo->bReceiving = TRUE;
        if (hcInfo->nDataLen < hcInfo->nBufSize)
        {
            nmh.code = HCOMN_GETDATA;
            SendMessage(hParent, WM_NOTIFY, nmh.idFrom, (LPARAM)(&nmh));
        }
        else
        {
            nmh.code = HCOMN_OVERFLOW;
            SendMessage(hParent, WM_NOTIFY, nmh.idFrom, (LPARAM)(&nmh));
        }
    }
    else if ((0 < hcInfo->nDataLen) && (TRUE == hcInfo->bReceiving))
    {
        nmh.code = HCOMN_GETMSG;
        SendMessage(hParent, WM_NOTIFY, nmh.idFrom, (LPARAM)(&nmh));
        hcInfo->bReceiving = FALSE;
    }
    else {}
}
// WndProc & ʵ�� ����
HANSTATIC LRESULT CALLBACK HANCOMWndProc(HWND hCOM, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lWndProcRet = 0;

    // ��ȡ����
    HANPCOM hcInfo = (HANPCOM)GetWindowLongPtr(hCOM, 0);

    switch(message) {
        case WM_CREATE: {
            HANDLE hHeap = GetProcessHeap();
            if (NULL == hHeap) { lWndProcRet = -1; break; }
            hcInfo = (HANPCOM)HANWinHeapAlloc(hHeap, NULL, sizeof(HANCOM));
            if (NULL == hcInfo) { lWndProcRet = -1; break; }
            SetWindowLongPtr(hCOM, 0, (LONG_PTR)hcInfo);
            hcInfo->hHeap = hHeap;
            
            LPCREATESTRUCT pCreate = (LPCREATESTRUCT)lParam;
            HINSTANCE hInst = pCreate->hInstance;

            // ע��״̬��ť��
            RegisterHANStateButton(hInst);

            // �����Ӵ���
            hcInfo->hOpenCOM = CreateWindow(
                HSTBT_CLASS, NULL, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                10, 10, 100, 40, hCOM, (HMENU)HCOM_WID_OPEN_COM, hInst, hcInfo);
            hcInfo->hPortID = CreateWindow(
                TEXT("combobox"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
                120, 16, 210, 100, hCOM, (HMENU)HCOM_WID_PORT_ID, hInst, NULL);
            hcInfo->hBaudrate = CreateWindow(
                TEXT("combobox"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN,
                10, 60, 100, 150, hCOM, (HMENU)HCOM_WID_BAUDRATE, hInst, NULL);
            hcInfo->hByteSize = CreateWindow(
                TEXT("combobox"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
                120, 60, 50, 126, hCOM, (HMENU)HCOM_WID_BYTEBITS, hInst, NULL);
            hcInfo->hParity = CreateWindow(
                TEXT("combobox"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
                180, 60, 80, 126, hCOM, (HMENU)HCOM_WID_PARITY, hInst, NULL);
            hcInfo->hStopBits = CreateWindow(
                TEXT("combobox"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
                270, 60, 60, 126, hCOM, (HMENU)HCOM_WID_STOPBITS, hInst, NULL);

            // ��ʼ����������
            hcInfo->hParent = pCreate->hwndParent;
            hcInfo->hSelf = hCOM;
            hcInfo->hMenu = pCreate->hMenu;
            hcInfo->tTimerPeriod = HCOM_DEF_TIMER_PERIOD;
            hcInfo->nBufSize = HCOM_DEF_BUF_SIZE;
            hcInfo->nDataLen = 0;
            hcInfo->bReceiving = FALSE;
            // ��ʼ����������
            hcInfo->cfgCOM = g_cfgDefCOM;
            // ��ʼ����ť
            HSTBTSTATE hssButtonState[] = {
                { .pText = TEXT("�򿪴���"), .Action = (HSTBTAction)HANCOMOpenCOMAction, },
                { .pText = TEXT("�رմ���"), .Action = (HSTBTAction)HANCOMCloseCOMAction, },
            };
            (void)HANStateButtonAddState(hcInfo->hOpenCOM, &hssButtonState[0]);
            (void)HANStateButtonAddState(hcInfo->hOpenCOM, &hssButtonState[1]);
            // ��ʼ���˿ں�
            (void)HANCOMUpdatePortList(hcInfo, hcInfo->cfgCOM.id);
            // ��ʼ��������
            (void)ComboBoxAddStringArr(hcInfo->hBaudrate, g_pBaudrate, ArrLen(g_pBaudrate));
            HANCHAR Baudrate[16];
            HAN_sprintf(Baudrate, TEXT("%lu"), hcInfo->cfgCOM.BaudRate);
            SetWindowText(hcInfo->hBaudrate, Baudrate);
            // ��ʼ���ֽڳ�
            (void)ComboBoxAddStringArr(hcInfo->hByteSize, g_pByteBits, ArrLen(g_pByteBits));
            (void)ComboBoxSetCursel(hcInfo->hByteSize, hcInfo->cfgCOM.ByteSize - HAN_strtol(g_pByteBits[0], NULL, 10));
            // ��ʼ��У��λ
            (void)ComboBoxAddStringArr(hcInfo->hParity, g_pParity, ArrLen(g_pParity));
            (void)ComboBoxSetCursel(hcInfo->hParity, hcInfo->cfgCOM.Parity);
            // ��ʼ��ֹͣλ
            (void)ComboBoxAddStringArr(hcInfo->hStopBits, g_pStopBits, ArrLen(g_pStopBits));
            (void)ComboBoxSetCursel(hcInfo->hStopBits, hcInfo->cfgCOM.StopBits);

            break;
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case HCOM_WID_PORT_ID: {
                    HANCOMPortIdAction(hcInfo, wParam);
                    break;
                }
                case HCOM_WID_BAUDRATE: {
                    HANCOMBaudrateAction(hcInfo, wParam);
                    break;
                }
                case HCOM_WID_BYTEBITS: {
                    HANCOMByteSizeAction(hcInfo, wParam);
                    break;
                }
                case HCOM_WID_PARITY: {
                    HANCOMParityAction(hcInfo, wParam);
                    break;
                }
                case HCOM_WID_STOPBITS: {
                    HANCOMStopBitsAction(hcInfo, wParam);
                    break;
                }

                default: {
                    break;
                }
            }
            break;
        }
        case WM_TIMER: {
            HANCOMTimerAction(hcInfo);
            break;
        }
        case HCOMM_SHOWOPENCOM: {
            s_HANCOMShowWindow(hcInfo->hOpenCOM, (BOOL)wParam);
            break;
        }
        case HCOMM_SHOWPORTID: {
            s_HANCOMShowWindow(hcInfo->hPortID, (BOOL)wParam);
            break;
        }
        case HCOMM_SHOWBAUDRATE: {
            s_HANCOMShowWindow(hcInfo->hBaudrate, (BOOL)wParam);
            break;
        }
        case HCOMM_SHOWBYTESIZE: {
            s_HANCOMShowWindow(hcInfo->hByteSize, (BOOL)wParam);
            break;
        }
        case HCOMM_SHOWPARITY: {
            s_HANCOMShowWindow(hcInfo->hParity, (BOOL)wParam);
            break;
        }
        case HCOMM_SHOWSTOPBITS: {
            s_HANCOMShowWindow(hcInfo->hStopBits, (BOOL)wParam);
            break;
        }
        case HCOMM_MOVEOPENCOM: {
            s_HANCOMMoveWindow(hcInfo->hOpenCOM, (RECT*)lParam, (BOOL)wParam);
            break;
        }
        case HCOMM_MOVEPORTID: {
            s_HANCOMMoveWindow(hcInfo->hPortID, (RECT*)lParam, (BOOL)wParam);
            break;
        }
        case HCOMM_MOVEBAUDRATE: {
            s_HANCOMMoveWindow(hcInfo->hBaudrate, (RECT*)lParam, (BOOL)wParam);
            break;
        }
        case HCOMM_MOVEBYTESIZE: {
            s_HANCOMMoveWindow(hcInfo->hByteSize, (RECT*)lParam, (BOOL)wParam);
            break;
        }
        case HCOMM_MOVEPARITY: {
            s_HANCOMMoveWindow(hcInfo->hParity, (RECT*)lParam, (BOOL)wParam);
            break;
        }
        case HCOMM_MOVESTOPBITS: {
            s_HANCOMMoveWindow(hcInfo->hStopBits, (RECT*)lParam, (BOOL)wParam);
            break;
        }
        case HCOMM_OPENCOM: {
            lWndProcRet = s_HANCOMOpenCOM(hcInfo);
            break;
        }
        case HCOMM_SETPORTID: {
            lWndProcRet = s_HANCOMSetPortId(hcInfo, (uint16_t)wParam);
            break;
        }
        case HCOMM_SETBAUDRATE: {
            lWndProcRet = s_HANCOMSetBaudrate(hcInfo, (DWORD)wParam);
            break;
        }
        case HCOMM_SETBYTESIZE: {
            lWndProcRet = s_HANCOMSetByteSize(hcInfo, (BYTE)wParam);
            break;
        }
        case HCOMM_SETPARITY: {
            lWndProcRet = s_HANCOMSetParity(hcInfo, (BYTE)wParam);
            break;
        }
        case HCOMM_SETSTOPBITS: {
            lWndProcRet = s_HANCOMSetStopBits(hcInfo, (BYTE)wParam);
            break;
        }
        case HCOMM_GETREVLEN: {
            lWndProcRet = s_HANCOMGetRevLen(hcInfo, (DWORD*)lParam);
            break;
        }
        case HCOMM_READDATA: {
            lWndProcRet = s_HANCOMReadData(hcInfo, (HANPCOMREADDATA)lParam);
            break;
        }
        case HCOMM_WRITEDATA: {
            lWndProcRet = s_HANCOMWriteData(hcInfo, (void*)lParam, (DWORD)wParam);
            break;
        }

        default: {
            lWndProcRet = DefWindowProc(hCOM, message, wParam, lParam);
            break;
        }
    }

    return lWndProcRet;
}
HANSTATIC void s_HANCOMShowWindow(HWND hWnd, BOOL bShow)
{
    int nShow;
    if (FALSE == bShow) { nShow = SW_HIDE; }
    else { nShow = SW_SHOW; }
    ShowWindow(hWnd, nShow);
}
HANSTATIC void s_HANCOMMoveWindow(HWND hWnd, RECT* rcWin, BOOL bRepaint)
{
    MoveWindow(hWnd, rcWin->left, rcWin->top, GetRectW(rcWin), GetRectH(rcWin), bRepaint);
}
HANSTATIC BOOL s_HANCOMOpenCOM(HANPCOM hcInfo)
{
    BOOL bRet = TRUE;
    DWORD nFlag;
    SendMessage(hcInfo->hOpenCOM, HSTBTM_CLICK, 0, 0);
    bRet = GetHandleInformation(hcInfo->hCOM, &nFlag);
    return bRet;
}
HANSTATIC HAN_errno_t s_HANCOMSetPortId(HANPCOM hcInfo, uint16_t nPortId)
{
    HAN_errno_t nRet = RET_OK;
    DWORD nFlag;

    if (FALSE == HANCOMUpdatePortList(hcInfo, nPortId))
    {
        nRet = HCOM_PORT_ID_NOT_EXIST;
    }

    if (RET_OK == nRet)
    {
        if (nPortId != hcInfo->cfgCOM.id)
        {
            if (TRUE == GetHandleInformation(hcInfo->hCOM, &nFlag))
            {
                COMCFG cfgCOM = hcInfo->cfgCOM;
                HANDLE hCOM;
                cfgCOM.id = nPortId;
                nRet = OpenCOM(&hCOM, &cfgCOM);
                if (RET_OK == nRet)
                {
                    CloseHandle(hcInfo->hCOM);
                    hcInfo->hCOM = hCOM;
                    hcInfo->cfgCOM.id = nPortId;
                }
            }
            else
            {
                hcInfo->cfgCOM.id = nPortId;
            }
        }
    }

    return nRet;
}
HANSTATIC BOOL s_HANCOMSetBaudrate(HANPCOM hcInfo, DWORD nBaudrate)
{
    BOOL bRet = TRUE;
    DCB dcbCOMState;
    DWORD nFlag;

    if (TRUE == GetHandleInformation(hcInfo->hCOM, &nFlag)) // �������Ч�ԣ����˿��Ѵ򿪣���������ͨ������ʹ������Ч
    {
        if (!GetCommState(hcInfo->hCOM, &dcbCOMState)) { bRet = FALSE; }
        if (TRUE == bRet)
        {
            dcbCOMState.BaudRate = nBaudrate;
            if (!SetCommState(hcInfo->hCOM, &dcbCOMState)) { bRet = FALSE; }
        }
    }
    if (TRUE == bRet)
    {
        HANCHAR pText[HCOM_NUM_TEXT_SIZE];
        HAN_snprintf(pText, ArrLen(pText), TEXT("%lu"), nBaudrate);
        SetWindowText(hcInfo->hBaudrate, pText);
        hcInfo->cfgCOM.BaudRate = nBaudrate;
    }

    return bRet;
}
HANSTATIC BOOL s_HANCOMSetByteSize(HANPCOM hcInfo, BYTE nByteSize)
{
    BOOL bRet;
    DCB dcbCOMState;
    DWORD nFlag;

    if ((5 <= nByteSize) && (nByteSize <= 8)) { bRet = TRUE; }  // ��������Ч��
    else { bRet = FALSE; }
    if (TRUE == GetHandleInformation(hcInfo->hCOM, &nFlag)) // �������Ч�ԣ����˿��Ѵ򿪣���������ͨ������ʹ������Ч
    {
        if (!GetCommState(hcInfo->hCOM, &dcbCOMState)) { bRet = FALSE; }
        if (TRUE == bRet)
        {
            dcbCOMState.ByteSize = nByteSize;
            if (!SetCommState(hcInfo->hCOM, &dcbCOMState)) { bRet = FALSE; }
        }
    }
    if (TRUE == bRet)
    {
        (void)ComboBoxSetCursel(hcInfo->hByteSize, nByteSize - 5);
        hcInfo->cfgCOM.ByteSize = nByteSize;
    }

    return bRet;
}
HANSTATIC BOOL s_HANCOMSetParity(HANPCOM hcInfo, BYTE nParity)
{
    BOOL bRet;
    DCB dcbCOMState;
    DWORD nFlag;

    if (nParity < ArrLen(g_pParity)) { bRet = TRUE; }
    else { bRet = FALSE; }
    if (TRUE == GetHandleInformation(hcInfo->hCOM, &nFlag)) // �������Ч�ԣ����˿��Ѵ򿪣���������ͨ������ʹ������Ч
    {
        if (!GetCommState(hcInfo->hCOM, &dcbCOMState)) { bRet = FALSE; }
        if (TRUE == bRet)
        {
            dcbCOMState.Parity = nParity;
            if (!SetCommState(hcInfo->hCOM, &dcbCOMState)) { bRet = FALSE; }
        }
    }
    if (TRUE == bRet)
    {
        (void)ComboBoxSetCursel(hcInfo->hParity, nParity);
        hcInfo->cfgCOM.Parity = nParity;
    }

    return bRet;
}
HANSTATIC BOOL s_HANCOMSetStopBits(HANPCOM hcInfo, BYTE nStopBits)
{
    BOOL bRet;
    DCB dcbCOMState;
    DWORD nFlag;

    if (nStopBits < ArrLen(g_pStopBits)) { bRet = TRUE; }
    else { bRet = FALSE; }
    if (TRUE == GetHandleInformation(hcInfo->hCOM, &nFlag)) // �������Ч�ԣ����˿��Ѵ򿪣���������ͨ������ʹ������Ч
    {
        if (!GetCommState(hcInfo->hCOM, &dcbCOMState)) { bRet = FALSE; }
        if (TRUE == bRet)
        {
            dcbCOMState.StopBits = nStopBits;
            if (!SetCommState(hcInfo->hCOM, &dcbCOMState)) { bRet = FALSE; }
        }
    }
    if (TRUE == bRet)
    {
        (void)ComboBoxSetCursel(hcInfo->hStopBits, nStopBits);
        hcInfo->cfgCOM.StopBits = nStopBits;
    }

    return bRet;
}
HANSTATIC HAN_errno_t s_HANCOMGetRevLen(HANPCCOM hcInfo, DWORD* pLen)
{
    HAN_errno_t nRet = RET_OK;
    DWORD nFlag;

    if (FALSE == GetHandleInformation(hcInfo->hCOM, &nFlag))
    {
        nRet = HCOM_NOT_OPEN;
    }
    else
    {
        *pLen = GetCOMDataRevCount(hcInfo->hCOM);
    }

    return nRet;
}
HANSTATIC HAN_errno_t s_HANCOMReadData(HANPCOM hcInfo, HANPCOMREADDATA rdReadData)
{
    HAN_errno_t nRet = RET_OK;
    DWORD nFlag;

    if (!GetHandleInformation(hcInfo->hCOM, &nFlag)) { nRet = HCOM_NOT_OPEN; }
    if (RET_OK == nRet)
    {
        nRet = ReadCOM(hcInfo->hCOM, rdReadData->pData, rdReadData->nBufSize, rdReadData->nRevSize, &(hcInfo->olRead));
    }

    return nRet;
}
HANSTATIC HAN_errno_t s_HANCOMWriteData(HANPCOM hcInfo, void* pData, DWORD nLen)
{
    HAN_errno_t nRet = RET_OK;
    DWORD nWord;

    if (FALSE == GetHandleInformation(hcInfo->hCOM, &nWord)) { nRet = HCOM_NOT_OPEN; }
    if (RET_OK == nRet)
    {
        if (FALSE == GetOverlappedResult(hcInfo->hCOM, &(hcInfo->olWrite), &nWord, FALSE))
        {
            nRet = HCOM_WRITING;
        }
    }
    if (RET_OK == nRet)
    {
        (void)WriteFile(hcInfo->hCOM, pData, nLen, NULL, &(hcInfo->olWrite));
    }

    return nRet;
}
#endif
