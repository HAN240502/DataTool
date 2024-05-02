#ifndef HAN_FILE_CONVERSION_TYPEDEF_H
#define HAN_FILE_CONVERSION_TYPEDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#define HAN_FILE_CONVERSION_INFO_MAX            32

#define FILE_CONVERSION_USER_SETTING_CFG_STR_SIZE   260
#define FILE_CONVERSION_USER_SETTING_CFG_CNT_MAX    30

#define Fc_snprintf             snprintf
#define Fc_strlen               strlen

typedef char                    FCCHAR;
typedef char*                   PFCSTR;
typedef const char*             PCFCSTR;

typedef struct tagUSERSETTING {
    FCCHAR                      pCfg[FILE_CONVERSION_USER_SETTING_CFG_CNT_MAX][FILE_CONVERSION_USER_SETTING_CFG_STR_SIZE];  // ������õ��ַ���
    uint32_t                    nCfgCnt;                                                                                    // ���ø���
    void*                       pData;                                                                                      // �������ݣ��û�����ʹ��
} USERSETTING, * PUSERSETTING;

typedef struct tagFILECONVERSION {
    uint8_t                     pHeader[32];
    uint32_t                    nHeaderLen;
    PFCSTR                      pTitle;
    PFCSTR                      pMsgName;
    uint32_t                    (*GetHeaderAction)(const uint8_t* pMsg, int nMsgLen, PUSERSETTING pUserSetting, HANDLE hDestFile);
} FILECONVERSION, * PFILECONVERSION;

typedef struct tagFILECONVERSIONUSERSETTING {
    /* ��ʼ���û�����
     * pUserSetting             �Զ������ýṹ��ָ��
     * hParentWnd               �����ھ�����������̨������������Ϊ NULL
     * hParentInst              ����ʵ�����
     */
    void                        (*InitUserSetting)(PUSERSETTING pUserSetting, HWND hParentWnd, HINSTANCE hInst);
    /* �����û��Զ������ô���
     * pUserSetting             �Զ������ýṹ��ָ��
     */
    void                        (*UserSettingButtonAction)(PUSERSETTING pUserSetting);
    /* �����û��Զ�������
     * pUserSetting             �Զ������ýṹ��ָ��
     */
    void                        (*SaveUserSetting)(PUSERSETTING pUserSetting);
} FILECONVERSIONUSERSETTING, * PFILECONVERSIONUSERSETTING;

typedef struct tagFILECONVERSIONINFO {
    /* ���ļ�ʱִ�еĲ���
     * pSrcName                 ϵͳ�򿪵�Դ�ļ���
     * pDestName                ��Ž���������ݵ��ļ��������û�����
     * return                   �Ƿ�������ļ���TRUE�������ļ���FALSE�������ļ��������������ļ�����ܻᱻϵͳ�ٴδ򿪣��û��ɸ����ļ���׺������ FALSE �Ժ��Դ����ļ���
     */
    bool                        (*OpenSrcFileAction)(PCFCSTR pSrcName, PFCSTR pDestName);
    /* �ļ�ת����Ϣ����������
     */
    FILECONVERSION              pFileConversion[HAN_FILE_CONVERSION_INFO_MAX];
    /* �ļ�ת����Ϣ���ʵ�ʳ���
     */
    uint32_t                    nFileConversionCnt;
    /* �Զ���������Ϣ
     */
    FILECONVERSIONUSERSETTING   fcUserSetting;
} FILECONVERSIONINFO, * PFILECONVERSIONINFO;

#ifdef __cplusplus
}
#endif

#endif
