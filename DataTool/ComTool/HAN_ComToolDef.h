#ifndef HAN_COM_TOOL_DEF_H
#define HAN_COM_TOOL_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "..\..\HAN_Lib\HAN_windows.h"
#include "..\..\GlobalVariables.h"
#include "..\DataToolTypedef.h"

#define INI_COM_TOOL_VALUE_SIZE             INI_MAIN_VALUE_STR_SIZE
#define INI_COM_TOOL_APP_NAME               TEXT("CtConfig")

#define COMTOOL_DEFAULT_BAUDRATE_ID         COMTOOL_BAUDRATE_115200
#define COMTOOL_DEFAULT_BYTE_SIZE           COMTOOL_BYTESIZE_8
#define COMTOOL_DEFAULT_STOP_BITS           COMTOOL_STOPBITS_1
#define COMTOOL_DEFAULT_PARITY              COMTOOL_PARITY_NONE

#define COMTOOL_COM_FRAME_BUF_SIZE          8192
#define COMTOOL_SUM_DATA_TEXT_SIZE          100
#define COMTOOL_SUM_RX_TITLE                TEXT("Rx: ")
#define COMTOOL_SUM_TX_TITLE                TEXT("Tx: ")

#define COMTOOL_TEXT_WINDOW_LIMIT           80000
#define COMTOOL_TEXT_WINDOW_MIN_W           800
#define COMTOOL_TEXT_WINDOW_MIN_H           400

#define COMTOOL_CONSLE_WINDOW_W             750
#define COMTOOL_CONSLE_WINDOW_H             190

#define COMTOOL_PRINT_COM_PERIOD            50
#define COMTOOL_READ_COM_PERIOD             10
#define COMTOOL_TIME_INFO_BUF_SIZE          (COMTOOL_PRINT_COM_PERIOD / COMTOOL_READ_COM_PERIOD * 3 * 5)
#define COMTOOL_DEF_USER_TIMEOUT            30

#define COMTOOL_WINDOW_DX                   10
#define COMTOOL_WINDOW_DY                   10

#define COMTOOL_DLU_WINDOW_DX               7
#define COMTOOL_DLU_WINDOW_DY               7
#define COMTOOL_DLU_TEXT_H                  13

#define COMTOOL_USER_BAUDRATE_DLU_W         100

#define COMTOOL_COMBOBOX_STYLE              (WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST)

#define COMTOOL_SIZE_PRINT_FORMAT           TEXT("%u")

#define SAVE_FILE_PATH_MESSAGEBOX_TITLE     TEXT("数据保存至：")
#define SAVE_FILE_PATH_MESSAGEBOX_STR_SIZE  PATH_STR_SIZE + 30

typedef enum {
    INI_COM_TOOL_COM_ID,
    INI_COM_TOOL_BAUDRATE,
    INI_COM_TOOL_USER_BAUDRATE,
    INI_COM_TOOL_BYTE_SIZE,
    INI_COM_TOOL_STOP_BITS,
    INI_COM_TOOL_PARITY,
    INI_COM_TOOL_HEX,
    INI_COM_TOOL_PRINT_START_TIME,
    INI_COM_TOOL_PRINT_END_TIME,
    INI_COM_TOOL_CFG_CNT,
} INICOMTOOLCFGID;

typedef enum {
    COMTOOL_BAUDRATE_USER,
    COMTOOL_BAUDRATE_110,
    COMTOOL_BAUDRATE_300,
    COMTOOL_BAUDRATE_600,
    COMTOOL_BAUDRATE_1200,
    COMTOOL_BAUDRATE_2400,
    COMTOOL_BAUDRATE_4800,
    COMTOOL_BAUDRATE_9600,
    COMTOOL_BAUDRATE_14400,
    COMTOOL_BAUDRATE_19200,
    COMTOOL_BAUDRATE_38400,
    COMTOOL_BAUDRATE_56000,
    COMTOOL_BAUDRATE_57600,
    COMTOOL_BAUDRATE_115200,
    COMTOOL_BAUDRATE_128000,
    COMTOOL_BAUDRATE_230400,
    COMTOOL_BAUDRATE_256000,
    COMTOOL_BAUDRATE_460800,
    COMTOOL_BAUDRATE_500000,
    COMTOOL_BAUDRATE_512000,
    COMTOOL_BAUDRATE_600000,
    COMTOOL_BAUDRATE_750000,
    COMTOOL_BAUDRATE_921600,
    COMTOOL_BAUDRATE_1000000,
    COMTOOL_BAUDRATE_1500000,
    COMTOOL_BAUDRATE_2000000,
    COMTOOL_BAUDRATE_3000000,
    COMTOOL_BAUDRATE_CNT,
} COMTOOLBAUDRATEID;
typedef enum {
    COMTOOL_BYTESIZE_5,
    COMTOOL_BYTESIZE_6,
    COMTOOL_BYTESIZE_7,
    COMTOOL_BYTESIZE_8,
    COMTOOL_BYTESIZE_CNT,
} COMTOOLBYTESIZEID;
typedef enum {
    COMTOOL_STOPBITS_1,
    COMTOOL_STOPBITS_1_5,
    COMTOOL_STOPBITS_2,
    COMTOOL_STOPBITS_CNT,
} COMTOOLSTOPBITSID;
typedef enum {
    COMTOOL_PARITY_NONE,
    COMTOOL_PARITY_ODD,
    COMTOOL_PARITY_EVEN,
    COMTOOL_PARITY_MARK,
    COMTOOL_PARITY_SPACE,
    COMTOOL_PARITY_CNT,
} COMTOOLPARITYID;

typedef enum {
    WID_COMTOOL_TEXT = 1,
    WID_COMTOOL_CONSOLE,
    WID_COMTOOL_OPEN_COM,
    WID_COMTOOL_CLEAR,
    WID_COMTOOL_SUM_RX,
    WID_COMTOOL_SUM_TX,
    WID_COMTOOL_COM_ID,
    WID_COMTOOL_BAUDRATE,
    WID_COMTOOL_USER_BAUDRATE_INPUT,
    WID_COMTOOL_USER_BAUDRATE_OK,
    WID_COMTOOL_BYTESIZE,
    WID_COMTOOL_STOPBITS,
    WID_COMTOOL_PARITY,
    WID_COMTOOL_HEX,
    WID_COMTOOL_PRINT_START_TIME,
    WID_COMTOOL_PRINT_END_TIME,
    WID_COMTOOL_SAVE_FILE_PATH,
    WID_COMTOOL_SAVE_FILE_SAVE,
} COMTOOLWID;

typedef enum {
    COMTOOL_DATA_TYPE_DATA,
    COMTOOL_DATA_TYPE_START,
    COMTOOL_DATA_TYPE_END,
} COMTOOLDATATYPE;

typedef enum {
    COMTOOL_UPDATE_COM_CFG_PORT     = 0x00000001,
    COMTOOL_UPDATE_COM_CFG_BAUDRATE = 0x00000002,
    COMTOOL_UPDATE_COM_CFG_BYTESIZE = 0x00000004,
    COMTOOL_UPDATE_COM_CFG_STOPBITS = 0x00000008,
    COMTOOL_UPDATE_COM_CFG_PARITY   = 0x00000010,
    COMTOOL_UPDATE_COM_CFG_ALL      = COMTOOL_UPDATE_COM_CFG_PORT
                                    | COMTOOL_UPDATE_COM_CFG_BAUDRATE
                                    | COMTOOL_UPDATE_COM_CFG_BYTESIZE
                                    | COMTOOL_UPDATE_COM_CFG_STOPBITS
                                    | COMTOOL_UPDATE_COM_CFG_PARITY,
} COMTOOLUPDATECOMCFG;

typedef enum {
    COMTOOL_READ_STATUS_READING,
    COMTOOL_READ_STATUS_WAITING,
} COMTOOLREADSTATUS;

typedef uint32_t            COMTOOLSIZE;

typedef struct tagCOMTOOLCFG {
    HANDATAINI          pSysConfig[INI_COM_TOOL_CFG_CNT];
} COMTOOLCFG, * PCOMTOOLCFG;

typedef struct tagCOMSBPAPARAM {
    HANPCSTR                pText;
    BYTE                    value;
} COMSBPAPARAM, * PCOMSBPAPARAM;

typedef struct tagCOMTOOLEXTRA {
    HANDLE                  hHeap;
    HINSTANCE               hInst;
    HANDLE                  hComFile;
    HWND                    hComOwner;
    HWND                    hSelf;
    HWND                    hConsle;
    HWND                    hOpenCom;
    struct {
        HWND                hComId;
        HWND                hBaudrate;
        HWND                hByteSize;
        HWND                hStopBits;
        HWND                hParity;
        COMCFG              cfgCOM;
    } cfgCOM;               /* 串口配置 */
    struct {
        HWND                hHex;
        BOOL                bHex;
    } printHex;             /* HEX 打印串口数据 */
    struct {
        struct {
            HWND            hText;
            HWND            hClear;
            HANDLE          hPrintComThread;
            HANDLE          hPrintComSemaphore;
        } printData;
        struct {
            HWND            hStart;
            BOOL            bStart;
        } printStartTime;
        struct {
            HWND            hEnd;
            BOOL            bEnd;
        } printEndTime;
    } printData;            /* 打印帧时间 */
    struct {
        HWND                hPath;
        HWND                hSave;
        HANCHAR             pPath[PATH_STR_SIZE];
        BOOL                bSave;
        HANDLE              hFile;
        OVERLAPPED          olFile;
    } saveToFile;           /* 保存数据到文件 */
    struct {
        HWND                hInput;
        HWND                hOk;
    } userBaudrate;         /* 用户数据 */
    struct {
        HFONT               hHex;
        HFONT               hSys;
    } hFont;                /* 字体 */
    COMTOOLSIZE             nTextWindowLen;
    struct {
        COMTOOLREADSTATUS   cStatus;
        uint8_t             nIndex;
        struct {
            struct {
                COMTOOLSIZE     nLen;
                struct {
                    COMTOOLDATATYPE cType;
                    SYSTEMTIME      tTime;
                    COMTOOLSIZE     nBlockLen;
                } pInfo[COMTOOL_TIME_INFO_BUF_SIZE];
            } pInfo;
            struct {
                COMTOOLSIZE nLen;
                uint8_t     pData[COMTOOL_COM_FRAME_BUF_SIZE];
            } pData;
            HANCHAR         pText[COMTOOL_COM_FRAME_BUF_SIZE * 3];
        } pBuf[2];
        struct {
            DWORD           msPrintComTime;
            DWORD           msTimeOut;
            DWORD           msTime;
            SYSTEMTIME      timeLastByte;
        } tTime;
    } revData;              /* 接收数据 */
    struct {
        struct {
            HWND            hWnd;
            COMTOOLSIZE     nSum;
        } sumTx;
        struct {
            HWND            hWnd;
            COMTOOLSIZE     nSum;
        } sumRx;
    } sumData;              /* 打印数据统计 */
    int                     nBaudrateListCurselId;
    COMTOOLCFG              pCtConfig;
} COMTOOLEXTRA, * PCOMTOOLEXTRA;
typedef const COMTOOLEXTRA* PCCOMTOOLEXTRA;

typedef struct tagCOMTOOLREADWRITECFG {
    HANPCSTR                pKey;
    HANPCSTR                pDefValue;
    void                    (*CfgWindowToText)(PCOMTOOLEXTRA ctInfo, HANPSTR pText);
    void                    (*CfgTextToWindow)(PCOMTOOLEXTRA ctInfo, HANPCSTR pText);
} COMTOOLREADWRITECFG;

#ifdef __cplusplus
}
#endif

#endif
