#ifndef HAN_FILE_CONVERSION_WINDOW_H
#define HAN_FILE_CONVERSION_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "..\DataToolTypedef.h"
#include "HAN_FileConversionTypedef.h"
#include "HAN_FileConversion.h"

#define HAN_FILE_CONVERSION_CLASS       TEXT("HAN_FileConversion")

#define HAN_FILE_CONVERSION_PATH_MAX    260

#define USER_SETTING_KEY_NAME           TEXT("UserConfig")

typedef enum {
    INI_FILE_CONVERSION_DLL_PATH,
    INI_FILE_CONVERSION_CONVERT_PATH,
    INI_FILE_CONVERSION_CONVERT_FOLDER,
    INI_FILE_CONVERSION_FILTER,
    INI_FILE_CONVERSION_CFG_CNT,
} INIFILECONVERSIONCFGID;

typedef struct tagFILECONVERSIONCFG {
    HANDATAINI          pSysConfig[INI_FILE_CONVERSION_CFG_CNT];
    USERSETTING         usUserConfig;
} FILECONVERSIONCFG, * PFILECONVERSIONCFG;
typedef const FILECONVERSIONCFG* PCFILECONVERSIONCFG;

typedef struct tagFILECONVERSIONWINPARAM {
    FILECONVERSIONPARAM     fcParam;
    HINSTANCE               hDll;
    HANCHAR                 pDllPath[PATH_STR_SIZE];
} FILECONVERSIONWINPARAM, * PFILECONVERSIONWINPARAM;
typedef const FILECONVERSIONWINPARAM* PCFILECONVERSIONWINPARAM;

/* ע���ļ�ת������
 */
void RegisterHANFileConversion(HINSTANCE hInst);

/* ��ʼ������
 * pIniPath                 ini �ļ�·��
 * pParam                   ������õĽṹ��ָ��
 */
void ReadFileConversionIniFile(HANPCSTR pIniPath, void* pParam);

/* ��������
 * pIniPath                 ini �ļ�·��
 * hFileConversion          ���ھ��
 */
void WriteFileConversionIniFile(HANPCSTR pIniPath, HWND hFileConversion);

/* ��ʼ���ļ�ת������
 * pFcConfig                ������õĽṹ��ָ��
 * pParam                   ��Ž���������ת�������Ľṹ��ָ��
 */
void InitFileConversionParam(PCFILECONVERSIONCFG pFcConfig, PFILECONVERSIONWINPARAM pWinParam);

#ifdef __cplusplus
}
#endif

#endif
