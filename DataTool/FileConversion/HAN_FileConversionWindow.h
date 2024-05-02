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
    WM_FILECONVERSIONGETCFG,                /* ��ȡ���ã�wParam��δʹ�ã�lParam��HANDATAINI[INI_FILE_CONVERSION_CFG_CNT] */
} FILECONVERSIONMESSAGE;

typedef struct tagFILECONVERSIONWINPARAM {
    FILECONVERSIONPARAM     fcParam;
    HINSTANCE               hDll;
    HANCHAR                 pDllPath[PATH_STR_SIZE];
} FILECONVERSIONWINPARAM, * PFILECONVERSIONWINPARAM;
typedef const FILECONVERSIONWINPARAM* PCFILECONVERSIONWINPARAM;

/* ע���ļ�ת������
 */
void RegisterHANFileConversion(HINSTANCE hInst);

/* ��������
 * hFileConversion          ���ھ��
 * pIniPath                 ini �ļ�·��
 */
void WriteFileConversionCfg(HWND hFileConversion, HANPCSTR pIniPath);

/* ��ʼ������
 * pFcConfig                ������õĽṹ��ָ��
 * pIniPath                 ini �ļ�·��
 */
void ReadConvertFileIniFile(PFILECONVERSIONCFG pFcConfig, HANPCSTR pIniPath);

/* ��ʼ���ļ�ת������
 * pFcConfig                ������õĽṹ��ָ��
 * pParam                   ��Ž���������ת�������Ľṹ��ָ��
 */
void InitFileConversionParam(PCFILECONVERSIONCFG pFcConfig, PFILECONVERSIONWINPARAM pWinParam);

#ifdef __cplusplus
}
#endif

#endif
