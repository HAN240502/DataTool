#include "HAN_ComTool.h"
#include "..\..\HAN_Lib\HAN_wingdi.h"

#define COMTOOL_COM_FRAME_BUF_SIZE          8192

#define COMTOOL_TEXT_WINDOW_LIMIT           80000

#define COMTOOL_READ_COM_TIMEOUT            10
#define COMTOOL_PRINT_COM_PERIOD            50
#define COMTOOL_DEF_USER_TIMEOUT            10

#define COMTOOL_WINDOW_DX                   10
#define COMTOOL_WINDOW_DY                   10

#define COMTOOL_DLU_WINDOW_DX               7
#define COMTOOL_DLU_WINDOW_DY               7
#define COMTOOL_DLU_TEXT_H                  13

#define COMTOOL_USER_BAUDRATE_DLU_W         100

#define COMTOOL_COMBOBOX_STYLE              (WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST)

typedef enum {
    WID_COMTOOL_TEXT = 1,
    WID_COMTOOL_OPEN_COM,
    WID_COMTOOL_COM_ID,
    WID_COMTOOL_BAUDRATE,
    WID_COMTOOL_USER_BAUDRATE_INPUT,
    WID_COMTOOL_USER_BAUDRATE_OK,
    WID_COMTOOL_BYTESIZE,
    WID_COMTOOL_STOPBITS,
    WID_COMTOOL_PARITY,
    WID_COMTOOL_HEX,
    WID_COMTOOL_PRINT_START_TIME,
    WID_COMTOOL_PRINT_END_TIME,
} COMTOOLWID;

typedef enum {
    COMTOOL_DATA_TYPE_DATA,
    COMTOOL_DATA_TYPE_START,
    COMTOOL_DATA_TYPE_CONTINUE,
    COMTOOL_DATA_TYPE_END,
} COMTOOLDATATYPE;

typedef enum {
    COMTOOL_UPDATE_COM_CFG_PORT     = 0x00000001,
    COMTOOL_UPDATE_COM_CFG_BAUDRATE = 0x00000002,
    COMTOOL_UPDATE_COM_CFG_BYTESIZE = 0x00000004,
    COMTOOL_UPDATE_COM_CFG_STOPBITS = 0x00000008,
    COMTOOL_UPDATE_COM_CFG_PARITY   = 0x00000010,
    COMTOOL_UPDATE_COM_CFG_ALL      = COMTOOL_UPDATE_COM_CFG_PORT
                                    | COMTOOL_UPDATE_COM_CFG_BAUDRATE
                                    | COMTOOL_UPDATE_COM_CFG_BYTESIZE
                                    | COMTOOL_UPDATE_COM_CFG_STOPBITS
                                    | COMTOOL_UPDATE_COM_CFG_PARITY,
} COMTOOLUPDATECOMCFG;

typedef uint32_t            COMTOOLSIZE;

typedef struct tagCOMSBPAPARAM {
    HANPCSTR                pText;
    BYTE                    value;
} COMSBPAPARAM, * PCOMSBPAPARAM;

typedef struct tagCOMTOOLEXTRA {
    HANDLE                  hHeap;
    HINSTANCE               hInst;
    HANDLE                  hComFile;
    HANDLE                  hReadComThread;
    HANDLE                  hPrintComThread;
    HANDLE                  hPrintComSemaphore;
    HWND                    hSelf;
    HWND                    hText;
    HWND                    hOpenCom;
    HWND                    hComId;
    HWND                    hBaudrate;
    HWND                    hByteSize;
    HWND                    hStopBits;
    HWND                    hParity;
    HWND                    hHex;
    HWND                    hPrintStartTime;
    HWND                    hPrintEndTime;
    HWND                    hUserBaudrateInput;
    HWND                    hUserBaudrateOk;
    HFONT                   hHexFont;
    HFONT                   hSysFont;
    COMCFG                  cfgCOM;
    COMTOOLSIZE             nTextWindowLen;
    DWORD                   msPrintComTime;
    DWORD                   msTimeOut;
    DWORD                   msTime;
    uint8_t                 iBuf;
    COMTOOLSIZE             pBufLen[2];
    uint8_t                 pBuf[2][COMTOOL_COM_FRAME_BUF_SIZE];
    HANCHAR                 pBufText[COMTOOL_COM_FRAME_BUF_SIZE * 3];
    BOOL                    bHex;
    BOOL                    bPrintStartTime;
    BOOL                    bPrintEndTime;
    COMTOOLSIZE             pLastDataTypeOffset[2];
    int                     nBaudrateListCurselId;
} COMTOOLEXTRA, * PCOMTOOLEXTRA;
typedef const COMTOOLEXTRA* PCCOMTOOLEXTRA;

static LRESULT CALLBACK ComToolWndProc(HWND hComTool, UINT message, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK ComToolUserBaudrateDialogProc(HWND hUserBaudrate, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT CreateAction(HWND hComTool, LPARAM lParam);
static void CommandAction(HWND hComTool, PCOMTOOLEXTRA ctInfo, WPARAM wParam, LPARAM lParam);
static HANSIZE OpenComAction(HWND hStateButton, HWND hChildButton, void* pParam);
static HANSIZE CloseComAction(HWND hStateButton, HWND hChildButton, void* pParam);
static void UpdateComCfg(PCOMTOOLEXTRA ctInfo, COMTOOLUPDATECOMCFG cfgMask);
static void UpdateCfgWindows(PCOMTOOLEXTRA ctInfo);
static void UpdatePortList(PCOMTOOLEXTRA ctInfo, uint32_t nTargetId);
static void SweepCOMAction(const LPBYTE nCOMId, HANPCSTR pCOMName, DWORD nCount, void* pParam);
static DWORD ReadComThread(PCOMTOOLEXTRA ctInfo);
static DWORD PrintComThread(PCOMTOOLEXTRA ctInfo);
static void ReadComGetByteSuccessAction(PCOMTOOLEXTRA ctInfo, uint8_t cByte);
static void ReadComGetByteTimeOutAction(PCOMTOOLEXTRA ctInfo);
static void ReadComRecordStart(PCOMTOOLEXTRA ctInfo);
static void SwapComBuf(PCOMTOOLEXTRA ctInfo);
static void PrintStartInfo(PCOMTOOLEXTRA ctInfo, PSYSTEMTIME pTime);
static void PrintEndInfo(PCOMTOOLEXTRA ctInfo, PSYSTEMTIME pTime);
static void PrintDataASCII(PCOMTOOLEXTRA ctInfo, const uint8_t* pBuf, COMTOOLSIZE nLen);
static void PrintDataHex(PCOMTOOLEXTRA ctInfo, const uint8_t* pBuf, COMTOOLSIZE nLen);
static void BaudrateAction(PCOMTOOLEXTRA ctInfo);
static void ByteSizeAction(PCOMTOOLEXTRA ctInfo);
static void StopBitsAction(PCOMTOOLEXTRA ctInfo);
static void ParityAction(PCOMTOOLEXTRA ctInfo);
static void HexButtonAction(PCOMTOOLEXTRA ctInfo);
static void PrintStartTimeButtonAction(PCOMTOOLEXTRA ctInfo);
static void PrintEndTimeButtonAction(PCOMTOOLEXTRA ctInfo);
static void TextWindowAppendText(PCOMTOOLEXTRA ctInfo, HANPCSTR pText, int nTextLen);
static DWORD CreateBaudrateDialog(PCOMTOOLEXTRA ctInfo);
static void BaudrateDialogInitAction(HWND hUserBaudrate, LPARAM lParam);
static void BaudrateDialogOkAction(PCOMTOOLEXTRA ctInfo, HWND hUserBaudrate);

static inline COMTOOLSIZE ComToolCpyData(void* pDest, void* pSrc, COMTOOLSIZE nSize);

static const HANPCSTR       g_pBaudrate[] = {
    TEXT("自定义"),
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
    TEXT("3000000"),
};
static const HANPCSTR       g_pByteSize[] = {
    TEXT("5"),
    TEXT("6"),
    TEXT("7"),
    TEXT("8"),
};
static const COMSBPAPARAM   g_pStopBits[] = {
    { .pText = TEXT("1"), .value = ONESTOPBIT, },
    { .pText = TEXT("1.5"), .value = ONE5STOPBITS, },
    { .pText = TEXT("2"), .value = TWOSTOPBITS, },
};
static const COMSBPAPARAM   g_pParity[] = {
    { .pText = TEXT("NONE"), .value = NOPARITY, },
    { .pText = TEXT("ODD"), .value = ODDPARITY, },
    { .pText = TEXT("EVEN"), .value = EVENPARITY, },
    { .pText = TEXT("MARK"), .value = MARKPARITY, },
    { .pText = TEXT("SPACE"), .value = SPACEPARITY, },
};
static const COMTOOLEXTRA g_ftDefaultCfg = {
    .cfgCOM = {
        .id = 1,
        .dwFlagsAndAttributes = 0,
        .BaudRate = 115200,
        .ByteSize = 8,
        .dwInQueue = 1024,
        .dwOutQueue = 1024,
        .Parity = NOPARITY,
        .StopBits = ONESTOPBIT,
        .ReadIntervalTimeout = 0,
        .ReadTotalTimeoutConstant = COMTOOL_READ_COM_TIMEOUT,
        .ReadTotalTimeoutMultiplier = 1,
        .WriteTotalTimeoutConstant = 0,
        .WriteTotalTimeoutMultiplier = 0,
    },
    .hComFile = INVALID_HANDLE_VALUE,
    .nTextWindowLen = 0,
    .msTime = 0,
    .msTimeOut = COMTOOL_DEF_USER_TIMEOUT,
    .bHex = FALSE,
    .bPrintStartTime = TRUE,
    .bPrintEndTime = TRUE,
};

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

static LRESULT CALLBACK ComToolWndProc(HWND hComTool, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lWndProcRet = 0;

    // 读取属性
    PCOMTOOLEXTRA ctInfo = (PCOMTOOLEXTRA)GetWindowLongPtr(hComTool, 0);

    switch (message) {
        case WM_CREATE: {
            lWndProcRet = CreateAction(hComTool, lParam);
            break;
        }
        case WM_COMMAND: {
            CommandAction(hComTool, ctInfo, wParam, lParam);
            break;
        }
        case WM_CTLCOLORSTATIC: {
            lWndProcRet = (INT_PTR)GetStockObject(WHITE_BRUSH);
            break;
        }

        default: {
            lWndProcRet = DefWindowProc(hComTool, message, wParam, lParam);
            break;
        }
    }

    return lWndProcRet;
}
static INT_PTR CALLBACK ComToolUserBaudrateDialogProc(HWND hUserBaudrate, UINT message, WPARAM wParam, LPARAM lParam)
{
    INT_PTR nRet = TRUE;
    
    // 读取属性
    PCOMTOOLEXTRA ctInfo = (PCOMTOOLEXTRA)GetWindowLongPtr(hUserBaudrate, DWLP_USER);

    switch (message) {
        case WM_INITDIALOG: {
            BaudrateDialogInitAction(hUserBaudrate, lParam);
            break;
        }
        case WM_COMMAND: {
            switch(LOWORD(wParam)) {
                case WID_COMTOOL_USER_BAUDRATE_OK: {
                    BaudrateDialogOkAction(ctInfo, hUserBaudrate);
                    break;
                }

                default: {
                    break;
                }
            }
            break;
        }
        case WM_CLOSE: {
            EndDialog(hUserBaudrate, 0);
            break;
        }
        case WM_DESTROY: {
            EndDialog(hUserBaudrate, 0);
            break;
        }

        default: {
            nRet = FALSE;
            break;
        }
    }

    return nRet;
}

static LRESULT CreateAction(HWND hComTool, LPARAM lParam)
{
    LRESULT lWndProcRet = 0;
    PCOMTOOLEXTRA ctInfo;
    HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;
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

        *ctInfo = g_ftDefaultCfg;
        ctInfo->hInst = hInst;

        SetWindowLongPtr(hComTool, 0, (LONG_PTR)ctInfo);
        ctInfo->hHeap = hHeap;

        GetClientRect(hComTool, &rcClientSize);

        RegisterHANStateButton(hInst);

        ctInfo->hHexFont = CreateFontIndirect(&g_lfHexFont);
        ctInfo->hSysFont = CreateFontIndirect(&g_lfSysFont);

        ctInfo->hText = CreateWindow(TEXT("edit"), NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_READONLY | ES_MULTILINE, nWinX, nWinY, 500, 300,
            hComTool, (HMENU)WID_COMTOOL_TEXT, hInst, NULL);
        nWinY += 300 + COMTOOL_WINDOW_DY;
        ctInfo->hOpenCom = CreateWindow(HSTBT_CLASS, NULL,
            WS_CHILD | WS_VISIBLE, nWinX, nWinY, 100, 30,
            hComTool, (HMENU)WID_COMTOOL_OPEN_COM, hInst, ctInfo);
        nWinX += 100 + COMTOOL_WINDOW_DX;
        nWinY += 4;
        ctInfo->hComId = CreateWindow(TEXT("combobox"), NULL,
            COMTOOL_COMBOBOX_STYLE, nWinX, nWinY, 200, 100,
            hComTool, (HMENU)WID_COMTOOL_COM_ID, hInst, ctInfo);

        nWinX = COMTOOL_WINDOW_DX;
        nWinY += 26 + COMTOOL_WINDOW_DY;
        ctInfo->hBaudrate = CreateWindow(TEXT("combobox"), NULL,
            COMTOOL_COMBOBOX_STYLE, nWinX, nWinY, 180, 200,
            hComTool, (HMENU)WID_COMTOOL_BAUDRATE, hInst, NULL);
        nWinX += 180 + COMTOOL_WINDOW_DX;
        ctInfo->hByteSize = CreateWindow(TEXT("combobox"), NULL,
            COMTOOL_COMBOBOX_STYLE, nWinX, nWinY, 90, 200,
            hComTool, (HMENU)WID_COMTOOL_BYTESIZE, hInst, NULL);
        nWinX += 90 + COMTOOL_WINDOW_DX;
        ctInfo->hStopBits = CreateWindow(TEXT("combobox"), NULL,
            COMTOOL_COMBOBOX_STYLE, nWinX, nWinY, 90, 200,
            hComTool, (HMENU)WID_COMTOOL_STOPBITS, hInst, NULL);
        nWinX += 90 + COMTOOL_WINDOW_DX;
        ctInfo->hParity = CreateWindow(TEXT("combobox"), NULL,
            COMTOOL_COMBOBOX_STYLE, nWinX, nWinY, 90, 200,
            hComTool, (HMENU)WID_COMTOOL_PARITY, hInst, NULL);

        nWinX = COMTOOL_WINDOW_DX;
        nWinY += 26 + COMTOOL_WINDOW_DY;
        ctInfo->hHex = CreateWindow(TEXT("button"), TEXT("HEX"),
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, nWinX, nWinY, 55, 26,
            hComTool, (HMENU)WID_COMTOOL_HEX, hInst, NULL);
        nWinX += 55 + COMTOOL_WINDOW_DX;
        ctInfo->hPrintStartTime = CreateWindow(TEXT("button"), TEXT("打印开始时间"),
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, nWinX, nWinY, 140, 26,
            hComTool, (HMENU)WID_COMTOOL_PRINT_START_TIME, hInst, NULL);
        nWinX += 140 + COMTOOL_WINDOW_DX;
        ctInfo->hPrintEndTime = CreateWindow(TEXT("button"), TEXT("打印结束时间"),
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, nWinX, nWinY, 140, 26,
            hComTool, (HMENU)WID_COMTOOL_PRINT_END_TIME, hInst, NULL);

        SendMessage(ctInfo->hText, WM_SETFONT, (WPARAM)(ctInfo->hHexFont), (LPARAM)TRUE);
        SendMessage(ctInfo->hComId, WM_SETFONT, (WPARAM)(ctInfo->hHexFont), (LPARAM)TRUE);
        SendMessage(ctInfo->hOpenCom, WM_SETFONT, (WPARAM)(ctInfo->hSysFont), (LPARAM)TRUE);
        SendMessage(ctInfo->hBaudrate, WM_SETFONT, (WPARAM)(ctInfo->hHexFont), (LPARAM)TRUE);
        SendMessage(ctInfo->hByteSize, WM_SETFONT, (WPARAM)(ctInfo->hHexFont), (LPARAM)TRUE);
        SendMessage(ctInfo->hStopBits, WM_SETFONT, (WPARAM)(ctInfo->hHexFont), (LPARAM)TRUE);
        SendMessage(ctInfo->hParity, WM_SETFONT, (WPARAM)(ctInfo->hHexFont), (LPARAM)TRUE);
        SendMessage(ctInfo->hHex, WM_SETFONT, (WPARAM)(ctInfo->hSysFont), (LPARAM)TRUE);
        SendMessage(ctInfo->hPrintStartTime, WM_SETFONT, (WPARAM)(ctInfo->hSysFont), (LPARAM)TRUE);
        SendMessage(ctInfo->hPrintEndTime, WM_SETFONT, (WPARAM)(ctInfo->hSysFont), (LPARAM)TRUE);

        HSTBTSTATE buttonState[2] = {
            { .pText = TEXT("打开串口"), .Action = OpenComAction, },
            { .pText = TEXT("关闭串口"), .Action = CloseComAction },
        };
        ctInfo->hSelf = hComTool;
        SendMessage(ctInfo->hText, EM_SETLIMITTEXT, COMTOOL_TEXT_WINDOW_LIMIT, 0);
        HANStateButtonAddState(ctInfo->hOpenCom, &buttonState[0]);
        HANStateButtonAddState(ctInfo->hOpenCom, &buttonState[1]);
        ComboBoxAddStringArr(ctInfo->hBaudrate, g_pBaudrate, ArrLen(g_pBaudrate));
        ComboBoxAddStringArr(ctInfo->hByteSize, g_pByteSize, ArrLen(g_pByteSize));
        ComboBoxAddStringStructArr(ctInfo->hStopBits, &g_pStopBits, sizeof(COMSBPAPARAM), StructMemberOffset(COMSBPAPARAM, pText), ArrLen(g_pStopBits));
        ComboBoxAddStringStructArr(ctInfo->hParity, &g_pParity, sizeof(COMSBPAPARAM), StructMemberOffset(COMSBPAPARAM, pText), ArrLen(g_pParity));
        if (TRUE == ctInfo->bHex) { ButtonSetChecked(ctInfo->hHex); }
        if (TRUE == ctInfo->bPrintStartTime) { ButtonSetChecked(ctInfo->hPrintStartTime); }
        if (TRUE == ctInfo->bPrintEndTime) { ButtonSetChecked(ctInfo->hPrintEndTime); }
        UpdateCfgWindows(ctInfo);
    }

    return lWndProcRet;
}
static void CommandAction(HWND hComTool, PCOMTOOLEXTRA ctInfo, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam)) {
        case WID_COMTOOL_BAUDRATE: {
            if (CBN_SELCHANGE == HIWORD(wParam)) { BaudrateAction(ctInfo); }
            break;
        }
        case WID_COMTOOL_BYTESIZE: {
            if (CBN_SELCHANGE == HIWORD(wParam)) { ByteSizeAction(ctInfo); }
            break;
        }
        case WID_COMTOOL_STOPBITS: {
            if (CBN_SELCHANGE == HIWORD(wParam)) { StopBitsAction(ctInfo); }
            break;
        }
        case WID_COMTOOL_PARITY: {
            if (CBN_SELCHANGE == HIWORD(wParam)) { ParityAction(ctInfo); }
            break;
        }
        case WID_COMTOOL_HEX: {
            HexButtonAction(ctInfo);
            break;
        }
        case WID_COMTOOL_PRINT_START_TIME: {
            PrintStartTimeButtonAction(ctInfo);
            break;
        }
        case WID_COMTOOL_PRINT_END_TIME: {
            PrintEndTimeButtonAction(ctInfo);
            break;
        }

        default: {
            break;
        }
    }
}
static HANSIZE OpenComAction(HWND hStateButton, HWND hChildButton, void* pParam)
{
    HANSIZE nRet = 0;
    PCOMTOOLEXTRA ctInfo = pParam;
    HAN_errno_t eComRet;

    UpdateComCfg(ctInfo, COMTOOL_UPDATE_COM_CFG_ALL);

    eComRet = OpenCOM(&(ctInfo->hComFile), &(ctInfo->cfgCOM));
    switch (eComRet) {
        case RET_OK: {
            ctInfo->iBuf = 0;
            memset(ctInfo->pBufLen, 0, sizeof(ctInfo->pBufLen));
            memset(ctInfo->pLastDataTypeOffset, 0, sizeof(ctInfo->pLastDataTypeOffset));
            ctInfo->msPrintComTime = 0;
            ctInfo->hPrintComSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
            ctInfo->hReadComThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadComThread, (void*)ctInfo, 0, NULL);
            ctInfo->hPrintComThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PrintComThread, (void*)ctInfo, 0, NULL);
            nRet = 1;
            break;
        }

        default: {
            MessageBox(NULL, TEXT("打开串口失败，请检查是否被占用"), TEXT("错误"), 0);
            break;
        }
    }

    return nRet;
}
static HANSIZE CloseComAction(HWND hStateButton, HWND hChildButton, void* pParam)
{
    PCOMTOOLEXTRA ctInfo = pParam;

    if (INVALID_HANDLE_VALUE != ctInfo->hComFile)
    {
        CloseHandle(ctInfo->hComFile);
        CloseHandle(ctInfo->hReadComThread);
        CloseHandle(ctInfo->hPrintComThread);
        CloseHandle(ctInfo->hPrintComSemaphore);
        ctInfo->hComFile = INVALID_HANDLE_VALUE;
    }

    return 1;
}
static void UpdateComCfg(PCOMTOOLEXTRA ctInfo, COMTOOLUPDATECOMCFG cfgMask)
{
    int iCursel;
    HANCHAR pText[PATH_STR_SIZE];

    /* 更新端口号 */
    if (0 != (cfgMask & COMTOOL_UPDATE_COM_CFG_PORT))
    {
        GetWindowText(ctInfo->hComId, pText, PATH_STR_SIZE);
        if (0 == memcmp(pText, "COM", 3 * sizeof(HANCHAR))) { ctInfo->cfgCOM.id = HAN_strtoul(&pText[3], NULL, 10); }
    }
    /* 更新波特率 */
    if (0 != (cfgMask & COMTOOL_UPDATE_COM_CFG_BAUDRATE))
    {
        iCursel = ComboBoxGetCursel(ctInfo->hBaudrate);
        if (0 == iCursel)
        {
            DWORD nBaudrate = CreateBaudrateDialog(ctInfo);
            if (0 != nBaudrate)
            {
                ctInfo->cfgCOM.BaudRate = nBaudrate;
                HAN_snprintf(pText, PATH_STR_SIZE, TEXT("自定义：%lu"), nBaudrate);
                SendMessage(ctInfo->hBaudrate, CB_DELETESTRING, 0, 0);
                SendMessage(ctInfo->hBaudrate, CB_INSERTSTRING, 0, (LPARAM)pText);
                ComboBoxSetCursel(ctInfo->hBaudrate, 0);
            }
            else
            {
                ComboBoxSetCursel(ctInfo->hBaudrate, ctInfo->nBaudrateListCurselId);
            }
        }
        else
        {
            ctInfo->cfgCOM.BaudRate = HAN_strtoul(g_pBaudrate[iCursel], NULL, 10);
        }
        ctInfo->nBaudrateListCurselId = ComboBoxGetCursel(ctInfo->hBaudrate);
    }
    /* 更新字节大小 */
    if (0 != (cfgMask & COMTOOL_UPDATE_COM_CFG_BYTESIZE))
    {
        iCursel = ComboBoxGetCursel(ctInfo->hByteSize);
        ctInfo->cfgCOM.ByteSize = HAN_strtoul(g_pByteSize[iCursel], NULL, 10);
    }
    /* 更新停止位 */
    if (0 != (cfgMask & COMTOOL_UPDATE_COM_CFG_STOPBITS))
    {
        iCursel = ComboBoxGetCursel(ctInfo->hStopBits);
        ctInfo->cfgCOM.StopBits = g_pStopBits[iCursel].value;
    }
    /* 更新校验位 */
    if (0 != (cfgMask & COMTOOL_UPDATE_COM_CFG_PARITY))
    {
        iCursel = ComboBoxGetCursel(ctInfo->hParity);
        ctInfo->cfgCOM.Parity = g_pParity[iCursel].value;
    }
}
static void UpdateCfgWindows(PCOMTOOLEXTRA ctInfo)
{
    int iBaudrate = 7;
    int iByteSize = 3;
    int iStopBits = 0;
    int iParity = 0;

    UpdatePortList(ctInfo, ctInfo->cfgCOM.id);
    for (uint32_t i = 0; i < ArrLen(g_pBaudrate); i++) { if (ctInfo->cfgCOM.BaudRate == HAN_strtoul(g_pBaudrate[i], NULL, 10)) { iBaudrate = i; } }
    ComboBoxSetCursel(ctInfo->hBaudrate, iBaudrate);
    ctInfo->nBaudrateListCurselId = iBaudrate;
    for (uint32_t i = 0; i < ArrLen(g_pByteSize); i++) { if (ctInfo->cfgCOM.ByteSize == HAN_strtoul(g_pByteSize[i], NULL, 10)) { iByteSize = i; } }
    ComboBoxSetCursel(ctInfo->hByteSize, iByteSize);
    for (uint32_t i = 0; i < ArrLen(g_pStopBits); i++) { if (ctInfo->cfgCOM.StopBits == g_pStopBits[i].value) { iStopBits = i; } }
    ComboBoxSetCursel(ctInfo->hStopBits, iStopBits);
    for (uint32_t i = 0; i < ArrLen(g_pParity); i++) { if (ctInfo->cfgCOM.Parity == g_pParity[i].value) { iParity = i; } }
    ComboBoxSetCursel(ctInfo->hParity, iParity);
}
static void UpdatePortList(PCOMTOOLEXTRA ctInfo, uint32_t nTargetId)
{
    HANCHAR pPortId[PATH_STR_SIZE];

    while (SendMessage(ctInfo->hComId, CB_DELETESTRING, 0, 0) > 0) { }
    SweepCOMFromRegedit((void (*)(const LPBYTE, HANPCSTR, DWORD, void*))SweepCOMAction, ctInfo);

    if (0 < nTargetId)
    {
        HAN_snprintf(pPortId, PATH_STR_SIZE, "COM%d", nTargetId);
        int nBoxId = SendMessage(ctInfo->hComId, CB_FINDSTRING, -1, (LPARAM)pPortId);
        if (nBoxId != CB_ERR)
        {
            (void)ComboBoxSetCursel(ctInfo->hComId, nBoxId);
        }
        else
        {
            (void)ComboBoxSetCursel(ctInfo->hComId, 0);
        }
    }
}
static void SweepCOMAction(const LPBYTE nCOMId, HANPCSTR pCOMName, DWORD nCount, void* pParam)
{
    (void)nCount;
    PCOMTOOLEXTRA ctInfo = pParam;
    HANCHAR pPortIdText[PATH_STR_SIZE];
    HANPCSTR pCOMNameId = pCOMName;
    if (pCOMName == HAN_strstr(pCOMName, TEXT("\\Device\\"))) { pCOMNameId = &pCOMName[HAN_strlen(TEXT("\\Device\\"))]; }
    (void)HAN_snprintf(pPortIdText, PATH_STR_SIZE, TEXT("%s: %s"), nCOMId, pCOMNameId);
    (void)ComboBoxAddString(ctInfo->hComId, pPortIdText);
}
static DWORD ReadComThread(PCOMTOOLEXTRA ctInfo)
{
    DWORD nRevLen;
    DWORD msTime;
    DWORD msByteTime = 0;
    BOOL bEnd = FALSE;
    uint8_t cByte;
    
    while (INVALID_HANDLE_VALUE != ctInfo->hComFile)
    {
        ReadFile(ctInfo->hComFile, &cByte, 1, &nRevLen, NULL);
        msTime = GetTickCount();
        if (0 < nRevLen)    // 读到数据
        {
            if (FALSE == bEnd)
            {
                ReadComRecordStart(ctInfo);
                bEnd = TRUE;
            }
            ReadComGetByteSuccessAction(ctInfo, cByte);
        }
        else                // 未读到数据
        {
            if ((TRUE == bEnd) && (ctInfo->msTimeOut < (msTime - msByteTime)))
            {
                ReadComGetByteTimeOutAction(ctInfo);
                bEnd = FALSE;
            }
        }
        if (COMTOOL_PRINT_COM_PERIOD < (msTime - ctInfo->msPrintComTime))
        {
            ctInfo->msPrintComTime = msTime;
            SwapComBuf(ctInfo);
        }
    }

    return 0;
}
static DWORD PrintComThread(PCOMTOOLEXTRA ctInfo)
{
    SYSTEMTIME tTime;
    uint8_t iBuf;
    uint8_t* pBuf;
    COMTOOLSIZE nLen;
    COMTOOLSIZE iLoop;
    COMTOOLSIZE nSize;
    COMTOOLDATATYPE cDataType;

    while (1)
    {
        WaitForSingleObject(ctInfo->hPrintComSemaphore, INFINITE);

        iBuf = 1 - ctInfo->iBuf;
        nLen = ctInfo->pBufLen[iBuf];
        pBuf = ctInfo->pBuf[iBuf];
        iLoop = 0;
        
        while (iLoop < nLen)
        {
            iLoop += ComToolCpyData(&cDataType, &pBuf[iLoop], sizeof(cDataType));
            iLoop += ComToolCpyData(&nSize, &pBuf[iLoop], sizeof(nSize));
            
            switch (cDataType) {
                case COMTOOL_DATA_TYPE_START: {
                    iLoop += ComToolCpyData(&tTime, &pBuf[iLoop], sizeof(tTime));
                    if (TRUE == ctInfo->bPrintStartTime) { PrintStartInfo(ctInfo, &tTime); }
                    break;
                }
                case COMTOOL_DATA_TYPE_END: {
                    iLoop += ComToolCpyData(&tTime, &pBuf[iLoop], sizeof(tTime));
                    if (TRUE == ctInfo->bPrintEndTime) { PrintEndInfo(ctInfo, &tTime); }
                    break;
                }
                case COMTOOL_DATA_TYPE_DATA: {
                    if (0 < nSize)
                    {
                        if (FALSE == ctInfo->bHex) { PrintDataASCII(ctInfo, &pBuf[iLoop], nSize); }
                        else { PrintDataHex(ctInfo, &pBuf[iLoop], nSize); }
                        iLoop += nSize;
                    }
                    break;
                }

                default: {
                    iLoop += nSize;
                    break;
                }
            }
        }
    }

    return 0;
}
static void ReadComGetByteSuccessAction(PCOMTOOLEXTRA ctInfo, uint8_t cByte)
{
    uint8_t iBuf = ctInfo->iBuf;
    uint8_t* pBuf = ctInfo->pBuf[iBuf];
    COMTOOLSIZE nLen = ctInfo->pBufLen[iBuf];
    COMTOOLSIZE nSize;
    COMTOOLSIZE nOffset = ctInfo->pLastDataTypeOffset[iBuf];

    pBuf[nLen] = cByte;
    (void)memcpy(&nSize, &pBuf[nOffset + sizeof(COMTOOLDATATYPE)], sizeof(nSize));
    nSize++;
    (void)memcpy(&pBuf[nOffset + sizeof(COMTOOLDATATYPE)], &nSize, sizeof(nSize));
    ctInfo->pBufLen[iBuf]++;
}
static void ReadComGetByteTimeOutAction(PCOMTOOLEXTRA ctInfo)
{
    SYSTEMTIME tTime;
    uint8_t iBuf = ctInfo->iBuf;
    COMTOOLSIZE nLen = ctInfo->pBufLen[iBuf];
    COMTOOLSIZE nSize = sizeof(tTime);
    uint8_t* pBuf = ctInfo->pBuf[iBuf];
    COMTOOLDATATYPE cDataType;

    GetLocalTime(&tTime);

    ctInfo->pLastDataTypeOffset[iBuf] = nLen;
    cDataType = COMTOOL_DATA_TYPE_END;
    nLen += ComToolCpyData(&pBuf[nLen], &cDataType, sizeof(cDataType));
    nLen += ComToolCpyData(&pBuf[nLen], &nSize, sizeof(nSize));
    nLen += ComToolCpyData(&pBuf[nLen], &tTime, sizeof(tTime));
    ctInfo->pBufLen[iBuf] = nLen;
}
static void ReadComRecordStart(PCOMTOOLEXTRA ctInfo)
{
    SYSTEMTIME tTime;
    uint8_t iBuf = ctInfo->iBuf;
    COMTOOLSIZE nLen = ctInfo->pBufLen[iBuf];
    COMTOOLSIZE nSize;
    uint8_t* pBuf = ctInfo->pBuf[iBuf];
    COMTOOLDATATYPE cDataType;

    GetLocalTime(&tTime);

    cDataType = COMTOOL_DATA_TYPE_START;
    nSize = sizeof(tTime);
    nLen += ComToolCpyData(&pBuf[nLen], &cDataType, sizeof(cDataType));
    nLen += ComToolCpyData(&pBuf[nLen], &nSize, sizeof(nSize));
    nLen += ComToolCpyData(&pBuf[nLen], &tTime, sizeof(tTime));

    ctInfo->pLastDataTypeOffset[iBuf] = nLen;
    cDataType = COMTOOL_DATA_TYPE_DATA;
    nSize = 0;
    nLen += ComToolCpyData(&pBuf[nLen], &cDataType, sizeof(cDataType));
    nLen += ComToolCpyData(&pBuf[nLen], &nSize, sizeof(nSize));
    ctInfo->pBufLen[iBuf] = nLen;
}
static void SwapComBuf(PCOMTOOLEXTRA ctInfo)
{
    uint8_t iBuf = 1 - ctInfo->iBuf;
    COMTOOLSIZE nLen = 0;
    COMTOOLSIZE nSize;
    uint8_t* pBuf = ctInfo->pBuf[iBuf];
    COMTOOLDATATYPE cDataType;

    ctInfo->iBuf = iBuf;
    ctInfo->pBufLen[iBuf] = 0;
    ctInfo->pLastDataTypeOffset[iBuf] = 0;

    cDataType = COMTOOL_DATA_TYPE_DATA;
    nSize = 0;
    nLen += ComToolCpyData(&pBuf[nLen], &cDataType, sizeof(cDataType));
    nLen += ComToolCpyData(&pBuf[nLen], &nSize, sizeof(nSize));
    ctInfo->pBufLen[iBuf] = nLen;
    
    ReleaseSemaphore(ctInfo->hPrintComSemaphore, 1, NULL);
}
static void PrintStartInfo(PCOMTOOLEXTRA ctInfo, PSYSTEMTIME pTime)
{
    int nLen = HAN_snprintf(
        ctInfo->pBufText, sizeof(ctInfo->pBufText),
        TEXT("\r\n[%u:%u:%u.%u] 收 -> "),
        pTime->wHour, pTime->wMinute, pTime->wSecond, pTime->wMilliseconds
    );
    TextWindowAppendText(ctInfo, ctInfo->pBufText, nLen);
}
static void PrintEndInfo(PCOMTOOLEXTRA ctInfo, PSYSTEMTIME pTime)
{
    int nLen = HAN_snprintf(
        ctInfo->pBufText, sizeof(ctInfo->pBufText),
        TEXT("\r\n[%u:%u:%u.%u]\r\n"),
        pTime->wHour, pTime->wMinute, pTime->wSecond, pTime->wMilliseconds
    );
    TextWindowAppendText(ctInfo, ctInfo->pBufText, nLen);
}
static void PrintDataASCII(PCOMTOOLEXTRA ctInfo, const uint8_t* pBuf, COMTOOLSIZE nLen)
{
    COMTOOLSIZE nOffset = 0;
    for (COMTOOLSIZE i = 0; i < nLen; i++)
    {
        if (TEXT('\0') != pBuf[i]) { (void)memcpy(&ctInfo->pBufText[nOffset], &pBuf[i], 1); }
        else { (void)memcpy(&ctInfo->pBufText[nOffset], TEXT("\\0"), 2); nOffset++; }
        nOffset++;
    }
    ctInfo->pBufText[nOffset] = TEXT('\0');
    TextWindowAppendText(ctInfo, ctInfo->pBufText, nOffset);
}
static void PrintDataHex(PCOMTOOLEXTRA ctInfo, const uint8_t* pBuf, COMTOOLSIZE nLen)
{
    for (COMTOOLSIZE i = 0; i < nLen; i++)
    {
        HAN_snprintf(&ctInfo->pBufText[i * 3], sizeof(ctInfo->pBufText) - (i * 3),
            TEXT(" %02X"), pBuf[i]
        );
    }
    EditAppendText(ctInfo->hText, ctInfo->pBufText, FALSE);
}
static void BaudrateAction(PCOMTOOLEXTRA ctInfo)
{
    UpdateComCfg(ctInfo, COMTOOL_UPDATE_COM_CFG_BAUDRATE);
    if (INVALID_HANDLE_VALUE != ctInfo->hComFile)
    {
        DCB dcbCOMState;
        if (TRUE == GetCommState(ctInfo->hComFile, &dcbCOMState))
        {
            dcbCOMState.BaudRate = ctInfo->cfgCOM.BaudRate;
            SetCommState(ctInfo->hComFile, &dcbCOMState);
        }
    }
}
static void ByteSizeAction(PCOMTOOLEXTRA ctInfo)
{
    UpdateComCfg(ctInfo, COMTOOL_UPDATE_COM_CFG_BYTESIZE);
    if (INVALID_HANDLE_VALUE != ctInfo->hComFile)
    {
        DCB dcbCOMState;
        if (TRUE == GetCommState(ctInfo->hComFile, &dcbCOMState))
        {
            dcbCOMState.ByteSize = ctInfo->cfgCOM.ByteSize;
            SetCommState(ctInfo->hComFile, &dcbCOMState);
        }
    }
}
static void StopBitsAction(PCOMTOOLEXTRA ctInfo)
{
    UpdateComCfg(ctInfo, COMTOOL_UPDATE_COM_CFG_STOPBITS);
    if (INVALID_HANDLE_VALUE != ctInfo->hComFile)
    {
        DCB dcbCOMState;
        if (TRUE == GetCommState(ctInfo->hComFile, &dcbCOMState))
        {
            dcbCOMState.StopBits = ctInfo->cfgCOM.StopBits;
            SetCommState(ctInfo->hComFile, &dcbCOMState);
        }
    }
}
static void ParityAction(PCOMTOOLEXTRA ctInfo)
{
    UpdateComCfg(ctInfo, COMTOOL_UPDATE_COM_CFG_PARITY);
    if (INVALID_HANDLE_VALUE != ctInfo->hComFile)
    {
        DCB dcbCOMState;
        if (TRUE == GetCommState(ctInfo->hComFile, &dcbCOMState))
        {
            dcbCOMState.Parity = ctInfo->cfgCOM.Parity;
            SetCommState(ctInfo->hComFile, &dcbCOMState);
        }
    }
}
static void HexButtonAction(PCOMTOOLEXTRA ctInfo)
{
    int nCheck = ButtonGetCheck(ctInfo->hHex);
    if (BST_CHECKED == nCheck) { ctInfo->bHex = TRUE; }
    else { ctInfo->bHex = FALSE; }
    SetWindowText(ctInfo->hText, NULL);
}
static void PrintStartTimeButtonAction(PCOMTOOLEXTRA ctInfo)
{
    int nCheck = ButtonGetCheck(ctInfo->hPrintStartTime);
    if (BST_CHECKED == nCheck) { ctInfo->bPrintStartTime = TRUE; }
    else { ctInfo->bPrintStartTime = FALSE; }
}
static void PrintEndTimeButtonAction(PCOMTOOLEXTRA ctInfo)
{
    int nCheck = ButtonGetCheck(ctInfo->hPrintEndTime);
    if (BST_CHECKED == nCheck) { ctInfo->bPrintEndTime = TRUE; }
    else { ctInfo->bPrintEndTime = FALSE; }
}
static void TextWindowAppendText(PCOMTOOLEXTRA ctInfo, HANPCSTR pText, int nTextLen)
{
    if ((ctInfo->nTextWindowLen + nTextLen) < COMTOOL_TEXT_WINDOW_LIMIT)
    {
        EditAppendText(ctInfo->hText, pText, FALSE);
        ctInfo->nTextWindowLen += nTextLen;
    }
    else
    {
        SetWindowText(ctInfo->hText, pText);
        ctInfo->nTextWindowLen = nTextLen;
    }
}
static DWORD CreateBaudrateDialog(PCOMTOOLEXTRA ctInfo)
{
    DWORD nBaudrate;
    HGLOBAL hData;
    uint8_t* pDialogInfo;
    DLGTEMPLATE* pDlgInfoTitle;
    WORD* pWord;
    WCHAR* pText;
    uint8_t nOffset = 0;
    int nTextLen;

    hData = GlobalAlloc(GMEM_ZEROINIT, 1024);
    if (NULL != hData)
    {
        pDialogInfo = GlobalLock(hData);
        pDlgInfoTitle = (void*)(&pDialogInfo[nOffset]);
        pDlgInfoTitle->style = WS_POPUP | WS_BORDER | WS_SYSMENU | WS_BORDER | DS_MODALFRAME | WS_CAPTION;
        pDlgInfoTitle->cdit = 0;
        pDlgInfoTitle->x = 100;
        pDlgInfoTitle->y = 100;
        pDlgInfoTitle->cx = COMTOOL_USER_BAUDRATE_DLU_W;
        pDlgInfoTitle->cy = COMTOOL_DLU_TEXT_H * 2 + 3 * COMTOOL_DLU_WINDOW_DY,
        nOffset += sizeof(DLGTEMPLATE);

        pWord = (void*)(&pDialogInfo[nOffset]);
        pWord[0] = 0;
        pWord[1] = 0;
        nOffset += sizeof(*pWord) * 2;
        pText = (void*)(&pDialogInfo[nOffset]);
        nTextLen = 1 + MultiByteToWideChar(CP_ACP, 0, "自定义波特率", -1, pText, 50);
        nOffset += sizeof(WCHAR) * nTextLen;
        
        GlobalUnlock(hData);
        nBaudrate = (DWORD)DialogBoxIndirectParam(
            ctInfo->hInst, (LPDLGTEMPLATE)hData, ctInfo->hSelf,
            ComToolUserBaudrateDialogProc, (LPARAM)ctInfo
        );
        printf("%lu errno = %ld\n", nBaudrate, GetLastError());
        GlobalFree(hData);
    }
    else
    {
        nBaudrate = 0;
    }

    return nBaudrate;
}
static void BaudrateDialogInitAction(HWND hUserBaudrate, LPARAM lParam)
{
    HANCHAR pText[PATH_STR_SIZE];
    HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;
    PCOMTOOLEXTRA ctInfo = (PCOMTOOLEXTRA)lParam;
    SetWindowLongPtr(hUserBaudrate, DWLP_USER, (LONG_PTR)ctInfo);

    RECT rcInput = {
        .left = COMTOOL_DLU_WINDOW_DX, .right = COMTOOL_USER_BAUDRATE_DLU_W - COMTOOL_DLU_WINDOW_DX,
        .top = COMTOOL_DLU_WINDOW_DY, .bottom = COMTOOL_DLU_WINDOW_DX + COMTOOL_DLU_TEXT_H,
    };
    RECT rcOk = {
        .left = COMTOOL_USER_BAUDRATE_DLU_W - 50, .right = COMTOOL_USER_BAUDRATE_DLU_W - COMTOOL_DLU_WINDOW_DX,
        .top = COMTOOL_DLU_TEXT_H + (2 * COMTOOL_DLU_WINDOW_DY),
        .bottom = (COMTOOL_DLU_TEXT_H + COMTOOL_DLU_WINDOW_DY) * 2,
    };
    MapDialogRect(hUserBaudrate, &rcInput);
    MapDialogRect(hUserBaudrate, &rcOk);

    ctInfo->hUserBaudrateInput = CreateWindow(TEXT("edit"), NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
        rcInput.left, rcInput.top,
        GetRectW(&rcInput), GetRectH(&rcInput),
        hUserBaudrate, (HMENU)WID_COMTOOL_USER_BAUDRATE_INPUT, hInst, NULL);
    ctInfo->hUserBaudrateOk = CreateWindow(TEXT("button"), TEXT("确定"),
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        rcOk.left, rcOk.top,
        GetRectW(&rcOk), GetRectH(&rcOk),
        hUserBaudrate, (HMENU)WID_COMTOOL_USER_BAUDRATE_OK, hInst, NULL);

    SendMessage(ctInfo->hUserBaudrateInput, WM_SETFONT, (WPARAM)(ctInfo->hHexFont), (LPARAM)TRUE);
    SendMessage(ctInfo->hUserBaudrateOk, WM_SETFONT, (WPARAM)(ctInfo->hSysFont), (LPARAM)TRUE);
    
    HAN_snprintf(pText, PATH_STR_SIZE, TEXT("%lu"), ctInfo->cfgCOM.BaudRate);
    SetWindowText(ctInfo->hUserBaudrateInput, pText);
}
static void BaudrateDialogOkAction(PCOMTOOLEXTRA ctInfo, HWND hUserBaudrate)
{
    HANCHAR pText[PATH_STR_SIZE];
    DWORD nBaudrate;
    
    GetWindowText(ctInfo->hUserBaudrateInput, pText, PATH_STR_SIZE);
    nBaudrate = HAN_strtoul(pText, NULL, 10);

    EndDialog(hUserBaudrate, nBaudrate);
}

static inline COMTOOLSIZE ComToolCpyData(void* pDest, void* pSrc, COMTOOLSIZE nSize)
{
    (void)memcpy(pDest, pSrc, nSize);
    return nSize;
}
