#ifndef HAN_GLOBAL_VARIABLES_H
#define HAN_GLOBAL_VARIABLES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "HAN_Lib\HAN_windows.h"
#include "DataTool\FileConversion\HAN_FileConversionTypedef.h"

#define LINK_FILE_CNT_MAX       64

#define CFG_INI_FILE_NAME       TEXT("HANDataCfg.ini")

#define INI_FILE_CONVERSION_VALUE_SIZE  PATH_STR_SIZE
#define INI_FILE_CONVERSION_APP_NAME    TEXT("fcConfig")

typedef enum {
    INI_FILE_CONVERSION_DLL_PATH,
    INI_FILE_CONVERSION_CONVERT_PATH,
    INI_FILE_CONVERSION_CONVERT_FOLDER,
    INI_FILE_CONVERSION_FILTER,
    INI_FILE_CONVERSION_CFG_CNT,
} INIFILECONVERSIONCFGID;
typedef enum {
    INI_HEX_VIEW_START_ADDR,
    INI_HEX_VIEW_END_ADDR,
    INI_HEX_VIEW_CFG_CNT,
} INIHEXVIEWCFGID;

typedef struct tagHANDATAINI {
    HANPCSTR            pKey;
    HANPCSTR            pDefValue;
    HANCHAR             pValue[INI_FILE_CONVERSION_VALUE_SIZE];
} HANDATAINI, * PHANDATAINI;

typedef struct tagFILECONVERSIONCFG {
    HANDATAINI          pSysConfig[INI_FILE_CONVERSION_CFG_CNT];
    USERSETTING         usUserConfig;
} FILECONVERSIONCFG, * PFILECONVERSIONCFG;
typedef const FILECONVERSIONCFG* PCFILECONVERSIONCFG;
typedef struct tagHEXVIEWCFG {
    HANDATAINI          pSysConfig[INI_HEX_VIEW_CFG_CNT];
} HEXVIEWCFG, * PHEXVIEWCFG;

typedef struct tagHANDATACFG {
    FILECONVERSIONCFG   cfgFileConversion;
    HEXVIEWCFG          cfgHexView;
} HANDATACFG, * PHANDATACFG;

extern LOGFONT          g_lfSysFont;
extern LOGFONT          g_lfHexFont;

extern HANDATACFG       g_cfgHANData;

extern HANCHAR          g_pIniFileName[PATH_STR_SIZE];

void GetIniFileName(HANPSTR pIniFileName, uint32_t nStrSize);

#ifdef __cplusplus
}
#endif

#endif
