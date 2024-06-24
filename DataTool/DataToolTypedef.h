#ifndef HAN_DATA_TOOL_TYPEDEF_H
#define HAN_DATA_TOOL_TYPEDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "..\HAN_Lib\HAN_windows.h"

#define INI_MAIN_VALUE_STR_SIZE         260

#define DATA_TOOL_TEXT_INPUT_EDIT_H     26
#define DATA_TOOL_PATH_INPUT_EDIT_W     500

#define LINK_FILE_CNT_MAX               64

#define ADDR_STR_SIZE                   32
#define ADDR_PRINT_FORMAT_SINGLE        TEXT("%X")
#define ADDR_PRINT_FORMAT_FULL          TEXT("%08X")

typedef struct tagADDRMAP {
    uint32_t        cStartAddr;
    uint32_t        cEndAddr;
} ADDRMAP, * PADDRMAP;

typedef struct tagHANDATAINI {
    HANCHAR             pKey[INI_MAIN_VALUE_STR_SIZE];
    HANCHAR             pDefValue[INI_MAIN_VALUE_STR_SIZE];
    HANCHAR             pValue[INI_MAIN_VALUE_STR_SIZE];
} HANDATAINI, * PHANDATAINI;

typedef struct tagDATATOOLINFO {
    HANCHAR     pTitle[256];
    HANPCSTR    pClass;
    HWND        hItem;
    HMENU       nWinId;
    void        (*RegisterHANTabWindow)(HINSTANCE hInst);
    void        (*ReadIniFile)(HANPCSTR pIniName, void* pIni);
    void        (*WriteIniFile)(HANPCSTR pIniName, HWND hTool);
    void*       pIni;
} DATATOOLINFO;

#ifdef __cplusplus
}
#endif

#endif
