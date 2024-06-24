#ifndef HAN_HEX_H
#define HAN_HEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "..\..\HAN_Lib\HAN_windows.h"
#include "..\DataToolTypedef.h"

#define HAN_HEX_VIEW_CLASS              TEXT("HAN_HexView")

#define DEFAULT_START_ADDR              0x00000000
#define DEFAULT_END_ADDR                0x0000FFFF
#define DEFAULT_START_ADDR_TEXT         TEXT("00000000")
#define DEFAULT_END_ADDR_TEXT           TEXT("0000FFFF")

#define SYS_TEXT_W       80
#define SYS_TEXT_H       21

typedef enum {
    INI_HEX_VIEW_START_ADDR,
    INI_HEX_VIEW_END_ADDR,
    INI_HEX_VIEW_CFG_CNT,
} INIHEXVIEWCFGID;

typedef struct tagHEXVIEWCFG {
    HANDATAINI          pSysConfig[INI_HEX_VIEW_CFG_CNT];
} HEXVIEWCFG, * PHEXVIEWCFG;

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

/* ��ʼ������
 * pIniPath                 ini �ļ�·��
 * pParam                   ������õĽṹ��ָ��
 */
void ReadHexViewIniFile(HANPCSTR pIniPath, void* pParam);

/* ��������
 * pIniPath                 ini �ļ�·��
 * hHexView                 ���ھ��
 */
void WriteHexViewIniFile(HANPCSTR pIniPath, HWND hHexView);

void BinDataToHexFile(HANDLE hFile, uint32_t cStartAddr, uint8_t* pBinData, uint32_t nLen);

void LinkHexFile(HANPCSTR pFileDest, HANCHAR pFileSrc[LINK_FILE_CNT_MAX][PATH_STR_SIZE], uint32_t nLen);

HAN_errno_t HANOpenHexFile(PHEXFILE pHexFile, HANPCSTR pFileName, HANDLE hHeap);
HAN_errno_t HexViewHexToBin(PHEXFILE pHexFile, PADDRMAP pAddrMap, HANDLE hHeap);

#ifdef __cplusplus
}
#endif

#endif
