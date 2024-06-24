#ifndef HAN_DATA_TOOL_INFO_H
#define HAN_DATA_TOOL_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DataToolTypedef.h"
#include "FileConversion\HAN_FileConversionWindow.h"
#include "HexView\HAN_Hex.h"
#include "BinView\HAN_Bin.h"
#include "SrecView\HAN_Srec.h"
#include "AscView\HAN_Asc.h"
#include "ComTool\HAN_ComTool.h"

typedef enum {
    DATA_TOOL_ID_FILE_CONVERSION,
    DATA_TOOL_ID_HEX_VIEW,
    DATA_TOOL_ID_BIN_VIEW,
    DATA_TOOL_ID_SREC_VIEW,
    DATA_TOOL_ID_ASC_VIEW,
    DATA_TOOL_ID_COM_TOOL,
    DATA_TOOL_ID_CNT_MAX,
} DATATOOLID;

extern DATATOOLINFO         g_tiTabInfo[DATA_TOOL_ID_CNT_MAX];

#ifdef __cplusplus
}
#endif

#endif
