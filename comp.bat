gcc ^
-g ^
*.c ^
HAN_Lib\*.c ^
DataTool\*.c ^
DataTool\DataCmd\*.c ^
DataTool\FileConversion\*.c ^
DataTool\HexView\*.c ^
DataTool\BinView\*.c ^
DataTool\SrecView\*.c ^
DataTool\AscView\*.c ^
DataTool\ComTool\*.c ^
DataTool\ComTool\ComToolModel\GlobalVariables\*.c ^
DataTool\ComTool\ComToolModel\Consle\*.c ^
-lgdi32 ^
-lcomdlg32 ^
-lcomctl32 ^
-lshlwapi ^
-Wall -Wextra ^
-o DataTool.exe

DataTool.exe