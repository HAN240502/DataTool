#include <string.h>
#include <windows.h>
#include <CommCtrl.h>
#include <shlwapi.h>
#include <wingdi.h>
#include <shlobj.h>

#include "..\..\HAN_Lib\HAN_windows.h"
#include "..\..\GlobalVariables.h"
#include "HAN_FileConversionWindow.h"
#include "HAN_FileConversion.h"

#define MSG_FILE_PRO_DIV            ((DWORD)100)

#define CONVERT_TRY_MSG_CNT         (100)
#define CONVERT_MIN_MSG_CNT         (CONVERT_TRY_MSG_CNT * 0.9)

#define AUTO_PROTOCOL_FILTER_TEXT   TEXT("自动识别协议")

typedef enum {
    WID_FILE_CONVERSION_DLL_NAME,
    WID_FILE_CONVERSION_IMPORT_DLL,
    WID_FILE_CONVERSION_EXPORT_DLL,
    WID_FILE_CONVERSION_PATH_INPUT,
    WID_FILE_CONVERSION_PATH_CHOOSE,
    WID_FILE_CONVERSION_CONVERT_BUTTON,
    WID_FILE_CONVERSION_CONVERT_DIR,
    WID_FILE_CONVERSION_FILTER,
    WID_FILE_CONVERSION_USERSETTING,
    WID_FILE_CONVERSION_PROGRESS,
    WID_FILE_CONVERSION_REPORT,
} ENUMWID;

typedef FILECONVERSION*                 PFILECONVERSION;

typedef struct tagFILECONVERSIONEXTRA {
    HWND                    hWnd;
    HINSTANCE               hInst;
    HWND                    hHeap;
    HWND                    hDllName;
    HWND                    hImportDll;
    HWND                    hPathInput;
    HWND                    hPathChoose;
    HWND                    hConvertButton;
    HWND                    hConvertDir;
    HWND                    hFilter;
    HWND                    hUserSettingButton;
    HWND                    hUserSettingWindow;
    HWND                    hProgress;
    HWND                    hReport;
    HFONT                   hHexFont;
    HFONT                   hSysFont;
    HANCHAR                 pPathInput[PATH_STR_SIZE];
    FILECONVERSIONCFG       pFcConfig;
    FILECONVERSIONWINPARAM  fcWinParam;
} FILECONVERSIONEXTRA, * PFILECONVERSIONEXTRA;

static void s_GetFileConversionCfg(PFILECONVERSIONEXTRA fcInfo, HANPCSTR pIniPath);

static LRESULT CALLBACK FileConversionWndProc(HWND hFileConversion, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CreateAction(HWND hWnd, LPARAM lParam);
static void LoadDllOkAction(PFILECONVERSIONEXTRA fcInfo, HANPCSTR pDllPath);
static FILECONVERSIONERRNO LoadDllFile(PFILECONVERSIONEXTRA fcInfo, HANPCSTR pDllPath);
static void LoadFileConversionCfg(PFILECONVERSIONEXTRA fcInfo);
static void CommandAction(HWND hWnd, PFILECONVERSIONEXTRA fcInfo, WPARAM wParam, LPARAM lParam);
static void ImportDllAction(PFILECONVERSIONEXTRA fcInfo, BOOL bErrMsg);
static void ChoosePathAction(PFILECONVERSIONEXTRA fcInfo);
static void ConvertButtonAction(PFILECONVERSIONEXTRA fcInfo);
static void UserSettingAction(PFILECONVERSIONEXTRA fcInfo);
static void UpdateFilterList(PFILECONVERSIONEXTRA fcInfo);
static DWORD ConvertThread(PFILECONVERSIONEXTRA fcInfo);
static void ConvertDir(PFILECONVERSIONEXTRA fcInfo, HANPCSTR pDir, BOOL bConvertDir, int* pFilter);
static void DoConvertFile(PFILECONVERSIONEXTRA fcInfo, HANPCSTR pSrc, int* pFilter);
static BOOL ConvertAlloc(PFILECONVERSIONEXTRA fcInfo, size_t nBufSize);
static void WindowsWriteFile(void* pData, uint32_t nLen, void* pFile);
static void WindowPrintReport(HANPCSTR pReport, void* pUserParam);
static void WindowUpdateProgress(uint32_t i, uint32_t iMax, void* pUserParam);

void RegisterHANFileConversion(HINSTANCE hInst)
{
    WNDCLASSEX wcex = {
        .cbSize         = sizeof(WNDCLASSEX),
        .style          = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc    = FileConversionWndProc,
        .cbClsExtra     = 0,
        .cbWndExtra     = sizeof(PFILECONVERSIONEXTRA),
        .hInstance      = hInst,
        .hIcon          = LoadIcon(NULL,IDI_APPLICATION),
        .hCursor        = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName   = NULL,
        .lpszClassName  = HAN_FILE_CONVERSION_CLASS,
        .hIconSm        = NULL,
    };
    RegisterClassEx(&wcex);
}

void WriteFileConversionCfg(HWND hFileConversion, HANPCSTR pIniPath)
{
    SendMessage(hFileConversion, WM_FILECONVERSIONGETCFG, 0, (LPARAM)pIniPath);
}

void ReadConvertFileIniFile(PFILECONVERSIONCFG pFcConfig, HANPCSTR pIniPath)
{
    HANCHAR pText[INI_FILE_CONVERSION_VALUE_SIZE];
    for (uint32_t i = 0; i < INI_FILE_CONVERSION_CFG_CNT; i++)
    {
        GetPrivateProfileString(
            INI_FILE_CONVERSION_APP_NAME, pFcConfig->pSysConfig[i].pKey, pFcConfig->pSysConfig[i].pDefValue,
            pFcConfig->pSysConfig[i].pValue, INI_FILE_CONVERSION_VALUE_SIZE, pIniPath);
    }
    GetPrivateProfileString(INI_FILE_CONVERSION_APP_NAME, USER_SETTING_KEY_NAME TEXT("Cnt"), TEXT("0"), pText, INI_FILE_CONVERSION_VALUE_SIZE, pIniPath);
    uint32_t nUserConfigCnt = HAN_strtoul(pText, NULL, 10);
    if (FILE_CONVERSION_USER_SETTING_CFG_CNT_MAX < nUserConfigCnt) { nUserConfigCnt = FILE_CONVERSION_USER_SETTING_CFG_CNT_MAX; }
    for (uint32_t i = 0; i < nUserConfigCnt; i++)
    {
        HAN_snprintf(pText, ArrLen(pText), USER_SETTING_KEY_NAME TEXT("%u"), i + 1);
        GetPrivateProfileString(
            INI_FILE_CONVERSION_APP_NAME, pText, TEXT(""),
            pFcConfig->usUserConfig.pCfg[i], INI_FILE_CONVERSION_VALUE_SIZE, pIniPath);
    }
}

void InitFileConversionParam(PCFILECONVERSIONCFG pFcConfig, PFILECONVERSIONWINPARAM pWinParam)
{
    
}

static void s_GetFileConversionCfg(PFILECONVERSIONEXTRA fcInfo, HANPCSTR pIniPath)
{
    HANDATAINI pFcSysConfig[INI_FILE_CONVERSION_CFG_CNT];
    HANCHAR pText[INI_FILE_CONVERSION_VALUE_SIZE];
    PUSERSETTING pUserSetting = &(fcInfo->pFcConfig.usUserConfig);
    int bConvertDir = ButtonGetCheck(fcInfo->hConvertDir);
    int nFilterId  = ComboBoxGetCursel(fcInfo->hFilter);

    if (CB_ERR == nFilterId) { nFilterId = 0; }

    GetWindowText(fcInfo->hDllName, pFcSysConfig[INI_FILE_CONVERSION_DLL_PATH].pValue, INI_FILE_CONVERSION_VALUE_SIZE);
    GetWindowText(fcInfo->hPathInput, pFcSysConfig[INI_FILE_CONVERSION_CONVERT_PATH].pValue, INI_FILE_CONVERSION_VALUE_SIZE);
    if (BST_CHECKED == bConvertDir) { HAN_snprintf(pFcSysConfig[INI_FILE_CONVERSION_CONVERT_FOLDER].pValue, INI_FILE_CONVERSION_VALUE_SIZE, TEXT("TRUE")); }
    else { HAN_snprintf(pFcSysConfig[INI_FILE_CONVERSION_CONVERT_FOLDER].pValue, INI_FILE_CONVERSION_VALUE_SIZE, TEXT("FALSE")); }
    HAN_snprintf(pFcSysConfig[INI_FILE_CONVERSION_FILTER].pValue, INI_FILE_CONVERSION_VALUE_SIZE, TEXT("%d"), nFilterId);

    for (uint32_t i = 0; i < INI_FILE_CONVERSION_CFG_CNT; i++)
    {
        WritePrivateProfileString(
            INI_FILE_CONVERSION_APP_NAME, g_cfgHANData.cfgFileConversion.pSysConfig[i].pKey, pFcSysConfig[i].pValue, pIniPath
        );
    }

    if (NULL != fcInfo->fcWinParam.fcParam.pConvertMsgInfo)
    {
        if (NULL != fcInfo->fcWinParam.fcParam.pConvertMsgInfo->fcUserSetting.SaveUserSetting)
        {
            fcInfo->fcWinParam.fcParam.pConvertMsgInfo->fcUserSetting.SaveUserSetting(&(fcInfo->pFcConfig.usUserConfig));
            if (FILE_CONVERSION_USER_SETTING_CFG_CNT_MAX < pUserSetting->nCfgCnt) { pUserSetting->nCfgCnt = FILE_CONVERSION_USER_SETTING_CFG_CNT_MAX; }
            HAN_snprintf(pText, ArrLen(pText), TEXT("%u"), fcInfo->pFcConfig.usUserConfig.nCfgCnt);
            WritePrivateProfileString(INI_FILE_CONVERSION_APP_NAME, USER_SETTING_KEY_NAME TEXT("Cnt"), pText, pIniPath);
            for (uint32_t i = 0; i < pUserSetting->nCfgCnt; i++)
            {
                HAN_snprintf(pText, ArrLen(pText), USER_SETTING_KEY_NAME TEXT("%u"), i + 1);
                WritePrivateProfileString(INI_FILE_CONVERSION_APP_NAME, pText, fcInfo->pFcConfig.usUserConfig.pCfg[i], pIniPath);
            }
        }
    }
}

static LRESULT CALLBACK FileConversionWndProc(HWND hFileConversion, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lWndProcRet = 0;

    // 读取属性
    PFILECONVERSIONEXTRA fcInfo = (PFILECONVERSIONEXTRA)GetWindowLongPtr(hFileConversion, 0);

    switch (message) {
        case WM_CREATE: {
            lWndProcRet = CreateAction(hFileConversion, lParam);
            break;
        }
        case WM_COMMAND: {
            CommandAction(hFileConversion, fcInfo, wParam, lParam);
            break;
        }
        case WM_CTLCOLORSTATIC: {
            lWndProcRet = (INT_PTR)GetStockObject(WHITE_BRUSH);
            break;
        }
        
        case WM_FILECONVERSIONGETCFG: {
            s_GetFileConversionCfg(fcInfo, (HANPCSTR)lParam);
            break;
        }

        default: {
            return DefWindowProc(hFileConversion, message, wParam, lParam);
        }
    }

    return lWndProcRet;
}
static LRESULT CreateAction(HWND hWnd, LPARAM lParam)
{
    LRESULT lWndProcRet = 0;
    PFILECONVERSIONEXTRA fcInfo;
    HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;
    PFILECONVERSIONCFG pFcConfig = ((LPCREATESTRUCT)lParam)->lpCreateParams;
    RECT rcClientSize;

    HANDLE hHeap = GetProcessHeap();
    if (NULL == hHeap) { lWndProcRet = -1; }
    if (-1 != lWndProcRet)
    {
        fcInfo = (PFILECONVERSIONEXTRA)HANWinHeapAlloc(hHeap, NULL, sizeof(FILECONVERSIONEXTRA));
        if (NULL == fcInfo) { lWndProcRet = -1; }
    }

    if (-1 != lWndProcRet)
    {
        int nWinX;
        int nWinY;

        SetWindowLongPtr(hWnd, 0, (LONG_PTR)fcInfo);
        fcInfo->hHeap = hHeap;

        GetClientRect(hWnd, &rcClientSize);

        fcInfo->hHexFont = CreateFontIndirect(&g_lfHexFont);
        fcInfo->hSysFont = CreateFontIndirect(&g_lfSysFont);

        fcInfo->hDllName = CreateWindow(TEXT("edit"), NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY, 10, 10, 500, 30,
            hWnd, (HMENU)WID_FILE_CONVERSION_DLL_NAME, hInst, NULL);
        nWinX = 520; nWinY = 10;
        fcInfo->hImportDll = CreateWindow(TEXT("button"), TEXT("导入DLL文件"),
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, nWinX, nWinY, 120, 30,
            hWnd, (HMENU)WID_FILE_CONVERSION_IMPORT_DLL, hInst, NULL);
        nWinX = 10; nWinY += 40;
        fcInfo->hPathInput = CreateWindow(TEXT("edit"), NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, nWinX, nWinY, 500, 30,
            hWnd, (HMENU)WID_FILE_CONVERSION_PATH_INPUT, hInst, NULL);
        nWinX = 520;
        fcInfo->hPathChoose = CreateWindow(TEXT("button"), TEXT("选择目录"),
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, nWinX, nWinY, 120, 30,
            hWnd, (HMENU)WID_FILE_CONVERSION_PATH_CHOOSE, hInst, NULL);
        nWinX = 10; nWinY += 40;
        fcInfo->hConvertButton = CreateWindow(TEXT("button"), TEXT("解析"),
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, nWinX, nWinY, 120, 30,
            hWnd, (HMENU)WID_FILE_CONVERSION_CONVERT_BUTTON, hInst, NULL);
        nWinX += 130;
        fcInfo->hConvertDir = CreateWindow(TEXT("button"), TEXT("解析文件夹"),
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, nWinX, nWinY, 120, 30,
            hWnd, (HMENU)WID_FILE_CONVERSION_CONVERT_DIR, hInst, NULL);
        nWinX += 130; nWinY += 3;
        fcInfo->hFilter = CreateWindow(TEXT("combobox"), NULL,
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, nWinX, nWinY, 150, 200,
            hWnd, (HMENU)WID_FILE_CONVERSION_FILTER, hInst, NULL);
        nWinX += 160; nWinY -= 3;
        fcInfo->hUserSettingButton = CreateWindow(TEXT("button"), TEXT("自定义设置"),
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, nWinX, nWinY, 120, 30,
            hWnd, (HMENU)WID_FILE_CONVERSION_USERSETTING, hInst, NULL);
        fcInfo->hUserSettingWindow = NULL;
        nWinX = 10; nWinY += 40;
        fcInfo->hProgress = CreateWindow(PROGRESS_CLASS, NULL,
            WS_CHILD | WS_VISIBLE, nWinX, nWinY, 500, 30,
            hWnd, (HMENU)WID_FILE_CONVERSION_PROGRESS, hInst, NULL);
        nWinY += 40;
        fcInfo->hReport = CreateWindow(TEXT("edit"), NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL, nWinX, nWinY, 500, 300,
            hWnd, (HMENU)WID_FILE_CONVERSION_REPORT, hInst, NULL);

        SendMessage(fcInfo->hDllName, WM_SETFONT, (WPARAM)(fcInfo->hSysFont), (LPARAM)TRUE);
        SendMessage(fcInfo->hImportDll, WM_SETFONT, (WPARAM)(fcInfo->hSysFont), (LPARAM)TRUE);
        SendMessage(fcInfo->hPathInput, WM_SETFONT, (WPARAM)(fcInfo->hSysFont), (LPARAM)TRUE);
        SendMessage(fcInfo->hPathChoose, WM_SETFONT, (WPARAM)(fcInfo->hSysFont), (LPARAM)TRUE);
        SendMessage(fcInfo->hConvertButton, WM_SETFONT, (WPARAM)(fcInfo->hSysFont), (LPARAM)TRUE);
        SendMessage(fcInfo->hConvertDir, WM_SETFONT, (WPARAM)(fcInfo->hSysFont), (LPARAM)TRUE);
        SendMessage(fcInfo->hFilter, WM_SETFONT, (WPARAM)(fcInfo->hSysFont), (LPARAM)TRUE);
        SendMessage(fcInfo->hUserSettingButton, WM_SETFONT, (WPARAM)(fcInfo->hSysFont), (LPARAM)TRUE);
        SendMessage(fcInfo->hReport, WM_SETFONT, (WPARAM)(fcInfo->hSysFont), (LPARAM)TRUE);

        ButtonSetChecked(fcInfo->hConvertDir);
        UpdateFilterList(fcInfo);

        fcInfo->hWnd = hWnd;
        fcInfo->hInst = hInst;
        fcInfo->fcWinParam.hDll = NULL;
        fcInfo->fcWinParam.fcParam.pConvertMsgInfo = NULL;
        fcInfo->fcWinParam.fcParam.pUserSetting = &(fcInfo->pFcConfig.usUserConfig);
        fcInfo->fcWinParam.fcParam.WriteFile = WindowsWriteFile;
        fcInfo->fcWinParam.fcParam.PrintReport = WindowPrintReport;
        fcInfo->fcWinParam.fcParam.UpdateProgress = WindowUpdateProgress;
        fcInfo->fcWinParam.fcParam.pUserParam = fcInfo;
        
        if (NULL != pFcConfig)
        {
            (void)memcpy(&(fcInfo->pFcConfig), pFcConfig, sizeof(fcInfo->pFcConfig));
            LoadFileConversionCfg(fcInfo);
        }
    }

    return lWndProcRet;
}
static void LoadDllOkAction(PFILECONVERSIONEXTRA fcInfo, HANPCSTR pDllPath)
{
    SetWindowText(fcInfo->hDllName, pDllPath);
    UpdateFilterList(fcInfo);
    if (NULL != fcInfo->fcWinParam.fcParam.pConvertMsgInfo->fcUserSetting.InitUserSetting)
    {
        fcInfo->fcWinParam.fcParam.pConvertMsgInfo->fcUserSetting.InitUserSetting(&(fcInfo->pFcConfig.usUserConfig), fcInfo->hWnd, fcInfo->hInst);
    }
}
static FILECONVERSIONERRNO LoadDllFile(PFILECONVERSIONEXTRA fcInfo, HANPCSTR pDllPath)
{
    FILECONVERSIONERRNO eRet = FILECONVERSION_ERRNO_OK;
    HINSTANCE hDll = LoadLibrary(pDllPath);
    if (NULL == hDll) { eRet = FILECONVERSION_ERRNO_OPEN_DLL_FAIL; }
    else
    {
        BOOL bGet = TRUE;
        PFILECONVERSIONINFO pConvertMsgInfo = (void*)GetProcAddress(hDll, "g_pFileConversionInfo");
        PFILECONVERSION pConvertMsg = pConvertMsgInfo->pFileConversion;

        if (NULL == pConvertMsgInfo) { bGet = FALSE; }
        if (NULL == pConvertMsgInfo->OpenSrcFileAction) { bGet = FALSE; }
        for (uint32_t i = 0; i < (pConvertMsgInfo->nFileConversionCnt); i++)
        {
            if (NULL == pConvertMsg->GetHeaderAction) { bGet = FALSE; }
        }
        if (TRUE == bGet)
        {
            if (pConvertMsgInfo->nFileConversionCnt <= HAN_FILE_CONVERSION_INFO_MAX)
            {
                if (NULL != fcInfo->fcWinParam.hDll) { FreeLibrary(fcInfo->fcWinParam.hDll); }
                fcInfo->fcWinParam.hDll = hDll;
                fcInfo->fcWinParam.fcParam.pConvertMsgInfo = pConvertMsgInfo;
            }
            else
            {
                eRet = FILECONVERSION_ERRNO_MSG_CNT_ERR;
                bGet = FALSE;
            }
        }
        else
        {
            eRet = FILECONVERSION_ERRNO_NOT_COMPATIBLE;
            FreeLibrary(hDll);
        }
    }
    return eRet;
}
static void LoadFileConversionCfg(PFILECONVERSIONEXTRA fcInfo)
{
    PHANDATAINI pFcConfig = fcInfo->pFcConfig.pSysConfig;
    /* 加载 DLL */
    if (TRUE == PathFileExists(pFcConfig[INI_FILE_CONVERSION_DLL_PATH].pValue))
    {
        if (FILECONVERSION_ERRNO_OK == LoadDllFile(fcInfo, pFcConfig[INI_FILE_CONVERSION_DLL_PATH].pValue))
        {
            LoadDllOkAction(fcInfo, pFcConfig[INI_FILE_CONVERSION_DLL_PATH].pValue);
        }
    }
    SetWindowText(fcInfo->hPathInput, pFcConfig[INI_FILE_CONVERSION_CONVERT_PATH].pValue);
    if (0 == HAN_strcmp(pFcConfig[INI_FILE_CONVERSION_CONVERT_FOLDER].pValue, TEXT("FALSE"))) { ButtonSetUnchecked(fcInfo->hConvertDir); }
    else { ButtonSetChecked(fcInfo->hConvertDir); }
    int iFilterId = HAN_strtoul(pFcConfig[INI_FILE_CONVERSION_FILTER].pValue, NULL, 10);
    int nFilterCnt = SendMessage(fcInfo->hFilter, CB_GETCOUNT, 0, 0);
    if ((CB_ERR == nFilterCnt) || (nFilterCnt <= iFilterId)) { iFilterId = 0; }
    ComboBoxSetCursel(fcInfo->hFilter, iFilterId);
}
static void CommandAction(HWND hWnd, PFILECONVERSIONEXTRA fcInfo, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam)) {
        case WID_FILE_CONVERSION_IMPORT_DLL: {
            ImportDllAction(fcInfo, TRUE);
            break;
        }
        case WID_FILE_CONVERSION_PATH_CHOOSE: {
            ChoosePathAction(fcInfo);
            break;
        }
        case WID_FILE_CONVERSION_CONVERT_BUTTON: {
            ConvertButtonAction(fcInfo);
            break;
        }
        case WID_FILE_CONVERSION_USERSETTING: {
            UserSettingAction(fcInfo);
            break;
        }
        default: {
            break;
        }
    }
}
static void ImportDllAction(PFILECONVERSIONEXTRA fcInfo, BOOL bErrMsg)
{
    HANCHAR pInputName[PATH_STR_SIZE] = { 0 };
    OPENFILENAME ofnOpenFile;
    ZeroMemory(&ofnOpenFile, sizeof(ofnOpenFile));
    ofnOpenFile.lStructSize = sizeof(ofnOpenFile);
    ofnOpenFile.hwndOwner = NULL;
    ofnOpenFile.lpstrFilter = TEXT("DLL\0*.DLL\0\0");
    ofnOpenFile.nFilterIndex = 0;
    ofnOpenFile.lpstrFile = pInputName;
    ofnOpenFile.nMaxFile = PATH_STR_SIZE;
    ofnOpenFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
    if (GetOpenFileName(&ofnOpenFile))
    {
        fcInfo->fcWinParam.fcParam.pUserSetting = &(fcInfo->pFcConfig.usUserConfig);
        switch(LoadDllFile(fcInfo, pInputName)) {
            case FILECONVERSION_ERRNO_OK: {
                LoadDllOkAction(fcInfo, pInputName);
                break;
            }
            case FILECONVERSION_ERRNO_OPEN_DLL_FAIL: {
                MessageBox(NULL, TEXT("打开DLL文件失败"), NULL, 0);
                break;
            }
            case FILECONVERSION_ERRNO_MSG_CNT_ERR: {
                MessageBox(NULL, TEXT("报文数量超出上限"), NULL, 0);
                break;
            }
            case FILECONVERSION_ERRNO_NOT_COMPATIBLE: {
                MessageBox(NULL, TEXT("DLL文件不兼容"), NULL, 0);
                break;
            }
            
            default: {
                break;
            }
        }
    }
}
static void ChoosePathAction(PFILECONVERSIONEXTRA fcInfo)
{
    HANCHAR pPath[PATH_STR_SIZE];
    BROWSEINFO biBrowsInfo = {
        .hwndOwner = NULL,
        .pidlRoot = NULL,
        .pszDisplayName = pPath,
        .lpszTitle = TEXT("选择目录"),
        .ulFlags = BIF_USENEWUI,
        .lpfn = NULL,
        .lParam = 0,
        .iImage = 0,
    };
    if (NULL != SHBrowseForFolder(&biBrowsInfo))
    {
        SetWindowText(fcInfo->hPathInput, pPath);
    }
}
static void ConvertButtonAction(PFILECONVERSIONEXTRA fcInfo)
{
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConvertThread, (void*)fcInfo, 0, NULL);
}
static void UserSettingAction(PFILECONVERSIONEXTRA fcInfo)
{
    PFILECONVERSIONUSERSETTING pUserSetting = &(fcInfo->fcWinParam.fcParam.pConvertMsgInfo->fcUserSetting);

    if (NULL != pUserSetting->UserSettingButtonAction)
    {
        pUserSetting->UserSettingButtonAction(&fcInfo->pFcConfig.usUserConfig);
    }
}
static void UpdateFilterList(PFILECONVERSIONEXTRA fcInfo)
{
    ComboBoxClearString(fcInfo->hFilter);
    ComboBoxAddString(fcInfo->hFilter, AUTO_PROTOCOL_FILTER_TEXT);
    if (NULL != fcInfo->fcWinParam.fcParam.pConvertMsgInfo)
    {
        ComboBoxAddStringStructArr(
            fcInfo->hFilter, fcInfo->fcWinParam.fcParam.pConvertMsgInfo->pFileConversion, sizeof((fcInfo->fcWinParam.fcParam.pConvertMsgInfo->pFileConversion)[0]),
            StructMemberOffset(FILECONVERSION, pMsgName), fcInfo->fcWinParam.fcParam.pConvertMsgInfo->nFileConversionCnt);
    }
    ComboBoxSetCursel(fcInfo->hFilter, 0);
}
static DWORD ConvertThread(PFILECONVERSIONEXTRA fcInfo)
{
    GetWindowText(fcInfo->hPathInput, fcInfo->pPathInput, ArrLen(fcInfo->pPathInput));
    if (NULL != fcInfo->fcWinParam.fcParam.pConvertMsgInfo)
    {
        if (0 < HAN_strlen(fcInfo->pPathInput))
        {
            /* 禁用解析按钮，防止重复触发 */
            EnableWindow(fcInfo->hConvertButton, FALSE);
            /* 清空 log 窗口 */
            SetWindowText(fcInfo->hReport, TEXT(""));
            /* 确认过滤类型，自动过滤设置 pFilter 为 NULL，选择过滤设置 pFilter 指向 nFilter，并将 nFilter 调整至从零开始（nFilter--） */
            int nFilter = ComboBoxGetCursel(fcInfo->hFilter);
            int* pFilter = NULL;
            if (0 < nFilter) { nFilter--; pFilter = &nFilter; }
            /* 获取路径属性 */
            DWORD cFileAttributes = GetFileAttributes(fcInfo->pPathInput);
            if (0 == (cFileAttributes & FILE_ATTRIBUTE_DIRECTORY))  /* 解析文件 */
            {
                DoConvertFile(fcInfo, fcInfo->pPathInput, pFilter);
            }
            else    /* 解析目录 */
            {
                /* 确认是否需要解析子目录 */
                int nConvertDirCheck = ButtonGetCheck(fcInfo->hConvertDir);
                BOOL bConvertDir = FALSE;
                if (BST_CHECKED == nConvertDirCheck) { bConvertDir = TRUE; }
                /* 执行解析 */
                ConvertDir(fcInfo, fcInfo->pPathInput, bConvertDir, pFilter);
            }
            /* 解析完成后在 log 窗口打印提示 */
            EditAppendText(fcInfo->hReport, TEXT("所有文件解析完成\r\n"), FALSE);
            /* 解锁解析按钮 */
            EnableWindow(fcInfo->hConvertButton, TRUE);
        }
        else
        {
            MessageBox(NULL, TEXT("请输入路径"), NULL, 0);
        }
    }
    else
    {
        MessageBox(NULL, TEXT("请导入DLL文件"), NULL, 0);
    }
    return 0;
}
static void ConvertDir(PFILECONVERSIONEXTRA fcInfo, HANPCSTR pDir, BOOL bConvertDir, int* pFilter)
{
    HANCHAR pInput[PATH_STR_SIZE];
    HANCHAR pPath[PATH_STR_SIZE];
    HANCHAR pFile[PATH_STR_SIZE];
    WIN32_FIND_DATA wfdFindFile;

    (void)HAN_strcpy(pInput, pDir);
    if (TEXT('\\') != pInput[HAN_strlen(pInput) - (size_t)1]) { (void)HAN_strcat(pInput, TEXT("\\")); }

    (void)HAN_strcpy(pPath, pInput);
    (void)HAN_strcat(pPath, TEXT("*.*"));
    HANDLE hFile = FindFirstFile(pPath, &wfdFindFile);
    
    while (TRUE == FindNextFile(hFile, &wfdFindFile))
    {
        if ((0 != strcmp(wfdFindFile.cFileName, ".")) && (0 != strcmp(wfdFindFile.cFileName, "..")))
        {
            (void)HAN_strcpy(pFile, pInput);
            (void)HAN_strcat(pFile, wfdFindFile.cFileName);
            if (0 == (wfdFindFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                DoConvertFile(fcInfo, pFile, pFilter);
            }
            else
            {
                if (TRUE == bConvertDir) { ConvertDir(fcInfo, pFile, bConvertDir, pFilter); }
            }
        }
    }
}
static void DoConvertFile(PFILECONVERSIONEXTRA fcInfo, HANPCSTR pSrc, int* pFilter)
{
    BOOL bRet = FALSE;
    HANCHAR pDestFileName[PATH_STR_SIZE];
    HANDLE hSrc = CreateFile(pSrc, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    HANDLE hDest = INVALID_HANDLE_VALUE;

    if (FALSE == bRet)
    {
        HANPCSTR pFileName = &pSrc[HAN_strlen(fcInfo->pPathInput)];
        if (0 == HAN_strlen(pFileName)) { pFileName = PathFindFileName(fcInfo->pPathInput); }

        if (TRUE == fcInfo->fcWinParam.fcParam.pConvertMsgInfo->OpenSrcFileAction(pSrc, pDestFileName))
        {
            EditAppendText(fcInfo->hReport, pFileName, FALSE);
            EditAppendText(fcInfo->hReport, TEXT("\r\n"), FALSE);
            SendMessage(fcInfo->hProgress, PBM_SETPOS, 0, 0);
            EditAppendText(fcInfo->hReport, TEXT("打开文件中...\r\n"), FALSE);
            hDest = CreateFile(pDestFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

            if (INVALID_HANDLE_VALUE == hSrc) { EditAppendText(fcInfo->hReport, TEXT("打开源文件失败\r\n"), FALSE); }
            else if (INVALID_HANDLE_VALUE == hDest) { EditAppendText(fcInfo->hReport, TEXT("打开目标件失败\r\n"), FALSE); }
            else
            {
                /* 扩缓存 */
                DWORD nFileSize = GetFileSize(hSrc, NULL);
                bRet = ConvertAlloc(fcInfo, nFileSize);
                /* 解析数据 */
                if (TRUE == bRet)
                {
                    fcInfo->fcWinParam.fcParam.nSrcDataSize = nFileSize;
                    fcInfo->fcWinParam.fcParam.pDestFile = hDest;
                    fcInfo->fcWinParam.fcParam.pFilter = pFilter;

                    ReadFile(hSrc, fcInfo->fcWinParam.fcParam.pSrcData, nFileSize, NULL, NULL);
                    EditAppendText(fcInfo->hReport, TEXT("开始解析...\r\n"), FALSE);

                    ConvertFile(&(fcInfo->fcWinParam.fcParam));
                }
                else
                {
                    EditAppendText(fcInfo->hReport, TEXT("内存不足\r\n"), FALSE);
                }
            }
            EditAppendText(fcInfo->hReport, TEXT("\r\n"), FALSE);
        }
    }

    if (INVALID_HANDLE_VALUE != hSrc) { (void)CloseHandle(hSrc); }
    if (INVALID_HANDLE_VALUE != hDest) { (void)CloseHandle(hDest); }
}
static BOOL ConvertAlloc(PFILECONVERSIONEXTRA fcInfo, size_t nBufSize)
{
    BOOL bRet = TRUE;

    if (fcInfo->fcWinParam.fcParam.nSrcBufSize < nBufSize)
    {
        uint8_t* pTemp;
        if (NULL == fcInfo->fcWinParam.fcParam.pSrcData) { pTemp = HeapAlloc(fcInfo->hHeap, 0, nBufSize); }
        else { pTemp = HeapReAlloc(fcInfo->hHeap, 0, fcInfo->fcWinParam.fcParam.pSrcData, nBufSize); }
        if (NULL != pTemp)
        {
            fcInfo->fcWinParam.fcParam.pSrcData = pTemp;
            fcInfo->fcWinParam.fcParam.nSrcBufSize = nBufSize;
        }
        else { bRet = FALSE; }
    }

    return bRet;
}
static void WindowsWriteFile(void* pData, uint32_t nLen, void* pFile)
{
    WriteFile(pFile, pData, nLen, NULL, NULL);
}
static void WindowPrintReport(HANPCSTR pReport, void* pUserParam)
{
    PFILECONVERSIONEXTRA fcInfo = (PFILECONVERSIONEXTRA)pUserParam;
    EditAppendText(fcInfo->hReport, pReport, FALSE);
}
static void WindowUpdateProgress(uint32_t i, uint32_t iMax, void* pUserParam)
{
    PFILECONVERSIONEXTRA fcInfo = (PFILECONVERSIONEXTRA)pUserParam;
    SendMessage(fcInfo->hProgress, PBM_SETPOS, i, 0);
}
