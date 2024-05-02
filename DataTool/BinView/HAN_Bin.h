#ifndef HAN_BIN_H
#define HAN_BIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "..\..\HAN_Lib\HAN_windows.h"
#include "..\..\HAN_Lib\HAN_wingdi.h"

typedef enum {
    CRCID_CRC4_ITU,
    CRCID_CRC5_EPC,
    CRCID_CRC5_ITU,
    CRCID_CRC5_USB,
    CRCID_CRC6_ITU,
    CRCID_CRC7_MMC,
    CRCID_CRC8_STD,
    CRCID_CRC8_ITU,
    CRCID_CRC8_ROHC,
    CRCID_CRC8_MAXIM,
    CRCID_CRC16_IBM,
    CRCID_CRC16_MAXIM,
    CRCID_CRC16_USB,
    CRCID_CRC16_MODEBUS,
    CRCID_CRC16_CCITT,
    CRCID_CRC16_CCITT_FALSE,
    CRCID_CRC16_UDS,
    CRCID_CRC16_X25,
    CRCID_CRC16_XMODEM,
    CRCID_CRC16_DNP,
    CRCID_CRC32_STD,
    CRCID_CRC32_MPEG2,
} ENUMCRCID;

#define HAN_BIN_VIEW_CLASS      TEXT("HAN_BinView")

#define DEFAULT_DATA_POS_VALUE  0x10FFC7
#define DEFAULT_CRC_START_VALUE 0x0
#define DEFAULT_CRC_END_VALUE   0x10FFFB
#define DEFAULT_CRC_POS_VALUE   0x10FFFE

#define DEFAULT_DATA_POS_TEXT   TEXT("10FFC7")
#define DEFAULT_CRC_START_TEXT  TEXT("0")
#define DEFAULT_CRC_END_TEXT    TEXT("10FFFB")
#define DEFAULT_CRC_POS_TEXT    TEXT("10FFFE")

#define DEFAULT_CRC_ID          CRCID_CRC16_UDS
#define CRC_NAME_MAX_CNT        100

typedef struct tagBINFILE {
    uint8_t*        pBinData;
    uint32_t        nBinLen;
} BINFILE, * PBINFILE;

void RegisterHANBinView(HINSTANCE hInst);

HAN_errno_t HANOpenBinFile(PBINFILE pBinFile, HANPCSTR pFileName, HANDLE hHeap);
void HANBinFileSetData(uint8_t* pDest, uint32_t nDestLen, HANPCSTR pSrcName, uint32_t nOffset, uint32_t nSrcLen);
uint32_t HANBinFileSetCrc(uint8_t* pData, uint32_t nLen, uint32_t cCrcId, uint8_t* pCrcResult);

BOOL HANBinViewGettCrcRoutine(uint32_t nCrcId, HANPSTR pCrcName, uint32_t nNameMaxCnt, uint32_t* pCrcSize);

#ifdef __cplusplus
}
#endif

#endif
