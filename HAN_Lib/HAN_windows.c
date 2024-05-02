#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <windows.h>

#include "HAN_windows.h"

LPVOID HANWinHeapAlloc(HANDLE hHeap, void* pReAllocMem, SIZE_T dwBytes)
{
    LPVOID pRet;
    if (NULL == pReAllocMem) { pRet = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwBytes); }
    else { pRet = HeapReAlloc(hHeap, HEAP_ZERO_MEMORY, pReAllocMem, dwBytes); }
    return pRet;
}

/******************** 颜色 ********************/
void RGBToHSB(const COLORREF* crRGB, COLORHSB* crHSB)
{
    UINT8 pRGB[] = { GetRValue(*crRGB), GetGValue(*crRGB), GetBValue(*crRGB) };
    UINT8 idMax = 0;
    UINT8 idMin = 0;
    for (UINT8 i = 0; i < ArrLen(pRGB); i++)
    {
        if (pRGB[i] > pRGB[idMax]) { idMax = i; }
        if (pRGB[i] < pRGB[idMin]) { idMin = i; }
    }
    // H
    if (idMax == idMin) { crHSB->kHue = 0; }
    else if ((0 == idMax) && (pRGB[1] >= pRGB[2])) { crHSB->kHue = 60 * ((pRGB[1] - pRGB[2]) * 1.0 / (pRGB[idMax] - pRGB[idMin])); }
    else if ((0 == idMax) && (pRGB[1] < pRGB[2])) { crHSB->kHue = 60 * ((pRGB[1] - pRGB[2]) * 1.0 / (pRGB[idMax] - pRGB[idMin])) + 360; }
    else if (1 == idMax) { crHSB->kHue = (60 * ((pRGB[2] - pRGB[0]) * 1.0 / (pRGB[idMax] - pRGB[idMin]))) + 120; }
    else if (2 == idMax) { crHSB->kHue = (60 * ((pRGB[0] - pRGB[1]) * 1.0 / (pRGB[idMax] - pRGB[idMin]))) + 240; }
    else { }
    // S
    if (0 == pRGB[idMax]) { crHSB->kSaturation = 0; }
    else { crHSB->kSaturation = 1 - (pRGB[idMin] * 1.0 / pRGB[idMax]); }
    // B
    crHSB->kBrightness = pRGB[idMax] * 1.0 / 255;
}
void HSBToRGB(const COLORHSB* crHSB, COLORREF* crRGB)
{
    int nI = (int)floor(crHSB->kHue / 60) % 6;
    float nF = (crHSB->kHue / 60) - nI;
    float nV = (crHSB->kBrightness * 255);
    float nP = nV * (1 - crHSB->kSaturation);
    float nQ = nV * (1 - (nF * crHSB->kSaturation));
    float nT = nV * (1 - ((1.0F - nF) * crHSB->kSaturation));
    switch (nI) {
        case 0: {
            *crRGB = RGB(nV, nT, nP);
            break;
        }
        case 1: {
            *crRGB = RGB(nQ, nV, nP);
            break;
        }
        case 2: {
            *crRGB = RGB(nP, nV, nT);
            break;
        }
        case 3: {
            *crRGB = RGB(nP, nQ, nV);
            break;
        }
        case 4: {
            *crRGB = RGB(nT, nP, nV);
            break;
        }
        case 5: {
            *crRGB = RGB(nV, nP, nQ);
            break;
        }
        default: { break; }
    }
}

/******************** 共享内存 ********************/
void* ConnectToSharedMem(HANPCSTR pMemName, DWORD nSize, HANDLE* phFile)
{
    // 尝试打开一个内存映射文件
    HANDLE hRet = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, pMemName);
    void* ret = NULL;
    if (hRet != NULL)
    {
        // 打开成功，获取共享内存的指针
        ret = MapViewOfFile(hRet, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    }
    else
    {
        // 打开失败，创建内存映射文件
        hRet = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            nSize,
            pMemName
        );
        if (hRet == NULL) { ret = NULL; }
        else { ret = MapViewOfFile(hRet, FILE_MAP_ALL_ACCESS, 0, 0, 0); }
    }
    if ((hRet != NULL) && (phFile != NULL)) { *phFile = hRet; }
    return ret;
}

void DisconnectFromSharedMem(void* pMemory) { if (pMemory != NULL) { UnmapViewOfFile((LPCVOID)pMemory); } }

void CloseSharedMem(HANDLE hFile) { CloseHandle(hFile); }

/******************** 串口 ********************/
HAN_errno_t OpenCOM(HANDLE* phCOM, PCCOMCFG pCOMInfo)
{
    HAN_errno_t eRet = RET_OK;
    HANDLE hRet;
    HANCHAR pCOMName[16];
    (void)HAN_sprintf(pCOMName, TEXT("\\\\.\\COM%d"), pCOMInfo->id);
    
    hRet = CreateFile(
        pCOMName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        pCOMInfo->dwFlagsAndAttributes,
        NULL
    );
    if (hRet == INVALID_HANDLE_VALUE) { eRet = CREATE_COM_FILE_ERR; }

    if (RET_OK == eRet)
    {
        if (FALSE == SetupComm(hRet, pCOMInfo->dwInQueue, pCOMInfo->dwOutQueue))
        {
            eRet = SET_COM_QUEUE_ERR;
        }
    }

    if (RET_OK == eRet)
    {
        COMMTIMEOUTS ctTimeOut = {
            .ReadIntervalTimeout = pCOMInfo->ReadIntervalTimeout,
            .ReadTotalTimeoutConstant = pCOMInfo->ReadTotalTimeoutConstant,
            .ReadTotalTimeoutMultiplier = pCOMInfo->ReadTotalTimeoutMultiplier,
            .WriteTotalTimeoutConstant = pCOMInfo->WriteTotalTimeoutConstant,
            .WriteTotalTimeoutMultiplier = pCOMInfo->WriteTotalTimeoutMultiplier,
        };
        if (!SetCommTimeouts(hRet, &ctTimeOut)) { eRet = SET_COM_TIME_OUT_ERR; }
    }

    if (RET_OK == eRet)
    {
        DCB dcbCOMState;
        if (!GetCommState(hRet, &dcbCOMState)) { eRet = SET_COM_STATE_ERR; }
        if (RET_OK == eRet)
        {
            dcbCOMState.BaudRate = pCOMInfo->BaudRate;
            dcbCOMState.ByteSize = pCOMInfo->ByteSize;
            dcbCOMState.Parity = pCOMInfo->Parity;
            dcbCOMState.StopBits = pCOMInfo->StopBits;
            if (!SetCommState(hRet, &dcbCOMState)) { eRet = SET_COM_STATE_ERR; }
        }
    }

    if (RET_OK == eRet) { *phCOM = hRet; }
    
    return eRet;
}

DWORD GetCOMDataRevCount(HANDLE hCOM)
{
    DWORD nCOMErr;
    COMSTAT csCOMStat;
    ClearCommError(hCOM, &nCOMErr, &csCOMStat);
    return csCOMStat.cbInQue;
}

HAN_errno_t ReadCOM(HANDLE hCOM, void* pBuf, DWORD nBufSize, DWORD* nRevSize, LPOVERLAPPED lpOverlapped)
{
    DWORD nReadSize;
    HAN_errno_t ret;
    // 获取已接收的字节数
    DWORD nRevLen = GetCOMDataRevCount(hCOM);
    if (nBufSize == 0) { nReadSize = nRevLen; }
    else
    {
        if (nBufSize < nRevLen) { nReadSize = nBufSize; }
        else { nReadSize = nRevLen; }
    }
    // 开始接收
    if (lpOverlapped == NULL)
    {
        // 同步操作通过判断函数的返回值即可判断是否断开
        if (TRUE == ReadFile(hCOM, pBuf, nReadSize, nRevSize, NULL)) { ret = RET_OK; }
        else { ret = READ_COM_ERR; }
    }
    else
    {
        // 重叠操作需要通过 GetLastError 来判断是否断开
        (void)ReadFile(hCOM, pBuf, nReadSize, NULL, lpOverlapped);
        ret = RET_OK;
        while (!GetOverlappedResult(hCOM, lpOverlapped, nRevSize, FALSE))
        {
            // IO 操作未完成且 IO 状态非 ERROR_IO_PENDING 说明串口异常，可能断开
            if (GetLastError() != ERROR_IO_PENDING) { ret = READ_COM_ERR; break; }
        }
    }
    return ret;
}

DWORD SweepCOMFromRegedit(void (*SweepAction)(const LPBYTE pCOMId, HANPCSTR pCOMName, DWORD nCount, void* pParam), void* pParam)
{
    HKEY hCOMRegedit;
    DWORD ret = 0;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hCOMRegedit) == ERROR_SUCCESS)
    {
        BYTE pCOMId[PATH_STR_SIZE];
        HANCHAR pCOMName[PATH_STR_SIZE];
        DWORD nCOMIdLen = PATH_STR_SIZE;
        DWORD nCOMNameLen = PATH_STR_SIZE;
        while (RegEnumValue(hCOMRegedit, ret, pCOMName, &nCOMNameLen, NULL, NULL, pCOMId, &nCOMIdLen) != ERROR_NO_MORE_ITEMS)
        {
            SweepAction(pCOMId, pCOMName, ret, pParam);
            ret++;
            nCOMIdLen = PATH_STR_SIZE;
            nCOMNameLen = PATH_STR_SIZE;
        }
        RegCloseKey(hCOMRegedit);
    }
    return ret;
}

/******************** 窗口 ********************/
#define HANWIN_DEFAULT_CLASS    TEXT("HANWinClass")
#define HANWIN_DEFAULT_TITLE    TEXT("HANWinTitle")

int CreateMainWindow(HINSTANCE hInstance, int nCmdShow, HANPCWIN wWin)
{
    int nRet = 0;
    BOOL bRet = FALSE;
    if (NULL != wWin)
    {
        HANPCSTR cls = HANWIN_DEFAULT_CLASS;
        if (NULL != wWin->cls) { cls = wWin->cls; }
        WNDCLASSEX wcex = {
            .cbSize         = sizeof(WNDCLASSEX),
            .style          = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc    = wWin->WndProc,
            .cbClsExtra     = 0,
            .cbWndExtra     = 0,
            .hInstance      = hInstance,
            .hIcon          = LoadIcon(NULL,IDI_APPLICATION),
            .hCursor        = LoadCursor(NULL, IDC_ARROW),
            .hbrBackground  = wWin->background,
            .lpszMenuName   = NULL,
            .lpszClassName  = cls,
            .hIconSm        = NULL,
        };
        RegisterClassEx(&wcex);

        HANPCSTR title = HANWIN_DEFAULT_TITLE;
        if (NULL != wWin->title) { title = wWin->title; }
        HWND hWnd = CreateWindow(cls, title, wWin->style, wWin->x, wWin->y, wWin->w, wWin->h, NULL, NULL, hInstance, NULL);
        if (!hWnd)
        {
            bRet = TRUE;
        }
        if (FALSE == bRet)
        {
            ShowWindow(hWnd, nCmdShow);
            UpdateWindow(hWnd);

            MSG msg;
            // 主消息线程:
            while (FALSE != GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            nRet = (int)(msg.wParam);
        }
    }
    return nRet;
}

LONG GetRectW(const RECT* rcWin)
{
    return rcWin->right - rcWin->left;
}
LONG GetRectH(const RECT* rcWin)
{
    return rcWin->bottom - rcWin->top;
}
BOOL PosInRect(const POINT* ptPos, const RECT* rcRect)
{
    BOOL bRet;
    if ((ptPos->x >= rcRect->left) &&
        (ptPos->x <= rcRect->right) &&
        (ptPos->y >= rcRect->top) &&
        (ptPos->y <= rcRect->bottom)
    ) { bRet = TRUE; }
    else { bRet = FALSE; }
    return bRet;
}
BOOL SimilarRect(RECT* pDist, const RECT* pSource, const POINT* pAnchor1, const POINT* pAnchor2)
{
    BOOL bRet = TRUE;
    if ((pAnchor1->x == pAnchor2->x) || (pAnchor1->y == pAnchor2->y)) { bRet = FALSE; }
    if (TRUE == bRet)
    {
        LONG wPoint = pAnchor2->x - pAnchor1->x;
        LONG hPoint = pAnchor2->y - pAnchor1->y;
        double kSource = fabs((double)GetRectW(pSource) / (double)GetRectH(pSource));
        double kPoint = fabs((double)(pAnchor1->x - pAnchor2->x) / (double)(pAnchor1->y - pAnchor2->y));
        LONG wDist;
        LONG hDist;
        if (kSource > kPoint)
        {
            hDist = hPoint;
            wDist = (LONG)(hDist * kSource);
        }
        else
        {
            wDist = wPoint;
            hDist = (LONG)(wDist / kSource);
        }
        if (wDist < 0) { wDist = -wDist; }
        if (hDist < 0) { hDist = -hDist; }
        if (wPoint > 0)
        {
            pDist->left = pAnchor1->x;
            pDist->right = pDist->left + wDist;
        }
        else
        {
            pDist->right = pAnchor1->x;
            pDist->left = pDist->right - wDist;
        }
        if (hPoint > 0)
        {
            pDist->top = pAnchor1->y;
            pDist->bottom = pDist->top + hDist;
        }
        else
        {
            pDist->bottom = pAnchor1->y;
            pDist->top = pDist->bottom - hDist;
        }
    }
    return TRUE;
}
LONG GetWindowStyle(HWND hWnd)
{
    return GetWindowLong(hWnd, GWL_STYLE);
}

// button
int ButtonGetCheck(HWND hButton)
{
    return SendMessage(hButton, BM_GETCHECK, 0, 0);
}
void ButtonSetChecked(HWND hButton)
{
    SendMessage(hButton, BM_SETCHECK, BST_CHECKED, 0);
}
void ButtonSetIndeterminate(HWND hButton)
{
    SendMessage(hButton, BM_SETCHECK, BST_INDETERMINATE, 0);
}
void ButtonSetUnchecked(HWND hButton)
{
    SendMessage(hButton, BM_SETCHECK, BST_UNCHECKED, 0);
}
// edit
void EditSetLimitText(HWND hEdit, size_t Len)
{
    SendMessage(hEdit, EM_SETLIMITTEXT, Len, 0);
}
void EditSetSel(HWND hEdit, int StartChar, int EndChar)
{
    SendMessage(hEdit, EM_SETSEL, StartChar, EndChar);
}
void EditAppendText(HWND hEdit, HANPCSTR pString, BOOL Undo)
{
    SendMessage(hEdit, EM_SETSEL, -1, -1);
    SendMessage(hEdit, EM_REPLACESEL, (WPARAM)Undo, (LPARAM)pString);
}
// listbox
int ListBoxAddString(HWND hListBox, HANPCSTR pString)
{
    return SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pString);
}
int ListBoxAddStringArr(HWND hListBox, HANPCSTR* StringArr, size_t nMaxCount)
{
    int nRet = 0;
    for (size_t i = 0; i < nMaxCount; i++)
    {
        nRet = SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(StringArr[i]));
        if ((nRet == LB_ERR) || (nRet == LB_ERRSPACE)) { break; }
    }
    return nRet;
}
int ListBoxAddStringStructArr(HWND hListBox, const void* pStruct, size_t nStructSize, size_t nOffset, size_t nMaxCount)
{
    int nRet = 0;
    for (size_t i = 0; i < nMaxCount; i++)
    {
        nRet = SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(*(HANPCSTR*)(((BYTE*)pStruct) + (nStructSize * i) + nOffset)));
        if ((nRet == LB_ERR) || (nRet == LB_ERRSPACE)) { break; }
    }
    return nRet;
}
int ListBoxGetCursel(HWND hListBox)
{
    return SendMessage(hListBox, LB_GETCURSEL, 0, 0);
}
int ListBoxSetCursel(HWND hListBox, int Index)
{
    return SendMessage(hListBox, LB_SETCURSEL, (WPARAM)Index, 0);
}
// combobox
int ComboBoxAddString(HWND hComboBox, HANPCSTR pString)
{
    return SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)pString);
}
int ComboBoxAddStringArr(HWND hComboBox, const HANPCSTR* StringArr, size_t nMaxCount)
{
    int nRet = 0;
    for (size_t i = 0; i < nMaxCount; i++)
    {
        nRet = SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)(StringArr[i]));
        if ((nRet == CB_ERR) || (nRet == CB_ERRSPACE)) { break; }
    }
    return nRet;
}
int ComboBoxAddStringStructArr(HWND hComboBox, const void* pStruct, size_t nStructSize, size_t nOffset, size_t nMaxCount)
{
    int nRet = 0;
    for (size_t i = 0; i < nMaxCount; i++)
    {
        nRet = SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)(*(HANPCSTR*)(((BYTE*)pStruct) + (nStructSize * i) + nOffset)));
        if ((nRet == CB_ERR) || (nRet == CB_ERRSPACE)) { break; }
    }
    return nRet;
}
int ComboBoxDeleteString(HWND hComboBox, int Index)
{
    return SendMessage(hComboBox, CB_DELETESTRING, Index, 0);
}
void ComboBoxClearString(HWND hComboBox)
{
    while (0 < SendMessage(hComboBox, CB_DELETESTRING, 0, 0))
    { }
}
int ComboBoxGetCursel(HWND hComboBox)
{
    return SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
}
int ComboBoxSetCursel(HWND hComboBox, int Index)
{
    return SendMessage(hComboBox, CB_SETCURSEL, (WPARAM)Index, 0);
}
int ComboBoxFindString(HWND hComboBox, int StartIndex, HANPCSTR pString)
{
    return SendMessage(hComboBox, CB_FINDSTRING, StartIndex, (LPARAM)pString);
}
int ComboBoxFindStringEx(HWND hComboBox, int StartIndex, HANPCSTR pString)
{
    return SendMessage(hComboBox, CB_FINDSTRINGEXACT, StartIndex, (LPARAM)pString);
}
