#ifndef HAN_FILE_CONVERSION_H
#define HAN_FILE_CONVERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "HAN_FileConversionTypedef.h"

#define FILE_CONVERSION_PRINT_SIZE_FORMAT   "%I64u"
#define FILE_CONVERSION_PRINT_OK_TEXT       "������ɣ��ļ���С " FILE_CONVERSION_PRINT_SIZE_FORMAT " Bytes\r\n"\
                                            "��Ч�ֽ��� " FILE_CONVERSION_PRINT_SIZE_FORMAT " Bytes�������� " FILE_CONVERSION_PRINT_SIZE_FORMAT "\r\n"
#define FILE_CONVERSION_PRINT_NO_DATA       "����Ч����\r\n"

typedef enum {
    FILECONVERSION_ERRNO_OK,
    FILECONVERSION_ERRNO_OPEN_DLL_FAIL,
    FILECONVERSION_ERRNO_MSG_CNT_ERR,
    FILECONVERSION_ERRNO_NOT_COMPATIBLE,
} FILECONVERSIONERRNO;

typedef struct tagFILECONVERSIONPARAM {
    void*               pSrcData;                                                           // Դ����
    size_t              nSrcBufSize;                                                        // Դ���ݻ����С
    size_t              nSrcDataSize;                                                       // Դ����ʵ�ʴ�С
    void*               pDestFile;                                                          // ��Ž���������ݵ��ļ�ָ��

    PFILECONVERSIONINFO pConvertMsgInfo;                                                    // Э����Ϣ
    int*                pFilter;                                                            // Э�����
    PUSERSETTING        pUserSetting;                                                       // �û��Զ�������
    
    void                (*WriteFile)(void* pData, uint32_t nLen, void* pFile);              // д�ļ��Ľӿ�

    void                (*PrintReport)(PCFCSTR pReport, void* pUserParam);                  // ��ӡ LOG �Ľӿ�
    void                (*UpdateProgress)(uint32_t i, uint32_t iMax, void* pUserParam);     // ���½��ȵĽӿ�
    void*               pUserParam;                                                         // �û��Զ������
} FILECONVERSIONPARAM, * PFILECONVERSIONPARAM;

void ConvertFile(PFILECONVERSIONPARAM pParam);

#ifdef __cplusplus
}
#endif

#endif
