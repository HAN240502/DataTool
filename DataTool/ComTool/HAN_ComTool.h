#ifndef HAN_COM_TOOL_H
#define HAN_COM_TOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

#include "..\..\GlobalVariables.h"
#include "..\..\HAN_Lib\HAN_windows.h"

#define HAN_COM_TOOL_CLASS      TEXT("ComTool")

#define SYS_TEXT_W       80
#define SYS_TEXT_H       21

void RegisterHANComTool(HINSTANCE hInst);

#ifdef __cplusplus
}
#endif

#endif
