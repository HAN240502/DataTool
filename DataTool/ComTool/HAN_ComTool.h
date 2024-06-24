#ifndef HAN_COM_TOOL_H
#define HAN_COM_TOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

#include "HAN_ComToolDef.h"

#define HAN_COM_TOOL_CLASS      TEXT("ComTool")

#define SYS_TEXT_W       80
#define SYS_TEXT_H       21

void RegisterHANComTool(HINSTANCE hInst);

/* ��ʼ������
 * pIniPath                 ini �ļ�·��
 * pParam                   ������õĽṹ��ָ��
 */
void ReadComToolIniFile(HANPCSTR pIniPath, void* pParam);

/* ��������
 * pIniPath                 ini �ļ�·��
 * hComTool                 ���ھ��
 */
void WriteComToolIniFile(HANPCSTR pIniPath, HWND hComTool);

#ifdef __cplusplus
}
#endif

#endif
