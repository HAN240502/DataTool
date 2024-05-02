#ifndef HAN_HEX_H
#define HAN_HEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "..\..\HAN_Lib\HAN_windows.h"
#include "..\..\GlobalVariables.h"

#define HAN_HEX_VIEW_CLASS      TEXT("HAN_HexView")

#define DEFAULT_START_ADDR      0x80100000
#define DEFAULT_END_ADDR        0x8020FFFF

#define DEFAULT_START_ADDR_TEXT TEXT("80100000")
#define DEFAULT_END_ADDR_TEXT   TEXT("8020FFFF")

#define SYS_TEXT_W       80
#define SYS_TEXT_H       21

typedef struct tagADDRMAP {
    uint32_t        cStartAddr;
    uint32_t        cEndAddr;
} ADDRMAP, * PADDRMAP;

typedef struct tagHEXRECORD {
    uint8_t     nDataLen;
    uint16_t    cAddr;
    uint8_t     cDataType;
    uint8_t     pData[256];
    uint8_t     cCheckValue;
    BOOL        bCheckOK;
} HEXRECORD, * PHEXRECORD;
typedef const HEXRECORD* PCHEXRECORD;

typedef struct tagHEXFILE {
    char*       pText;
    PHEXRECORD  pHexRecord;
    uint8_t*    pBinData;
    uint32_t    nTextLen;
    uint32_t    nHexRecordCnt;
    ADDRMAP     amBinAddrMap;
} HEXFILE, * PHEXFILE;
typedef const HEXFILE* PCHEXFILE;

void RegisterHANHexView(HINSTANCE hInst);
void BinDataToHexFile(HANDLE hFile, uint32_t cStartAddr, uint8_t* pBinData, uint32_t nLen);

void LinkHexFile(HANPCSTR pFileDest, HANCHAR pFileSrc[LINK_FILE_CNT_MAX][PATH_STR_SIZE], uint32_t nLen);

HAN_errno_t HANOpenHexFile(PHEXFILE pHexFile, HANPCSTR pFileName, HANDLE hHeap);
HAN_errno_t HexViewHexToBin(PHEXFILE pHexFile, PADDRMAP pAddrMap, HANDLE hHeap);

#ifdef __cplusplus
}
#endif

#endif
