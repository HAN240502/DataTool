#ifndef HAN_WIN_GDI_H
#define HAN_WIN_GDI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "HAN_windows.h"

#if 1 /******************** ��ǰ���ı�������򡢺����ı�������ؼ� ********************/
#define HINPUT_CLASS    TEXT("HANInput")

#if 1 // Ĭ�ϲ���
#endif

typedef enum {
    HIPM_NOTUSED = WM_USER,
    HIPM_SETLEFTTEXTFONT,
    HIPM_SETINPUTFONT,
    HIPM_SETRIGHTTEXTFONT,
    HIPM_MOVELEFTTEXT,
    HIPM_MOVEINPUT,
    HIPM_MOVERIGHTTEXT,
    HIPM_SETLEFTTEXTTEXT,
    HIPM_SETINPUTTEXT,
    HIPM_SETRIGHTTEXTTEXT,
    HIPM_GETLEFTTEXTTEXT,
    HIPM_GETINPUTTEXT,
    HIPM_GETRIGHTTEXTTEXT,
} HANINPUTMESSAGE;

/* ע������ؼ��࣬Ӧ�ó�����ʹ�ô˿ؼ�ǰ�����ȵ��øú���ע�ᴰ����
 */
void RegisterHANInput(HINSTANCE hInstance);
/* ����ǰ���ı�����
 */
void HANInputSetLeftTextFont(HWND hInput, HFONT hFOnt, BOOL bRepaint);
/* �������������
 */
void HANInputSetInputFont(HWND hInput, HFONT hFOnt, BOOL bRepaint);
/* ���ú����ı�����
 */
void HANInputSetRightTextFont(HWND hInput, HFONT hFOnt, BOOL bRepaint);
/* �ƶ�ǰ���ı�
 */
void HANInputMoveLeftText(HWND hInput, int x, int y, int w, int h, BOOL bRepaint);
/* �ƶ������
 */
void HANInputMoveInput(HWND hInput, int x, int y, int w, int h, BOOL bRepaint);
/* �ƶ������ı�
 */
void HANInputMoveRightText(HWND hInput, int x, int y, int w, int h, BOOL bRepaint);
/* ����ǰ���ı�������
 */
void HANInputSetLeftTextText(HWND hInput, HANPCSTR pText);
/* ��������������
 */
void HANInputSetInputText(HWND hInput, HANPCSTR pText);
/* ���ú����ı�������
 */
void HANInputSetRightTextText(HWND hInput, HANPCSTR pText);
/* ��ȡǰ���ı�������
 */
void HANInputGetLeftTextText(HWND hInput, HANPSTR pText, int nMaxCnt);
/* ��ȡ����������
 */
void HANInputGetInputText(HWND hInput, HANPSTR pText, int nMaxCnt);
/* ��ȡ�����ı�������
 */
void HANInputGetRightTextText(HWND hInput, HANPSTR pText, int nMaxCnt);
#endif

#if 1 /******************** ״̬��ť ********************/
#define HSTBT_CLASS     TEXT("HANStateButton")

#if 1 // Ĭ�ϲ���
#define HSTBT_TXET_SIZE     256 // �ı��������ޣ��� '\0'
#endif

/* HSBTAction ָ���˰��°�ťʱ��ִ�еĶ�������ָ�����Ϊ NULL
 * hStateButton ���ھ��
 * hChildButton �Ӵ��ھ���������Ƴ�ͼ�εİ�ť�����Ʒ�ʽͬ windows.h ��Ĭ�� button ��
 * pParam       �û��������ò���Ϊ���� CreateWindow �����һ������
 * return       ��Ҫ�л���״̬�������û�����ͨ������ֵ���ư�ť��״̬��������һ��״̬������ 1��/ ���ֵ�ǰ״̬������ 0��/ ���� n ��״̬������ n + 1��
 *              ������ָ��Ϊ NULL����Ĭ�Ϸ��� 1
 */
typedef HANSIZE (*HSTBTAction)(HWND hStateButton, HWND hChildButton, void* pParam);
typedef struct tagHSTBTSTATE {
    HANCHAR     pText[HSTBT_TXET_SIZE];
    HSTBTAction Action;
} HSTBTSTATE, * HPSTBTSTATE;
typedef const HSTBTSTATE* HPCSTBTSTATE;

/* ������Ϣ */
typedef enum {
    HSTBTM_NOTUSED = WM_USER,
    HSTBTM_ADDSTATE,
    HSTBTM_CLICK,
} HANSTATEBUTTONMESSAGE;

/* ע���״̬��ť�࣬Ӧ�ó�����ʹ�ô˿ؼ�ǰ�����ȵ��øú���ע�ᴰ����
 */
void RegisterHANStateButton(HINSTANCE hInstance);
/* ���״̬
 */
HAN_errno_t HANStateButtonAddState(HWND hStateButton, HPCSTBTSTATE pState);
/* ���
 */
void HANStateButtonClick(HWND hStateButton);
#endif

#if 1 /******************** COM ********************/
#define HCOM_CLASS      TEXT("HANCOM")

#if 1 // Ĭ�ϲ�������Щ���������⿪�ţ����ڴ�������ʱ��ο�
#define HCOM_DEF_BUF_SIZE       10000
#define HCOM_NUM_TEXT_SIZE      100
#endif

#if 1 // ���ڷ��
#define HCOMMS_MSG_BOX          0x00000001  // ������Ϣ�����򿪴��ڲ���ʧ�ܻ����÷Ƿ�����ʱ�����Ϣ��ʾ��
#endif

#if 1 // ������Ϣ
typedef enum {
    HCOMM_NOTUSED = WM_USER,    // ��ʹ�õ���Ϣֵ������Ϣ�����·����
    HCOMM_SHOWOPENCOM,          // ��ʾ�򿪴��ڰ�ť��WPARAM��BOOL �Ƿ���ʾ��LPARAM��δʹ�ã�
    HCOMM_SHOWPORTID,           // ��ʾ�˿ںţ�WPARAM��BOOL �Ƿ���ʾ��LPARAM��δʹ�ã�
    HCOMM_SHOWBAUDRATE,         // ��ʾ�����ʣ�WPARAM��BOOL �Ƿ���ʾ��LPARAM��δʹ�ã�
    HCOMM_SHOWBYTESIZE,         // ��ʾ�ֽڴ�С��WPARAM��BOOL �Ƿ���ʾ��LPARAM��δʹ�ã�
    HCOMM_SHOWPARITY,           // ��ʾУ��λ��WPARAM��BOOL �Ƿ���ʾ��LPARAM��δʹ�ã�
    HCOMM_SHOWSTOPBITS,         // ��ʾֹͣλ��WPARAM��BOOL �Ƿ���ʾ��LPARAM��δʹ�ã�
    HCOMM_MOVEOPENCOM,          // �ƶ��򿪴��ڰ�ť��WPARAM��BOOL bRepaint��LPARAM��RECT* �ƶ�������꣩
    HCOMM_MOVEPORTID,           // �ƶ��򿪴��ڰ�ť��WPARAM��BOOL bRepaint��LPARAM��RECT* �ƶ�������꣩
    HCOMM_MOVEBAUDRATE,         // �ƶ��򿪴��ڰ�ť��WPARAM��BOOL bRepaint��LPARAM��RECT* �ƶ�������꣩
    HCOMM_MOVEBYTESIZE,         // �ƶ��򿪴��ڰ�ť��WPARAM��BOOL bRepaint��LPARAM��RECT* �ƶ�������꣩
    HCOMM_MOVEPARITY,           // �ƶ��򿪴��ڰ�ť��WPARAM��BOOL bRepaint��LPARAM��RECT* �ƶ�������꣩
    HCOMM_MOVESTOPBITS,         // �ƶ��򿪴��ڰ�ť��WPARAM��BOOL bRepaint��LPARAM��RECT* �ƶ�������꣩
    HCOMM_OPENCOM,              // �򿪴��ڣ�WPARAM��δʹ�ã�LPARAM��δʹ�ã�
    HCOMM_SETPORTID,            // ���ö˿ںţ�WPARAM��uint16_t 1 ~ 256��LPARAM��δʹ�ã�
    HCOMM_SETBAUDRATE,          // ���ò����ʣ�WPARAM��DWORD �����ʣ�LPARAM��δʹ�ã�
    HCOMM_SETBYTESIZE,          // �����ֽڴ�С��WPARAM��BYTE �ֽڴ�С��LPARAM��δʹ�ã�
    HCOMM_SETPARITY,            // ����У��λ��WPARAM��BYTE У��λ��NOPARITY��ODDPARITY��EVENPARITY��MARKPARITY��SPACEPARITY����LPARAM��δʹ�ã�
    HCOMM_SETSTOPBITS,          // ����ֹͣλ��WPARAM��BYTE ֹͣλ��ONESTOPBIT��ONE5STOPBITS�����ܲ����ã���TWOSTOPBITS����LPARAM��δʹ�ã�
    HCOMM_GETREVLEN,            // ��ȡ���յ��ĳ��ȣ�WPARAM��δʹ�ã�LPARAM��DWORD* ���泤�ȵ� DWORD �ͱ����ĵ�ַ��
    HCOMM_READDATA,             // �������ݣ�WPARAM��δʹ�ã�LPARAM��ָ�� HANCOMREADDATA �ṹ���ָ�룩
    HCOMM_WRITEDATA,            // �������ݣ�WPARAM�����ȣ�LPARAM ׼�����͵����ݣ�
} HANCOMMESSAGE;
#endif

#if 1 // ֪ͨ��
// �򿪶˿ں󣬴��ڲ��񵽶˿����ݺ���򸸴��ڷ��� WM_NOTIFY ��Ϣ
// �±��� LPNMHDR->code ���ܳ��ֵ�����֪ͨ��
typedef enum {
    HCOMN_GETDATA,              // �˿��յ����ݣ���֪ͨ����ڽ��������ڼ��������
    HCOMN_GETMSG,               // �˿��յ����ģ���֪ͨ����ڽ��ս������ٴ���
    HCOMN_OVERFLOW,             // �˿ڽ��ջ����������֪ͨ����ڻ���������������
} HANCOMNOTIFYCODE;
#endif

typedef struct tagHCOMREADDATA {
    void*   pData;              // �������ݵ�����
    DWORD   nBufSize;           // ׼�����յĳ��ȣ���ֵ����С�ڵ��� pData ָ��������С
    DWORD*  nRevSize;           // ���ն�����ʵ���յ��ĳ���
} HANCOMREADDATA, * HANPCOMREADDATA;

/* ע�ᴮ���࣬Ӧ�ó�����ʹ�ô˿ؼ�ǰ�����ȵ��øú���ע�ᴰ����
 */
void RegisterHANCOM(HINSTANCE hInstance);
/* ��ʾ�򿪴��ڰ�ť
 * bShow        �Ƿ���ʾ�򿪴��ڰ�ť��TRUE����ʾ��FALSE�����أ�
 */
void HANCOMShowOpenCOM(HWND hCOM, BOOL bShow);
/* ��ʾ�˿ں�
 * bShow        �Ƿ���ʾ�˿ںţ�TRUE����ʾ��FALSE�����أ�
 */
void HANCOMShowPortId(HWND hCOM, BOOL bShow);
/* ��ʾ������
 * bShow        �Ƿ���ʾ�˿ںţ�TRUE����ʾ��FALSE�����أ�
 */
void HANCOMShowBaudrate(HWND hCOM, BOOL bShow);
/* ��ʾ�ֽڴ�С
 * bShow        �Ƿ���ʾ�ֽڴ�С��TRUE����ʾ��FALSE�����أ�
 */
void HANCOMShowByteSize(HWND hCOM, BOOL bShow);
/* ��ʾУ��λ
 * bShow        �Ƿ���ʾУ��λ��TRUE����ʾ��FALSE�����أ�
 */
void HANCOMShowParity(HWND hCOM, BOOL bShow);
/* ��ʾֹͣλ
 * bShow        �Ƿ���ʾֹͣλ��TRUE����ʾ��FALSE�����أ�
 */
void HANCOMShowStopBits(HWND hCOM, BOOL bShow);
void HANCOMMoveOpenCOM(HWND hCOM, int x, int y, int w, int h, BOOL bRepaint);
void HANCOMMovePortId(HWND hCOM, int x, int y, int w, int h, BOOL bRepaint);
void HANCOMMoveBaudrate(HWND hCOM, int x, int y, int w, int h, BOOL bRepaint);
void HANCOMMoveByteSize(HWND hCOM, int x, int y, int w, int h, BOOL bRepaint);
void HANCOMMoveParity(HWND hCOM, int x, int y, int w, int h, BOOL bRepaint);
void HANCOMMoveStopBits(HWND hCOM, int x, int y, int w, int h, BOOL bRepaint);
/* �򿪴���
 */
BOOL HANCOMOpenCOM(HWND hCOM);
/* ���ö˿ں�
 */
HAN_errno_t HANCOMSetPortId(HWND hCOM, uint16_t nPortId);
/* ���ò�����
 */
BOOL HANCOMSetBaudrate(HWND hCOM, DWORD nBaudrate);
/* �����ֽڴ�С
 */
BOOL HANCOMSetByteSize(HWND hCOM, BYTE nByteSize);
/* ����У��λ
 */
BOOL HANCOMSetParity(HWND hCOM, BYTE nParity);
/* ����ֹͣλ
 */
BOOL HANCOMSetStopBits(HWND hCOM, BYTE nStopBits);
/* ��ȡ���ڻ������յ������ݳ���
 */
HAN_errno_t HANCOMGetRevLen(HWND hCOM, DWORD* pLen);
/* ��ȡ����
 */
HAN_errno_t HANCOMReadData(HWND hCOM, void* pData, DWORD nBufSize, DWORD* nRevSize);
/* ��������
 */
HAN_errno_t HANCOMWriteData(HWND hCOM, const void* pData, DWORD nLen);
#endif

#ifdef __cplusplus
}
#endif

#endif
