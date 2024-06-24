#include "DataToolInfo.h"
#include "FileConversion\HAN_FileConversionWindow.h"
#include "HexView\HAN_Hex.h"
#include "BinView\HAN_Bin.h"
#include "SrecView\HAN_Srec.h"
#include "AscView\HAN_Asc.h"
#include "ComTool\HAN_ComTool.h"

static FILECONVERSIONCFG    g_cfgFileConversion;
static BINVIEWCFG           g_cfgBinView;
static HEXVIEWCFG           g_cfgHexView;
static SRECVIEWCFG          g_cfgSrecView;
static COMTOOLCFG           sg_cfgComTool;

DATATOOLINFO    g_tiTabInfo[DATA_TOOL_ID_CNT_MAX] = {
    [DATA_TOOL_ID_FILE_CONVERSION] = {
        .pTitle = TEXT("文件转换"),
        .pClass = HAN_FILE_CONVERSION_CLASS,
        .RegisterHANTabWindow = RegisterHANFileConversion,
        .ReadIniFile = ReadFileConversionIniFile,
        .WriteIniFile = WriteFileConversionIniFile,
        .pIni = &g_cfgFileConversion,
    },
    [DATA_TOOL_ID_BIN_VIEW] = {
        .pTitle = TEXT("bin"),
        .pClass = HAN_BIN_VIEW_CLASS,
        .RegisterHANTabWindow = RegisterHANBinView,
        .ReadIniFile = ReadBinViewIniFile,
        .WriteIniFile = WriteBinViewIniFile,
        .pIni = &g_cfgBinView,
    },
    [DATA_TOOL_ID_HEX_VIEW] = {
        .pTitle = TEXT("hex"),
        .pClass = HAN_HEX_VIEW_CLASS,
        .RegisterHANTabWindow = RegisterHANHexView,
        .ReadIniFile = ReadHexViewIniFile,
        .WriteIniFile = WriteHexViewIniFile,
        .pIni = &g_cfgHexView,
    },
    [DATA_TOOL_ID_SREC_VIEW] = {
        .pTitle = TEXT("srec"),
        .pClass = HAN_SREC_VIEW_CLASS,
        .RegisterHANTabWindow = RegisterHANSrecView,
        .ReadIniFile = ReadSrecViewIniFile,
        .WriteIniFile = WriteSrecViewIniFile,
        .pIni = &g_cfgSrecView,
    },
    [DATA_TOOL_ID_ASC_VIEW] = {
        .pTitle = TEXT("asc"),
        .pClass = HAN_ASC_VIEW_CLASS,
        .RegisterHANTabWindow = RegisterHANAscView,
        .ReadIniFile = NULL,
        .WriteIniFile = NULL,
        .pIni = NULL,
    },
    [DATA_TOOL_ID_COM_TOOL] = {
        .pTitle = TEXT("串口工具"),
        .pClass = HAN_COM_TOOL_CLASS,
        .RegisterHANTabWindow = RegisterHANComTool,
        .ReadIniFile = ReadComToolIniFile,
        .WriteIniFile = WriteComToolIniFile,
        .pIni = &sg_cfgComTool,
    },
};
