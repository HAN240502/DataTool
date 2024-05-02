#ifndef HAN_FILE_CONVERSION_WINDOW_H
#define HAN_FILE_CONVERSION_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "..\..\GlobalVariables.h"
#include "HAN_FileConversionTypedef.h"
#include "HAN_FileConversion.h"

#define HAN_FILE_CONVERSION_CLASS       TEXT("HAN_FileConversion")

#define HAN_FILE_CONVERSION_PATH_MAX    260

#define USER_SETTING_KEY_NAME           TEXT("UserConfig")

typedef enum {
    WM_FILECONVERSIONNOTUSED = WM_USER,
    WM_FILECONVERSIONGETCFG,                /* 获取配置，wParam：未使用，lParam：HANDATAINI[INI_FILE_CONVERSION_CFG_CNT] */
} FILECONVERSIONMESSAGE;

typedef struct tagFILECONVERSIONWINPARAM {
    FILECONVERSIONPARAM     fcParam;
    HINSTANCE               hDll;
    HANCHAR                 pDllPath[PATH_STR_SIZE];
} FILECONVERSIONWINPARAM, * PFILECONVERSIONWINPARAM;
typedef const FILECONVERSIONWINPARAM* PCFILECONVERSIONWINPARAM;

/* 注册文件转换窗口
 */
void RegisterHANFileConversion(HINSTANCE hInst);

/* 保存配置
 * hFileConversion          窗口句柄
 * pIniPath                 ini 文件路径
 */
void WriteFileConversionCfg(HWND hFileConversion, HANPCSTR pIniPath);

/* 初始化配置
 * pFcConfig                存放配置的结构体指针
 * pIniPath                 ini 文件路径
 */
void ReadConvertFileIniFile(PFILECONVERSIONCFG pFcConfig, HANPCSTR pIniPath);

/* 初始化文件转换参数
 * pFcConfig                存放配置的结构体指针
 * pParam                   存放解析出来的转换参数的结构体指针
 */
void InitFileConversionParam(PCFILECONVERSIONCFG pFcConfig, PFILECONVERSIONWINPARAM pWinParam);

#ifdef __cplusplus
}
#endif

#endif
