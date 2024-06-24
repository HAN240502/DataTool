#ifndef HAN_COM_TOOL_CONSLE_H
#define HAN_COM_TOOL_CONSLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

#include "..\..\HAN_ComToolDef.h"

void RegisterHANComToolConsle(HINSTANCE hInst);

HWND CreateComToolWindow(HANINT x, HANINT y, HWND hComTool, HMENU winId, HINSTANCE hInst, PCOMTOOLEXTRA ctInfo);

void ReadComToolConsleIniFile(HANPCSTR pIniPath, void* pParam);

void WriteComToolConsleIniFile(HANPCSTR pIniPath, HWND hComTool);

void LoadComToolCfg(PCOMTOOLEXTRA ctInfo, PCOMTOOLCFG pCtConfig);

#ifdef __cplusplus
}
#endif

#endif
