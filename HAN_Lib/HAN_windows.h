#ifndef HAN_WINDOWS_H
#define HAN_WINDOWS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdint.h>
#include <windows.h>
#include <tchar.h>

#include "HAN_def.h"

#define PATH_STR_SIZE   260

typedef TCHAR           HANCHAR;
typedef TCHAR*          HANPSTR;
typedef const TCHAR*    HANPCSTR;
typedef uint64_t        HANLSIZE;
typedef HANLSIZE*       HANPLSIZE;
typedef const HANLSIZE* HANPCLSIZE;
typedef unsigned int    HANSIZE;
typedef HANSIZE*        HANPSIZE;
typedef const HANSIZE*  HANPCSIZE;

#define HAN_printf      _tprintf
#define HAN_sprintf     _stprintf
#define HAN_snprintf    _sntprintf
#define HAN_fprintf     _ftprintf
#define HAN_vsnprintf   _vsntprintf

#define HAN_strcmp      _tcscmp
#define HAN_strlen      _tcslen
#define HAN_strcpy      _tcscpy
#define HAN_strncpy     _tcsncpy
#define HAN_strcat      _tcscat
#define HAN_strstr      _tcsstr

#define HAN_strtod      _tcstod
#define HAN_strtof      _tcstof
#define HAN_strtol      _tcstol
#define HAN_strtoul     _tcstoul
#define HAN_atoi        _tstoi
#define HAN_atof        _tstof

#define HAN_isdigit     _istdigit
#define HAN_isalpha     _istalnum
#define HAN_isspace     _istspace

LPVOID HANWinHeapAlloc(HANDLE hHeap, void* pReAllocMem, SIZE_T dwBytes);

/******************** 颜色 ********************/
typedef struct tagCOLORHSB {
    FLOAT   kHue;
    FLOAT   kSaturation;
    FLOAT   kBrightness;
} COLORHSB;
void RGBToHSB(const COLORREF* crRGB, COLORHSB* crHSB);
void HSBToRGB(const COLORHSB* crHSB, COLORREF* crRGB);

/******************** 共享内存 ********************/
/* 连接到共享内存的函数，若内存不存在，该函数会创建内存并连接，若已存在，则直接连接，返回值便是该内存的首地址
 * 不同进程使用同一个 mem_name 会返回同一片内存映射的地址
 * nSize        内存的大小
 * phFile       可为 NULL，若不为 NULL ，file_handle指向的句柄会被指向映射文件，作为关闭文件的依据
 * return       共享内存的首地址
 */
void* ConnectToSharedMem(HANPCSTR pMemName, DWORD nSize, HANDLE* phFile);
/* 断开共享内存的连接和关闭内存映射文件的函数
 * 一个完整的操作流程应当是所有进程都有一个断开连接，最后一个进程关闭文件
 * 除非该共享内存存在于所有进程的整个生命周期，在关闭进程时系统会自动回收
 * 等价于 UnmapViewOfFile 函数
 */
void DisconnectFromSharedMem(void* pMemory);
/* 关闭内存映射文件，等价于 CloseHandle 函数
 */
void CloseSharedMem(HANDLE hFile);

/******************** 串口 ********************/
typedef struct COM_init_struct {
    struct {   // 文件
        uint16_t    id;                             // 设备管理器里能查到的端口号，填数字就行
        DWORD       dwFlagsAndAttributes;           // 必须填 0 （同步方式）或 FILE_FLAG_OVERLAPPED （重叠方式）
    };
    struct {    // 缓冲
        DWORD       dwInQueue;                      // 输入缓冲区的大小，必须偶数
        DWORD       dwOutQueue;                     // 输出缓冲区的大小，必须偶数
    };
    struct {   // 超时
        DWORD       ReadIntervalTimeout;            // 读间隔超时
        DWORD       ReadTotalTimeoutMultiplier;     // 读时间系数
        DWORD       ReadTotalTimeoutConstant;       // 读时间常量
        DWORD       WriteTotalTimeoutMultiplier;    // 写时间系数
        DWORD       WriteTotalTimeoutConstant;      // 写时间常量
    };
    struct {  // 串口参数
        DWORD       BaudRate;                       // 波特率
        BYTE        ByteSize;                       // 字节位数
        BYTE        Parity;                         // 检验位：NOPARITY / ODDPARITY / EVENPARITY / MARKPARITY / SPACEPARITY
        BYTE        StopBits;                       // 停止位：ONESTOPBIT / TWOSTOPBITS （ ONE5STOPBITS 可能不能用）
    };
} COMCFG, * PCOMCFG;
typedef const COMCFG* PCCOMCFG;
/* 打开串口
 */
HAN_errno_t OpenCOM(HANDLE* phCOM, PCCOMCFG pCOMInfo);
DWORD GetCOMDataRevCount(HANDLE hCOM);
/* 读取串口数据，可同步可重叠，该函数即使在同步操作中使用，也不会造成阻塞
 * 如果要读取的字节数大于已接收字节数，该函数会将已接收的数据取出后立即返回
 * hCOM         串口句柄
 * pBuf         接收数据的数组
 * nBufSize     指定接收的字节数上限，0 表示将串口缓存里已接收的数据全部读出，非 0 时，则会读取串口缓存中字节数或该值中较小的
 *              不管该值填几，都应保证 buffer 足够大
 * nRevSize     返回实际接收到的字节数
 * lpOverlapped 如果打开的串口是同步模式，该值必须填 NULL ，如果是重叠模式，该值必须有效
 * return       成功返回 RET_OK ，失败返回 READ_COM_ERR 。失败可能是因为连断开
 */
HAN_errno_t ReadCOM(HANDLE hCOM, void* pBuf, DWORD nBufSize, DWORD* nRevSize, LPOVERLAPPED lpOverlapped);
/* 从注删表扫描端口，注删表中的端口名可能与设备管理器中的端口名不同，但内容一般相同
 * SweepAction  每扫描到一个端口执行的操作
 *              pCOMId      当前扫描到的端口号
 *              pCOMName    当前扫描到的端口名
 *              nCount      当前扫描的次数，从 0 开始
 *              pParam      用户参数
 * pParam       传给 SweepAction 的用户参数
 */
DWORD SweepCOMFromRegedit(void (*SweepAction)(const LPBYTE pCOMId, HANPCSTR pCOMName, DWORD nCount, void* pParam), void* pParam);

/******************** 窗口 ********************/
#define DEFAULT_MAIN_WINDOW_STYLE   (WS_OVERLAPPEDWINDOW)
#define DEFAULT_MAIN_WINDOW_X       (CW_USEDEFAULT)
#define DEFAULT_MAIN_WINDOW_Y       (0)
#define DEFAULT_MAIN_WINDOW_WIDTH   (CW_USEDEFAULT)
#define DEFAULT_MAIN_WINDOW_HEIGHT  (0)

typedef LRESULT (*MsgAction)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
typedef struct tagHANWIN {
    HANPSTR cls;
    HANPSTR title;
    DWORD   style;
    int     x;
    int     y;
    int     w;
    int     h;
    HBRUSH  background;
    WNDPROC WndProc;
} HANWIN, * HANPWIN;
typedef const HANWIN* HANPCWIN;

/* 创建主窗口
 */
int CreateMainWindow(HINSTANCE hInstance, int nCmdShow, HANPCWIN wWin);

/* 获取 RECT 结构体的宽
 */
LONG GetRectW(const RECT* rcWin);
/* 获取 RECT 结构体的高
 */
LONG GetRectH(const RECT* rcWin);
/* 判断点是否在 RECT 结构体内
 */
BOOL PosInRect(const POINT* ptPos, const RECT* rcRect);
/* 获取相似矩形（成功返回 TRUE ，失败（pAnchor1 和 pAnchor2 同行或同列）返回 FALSE ）
 * pDist        存放结果的矩形
 * pSource      作为标准的矩形
 * pAnchor1     锚点1，该点确定位置，计算出来的矩形必有一个顶点是该点
 * pAnchor2     锚点2，该点确定大小，计算出来的矩形必有一条边经过该点
 */
BOOL SimilarRect(RECT* pDist, const RECT* pSource, const POINT* pAnchor1, const POINT* pAnchor2);
/* 获取窗口风格
 */
LONG GetWindowStyle(HWND hWnd);

// button
int ButtonGetCheck(HWND hButton); // 返回值: BST_CHECKED, BST_INDETERMINATE, BST_UNCHECKED
void ButtonSetChecked(HWND hButton);
void ButtonSetIndeterminate(HWND hButton);
void ButtonSetUnchecked(HWND hButton);
// edit
void EditSetLimitText(HWND hEdit, size_t Len);
void EditSetSel(HWND hEdit, int StartChar, int EndChar);
void EditAppendText(HWND hEdit, HANPCSTR pString, BOOL Undo);
// listbox
int ListBoxAddString(HWND hListBox, HANPCSTR pString);
int ListBoxAddStringArr(HWND hListBox, HANPCSTR* StringArr, size_t nMaxCount);
int ListBoxAddStringStructArr(HWND hListBox, const void* pStruct, size_t nStructSize, size_t nOffset, size_t nMaxCount);
int ListBoxGetCursel(HWND hListBox);
int ListBoxSetCursel(HWND hListBox, int Index);
// combobox
int ComboBoxAddString(HWND hComboBox, HANPCSTR pString);
int ComboBoxAddStringArr(HWND hComboBox, const HANPCSTR* StringArr, size_t nMaxCount);
int ComboBoxAddStringStructArr(HWND hComboBox, const void* pStruct, size_t nStructSize, size_t nOffset, size_t nMaxCount);
int ComboBoxDeleteString(HWND hComboBox, int Index);
void ComboBoxClearString(HWND hComboBox);
int ComboBoxGetCursel(HWND hComboBox);
int ComboBoxSetCursel(HWND hComboBox, int Index);
int ComboBoxFindString(HWND hComboBox, int StartIndex, HANPCSTR pString);
int ComboBoxFindStringEx(HWND hComboBox, int StartIndex, HANPCSTR pString);

#ifdef __cplusplus
}
#endif

#endif
