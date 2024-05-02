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

/******************** ��ɫ ********************/
typedef struct tagCOLORHSB {
    FLOAT   kHue;
    FLOAT   kSaturation;
    FLOAT   kBrightness;
} COLORHSB;
void RGBToHSB(const COLORREF* crRGB, COLORHSB* crHSB);
void HSBToRGB(const COLORHSB* crHSB, COLORREF* crRGB);

/******************** �����ڴ� ********************/
/* ���ӵ������ڴ�ĺ��������ڴ治���ڣ��ú����ᴴ���ڴ沢���ӣ����Ѵ��ڣ���ֱ�����ӣ�����ֵ���Ǹ��ڴ���׵�ַ
 * ��ͬ����ʹ��ͬһ�� mem_name �᷵��ͬһƬ�ڴ�ӳ��ĵ�ַ
 * nSize        �ڴ�Ĵ�С
 * phFile       ��Ϊ NULL������Ϊ NULL ��file_handleָ��ľ���ᱻָ��ӳ���ļ�����Ϊ�ر��ļ�������
 * return       �����ڴ���׵�ַ
 */
void* ConnectToSharedMem(HANPCSTR pMemName, DWORD nSize, HANDLE* phFile);
/* �Ͽ������ڴ�����Ӻ͹ر��ڴ�ӳ���ļ��ĺ���
 * һ�������Ĳ�������Ӧ�������н��̶���һ���Ͽ����ӣ����һ�����̹ر��ļ�
 * ���Ǹù����ڴ���������н��̵������������ڣ��ڹرս���ʱϵͳ���Զ�����
 * �ȼ��� UnmapViewOfFile ����
 */
void DisconnectFromSharedMem(void* pMemory);
/* �ر��ڴ�ӳ���ļ����ȼ��� CloseHandle ����
 */
void CloseSharedMem(HANDLE hFile);

/******************** ���� ********************/
typedef struct COM_init_struct {
    struct {   // �ļ�
        uint16_t    id;                             // �豸���������ܲ鵽�Ķ˿ںţ������־���
        DWORD       dwFlagsAndAttributes;           // ������ 0 ��ͬ����ʽ���� FILE_FLAG_OVERLAPPED ���ص���ʽ��
    };
    struct {    // ����
        DWORD       dwInQueue;                      // ���뻺�����Ĵ�С������ż��
        DWORD       dwOutQueue;                     // ����������Ĵ�С������ż��
    };
    struct {   // ��ʱ
        DWORD       ReadIntervalTimeout;            // �������ʱ
        DWORD       ReadTotalTimeoutMultiplier;     // ��ʱ��ϵ��
        DWORD       ReadTotalTimeoutConstant;       // ��ʱ�䳣��
        DWORD       WriteTotalTimeoutMultiplier;    // дʱ��ϵ��
        DWORD       WriteTotalTimeoutConstant;      // дʱ�䳣��
    };
    struct {  // ���ڲ���
        DWORD       BaudRate;                       // ������
        BYTE        ByteSize;                       // �ֽ�λ��
        BYTE        Parity;                         // ����λ��NOPARITY / ODDPARITY / EVENPARITY / MARKPARITY / SPACEPARITY
        BYTE        StopBits;                       // ֹͣλ��ONESTOPBIT / TWOSTOPBITS �� ONE5STOPBITS ���ܲ����ã�
    };
} COMCFG, * PCOMCFG;
typedef const COMCFG* PCCOMCFG;
/* �򿪴���
 */
HAN_errno_t OpenCOM(HANDLE* phCOM, PCCOMCFG pCOMInfo);
DWORD GetCOMDataRevCount(HANDLE hCOM);
/* ��ȡ�������ݣ���ͬ�����ص����ú�����ʹ��ͬ��������ʹ�ã�Ҳ�����������
 * ���Ҫ��ȡ���ֽ��������ѽ����ֽ������ú����Ὣ�ѽ��յ�����ȡ������������
 * hCOM         ���ھ��
 * pBuf         �������ݵ�����
 * nBufSize     ָ�����յ��ֽ������ޣ�0 ��ʾ�����ڻ������ѽ��յ�����ȫ���������� 0 ʱ������ȡ���ڻ������ֽ������ֵ�н�С��
 *              ���ܸ�ֵ�����Ӧ��֤ buffer �㹻��
 * nRevSize     ����ʵ�ʽ��յ����ֽ���
 * lpOverlapped ����򿪵Ĵ�����ͬ��ģʽ����ֵ������ NULL ��������ص�ģʽ����ֵ������Ч
 * return       �ɹ����� RET_OK ��ʧ�ܷ��� READ_COM_ERR ��ʧ�ܿ�������Ϊ���Ͽ�
 */
HAN_errno_t ReadCOM(HANDLE hCOM, void* pBuf, DWORD nBufSize, DWORD* nRevSize, LPOVERLAPPED lpOverlapped);
/* ��עɾ��ɨ��˿ڣ�עɾ���еĶ˿����������豸�������еĶ˿�����ͬ��������һ����ͬ
 * SweepAction  ÿɨ�赽һ���˿�ִ�еĲ���
 *              pCOMId      ��ǰɨ�赽�Ķ˿ں�
 *              pCOMName    ��ǰɨ�赽�Ķ˿���
 *              nCount      ��ǰɨ��Ĵ������� 0 ��ʼ
 *              pParam      �û�����
 * pParam       ���� SweepAction ���û�����
 */
DWORD SweepCOMFromRegedit(void (*SweepAction)(const LPBYTE pCOMId, HANPCSTR pCOMName, DWORD nCount, void* pParam), void* pParam);

/******************** ���� ********************/
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

/* ����������
 */
int CreateMainWindow(HINSTANCE hInstance, int nCmdShow, HANPCWIN wWin);

/* ��ȡ RECT �ṹ��Ŀ�
 */
LONG GetRectW(const RECT* rcWin);
/* ��ȡ RECT �ṹ��ĸ�
 */
LONG GetRectH(const RECT* rcWin);
/* �жϵ��Ƿ��� RECT �ṹ����
 */
BOOL PosInRect(const POINT* ptPos, const RECT* rcRect);
/* ��ȡ���ƾ��Σ��ɹ����� TRUE ��ʧ�ܣ�pAnchor1 �� pAnchor2 ͬ�л�ͬ�У����� FALSE ��
 * pDist        ��Ž���ľ���
 * pSource      ��Ϊ��׼�ľ���
 * pAnchor1     ê��1���õ�ȷ��λ�ã���������ľ��α���һ�������Ǹõ�
 * pAnchor2     ê��2���õ�ȷ����С����������ľ��α���һ���߾����õ�
 */
BOOL SimilarRect(RECT* pDist, const RECT* pSource, const POINT* pAnchor1, const POINT* pAnchor2);
/* ��ȡ���ڷ��
 */
LONG GetWindowStyle(HWND hWnd);

// button
int ButtonGetCheck(HWND hButton); // ����ֵ: BST_CHECKED, BST_INDETERMINATE, BST_UNCHECKED
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
