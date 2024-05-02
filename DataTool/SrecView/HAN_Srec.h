#ifndef HAN_SREC_H
#define HAN_SREC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "..\..\HAN_Lib\HAN_windows.h"
#include "..\..\GlobalVariables.h"
#include "..\HexView\HAN_Hex.h"

#define HAN_SREC_VIEW_CLASS     TEXT("HAN_SrecView")

#define HAN_SREC_DATA_LEN_MAX   (0x10U)

typedef enum {
    SREC_ADDR_SIZE_2 = 2,
    SREC_ADDR_SIZE_3 = 3,
    SREC_ADDR_SIZE_4 = 4,
} SRECADDRSIZE;

typedef struct tagSRECRECORD {
    uint8_t     nDataLen;
    uint32_t    cAddr;
    uint8_t     cDataType;
    uint8_t     pData[256];
    uint8_t     cCheckValue;
    BOOL        bCheckOK;
} SRECRECORD, * PSRECRECORD;
typedef const SRECRECORD* PCSRECRECORD;

typedef struct tagSRECFILE {
    char*       pText;
    PSRECRECORD pSrecRecord;
    uint8_t*    pBinData;
    uint32_t    nTextLen;
    uint32_t    nSrecRecordCnt;
    ADDRMAP     amBinAddrMap;
} SRECFILE, * PSRECFILE;
typedef const SRECFILE* PCSRECFILE;

void RegisterHANSrecView(HINSTANCE hInst);
void BinDataToSrecFile(HANDLE hFile, SRECADDRSIZE sasAddrSize, uint32_t cStartAddr, uint8_t* pBinData, uint32_t nLen);

void LinkSrecFile(HANPCSTR pFileDest, HANCHAR pFileSrc[LINK_FILE_CNT_MAX][PATH_STR_SIZE], uint32_t nLen);

HAN_errno_t HANOpenSrecFile(PSRECFILE pSrecFile, HANPCSTR pFileName, HANDLE hHeap);
HAN_errno_t SrecViewSrecToBin(PSRECFILE pSrecFile, PADDRMAP pAddrMap, HANDLE hHeap);

#ifdef __cplusplus
}
#endif

#endif
