#include "GlobalVariables.h"
#include <string.h>

LOGFONT         g_lfSysFont = {
    .lfHeight = 17,
    .lfWidth = 9,
    .lfEscapement = 0,
    .lfOrientation = 0,
    .lfWeight = FW_NORMAL,
    .lfItalic = FALSE,
    .lfUnderline = FALSE,
    .lfStrikeOut = FALSE,
    .lfCharSet = DEFAULT_CHARSET,
    .lfOutPrecision = OUT_CHARACTER_PRECIS,
    .lfClipPrecision = CLIP_CHARACTER_PRECIS,
    .lfQuality = DEFAULT_QUALITY,
    .lfPitchAndFamily = FF_DONTCARE,
    .lfFaceName = TEXT("ºÚÌå"),
};
LOGFONT         g_lfHexFont = {
    .lfHeight = 17,
    .lfWidth = 9,
    .lfEscapement = 0,
    .lfOrientation = 0,
    .lfWeight = FW_NORMAL,
    .lfItalic = FALSE,
    .lfUnderline = FALSE,
    .lfStrikeOut = FALSE,
    .lfCharSet = DEFAULT_CHARSET,
    .lfOutPrecision = OUT_CHARACTER_PRECIS,
    .lfClipPrecision = CLIP_CHARACTER_PRECIS,
    .lfQuality = DEFAULT_QUALITY,
    .lfPitchAndFamily = FF_DONTCARE,
    .lfFaceName = TEXT("Lucida Console"),
};

HANDATACFG      g_cfgHANData = {
    .cfgFileConversion = {
        .pSysConfig = {
            [INI_FILE_CONVERSION_DLL_PATH] = {
                .pKey = TEXT("DllPath"),
                .pDefValue = TEXT(""),
            },
            [INI_FILE_CONVERSION_CONVERT_PATH] = {
                .pKey = TEXT("ConvertPath"),
                .pDefValue = TEXT(""),
            },
            [INI_FILE_CONVERSION_CONVERT_FOLDER] = {
                .pKey = TEXT("ConvertFolder"),
                .pDefValue = TEXT("TRUE"),
            },
            [INI_FILE_CONVERSION_FILTER] = {
                .pKey = TEXT("Filter"),
                .pDefValue = TEXT("0"),
            },
        },
        .usUserConfig = {
            .nCfgCnt = 0,
        }
    }
};

HANCHAR         g_pIniFileName[PATH_STR_SIZE];

void GetIniFileName(HANPSTR pIniFileName, uint32_t nStrSize)
{
    strncpy(pIniFileName, g_pIniFileName, nStrSize);
}
