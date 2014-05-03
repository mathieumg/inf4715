@REM Create a engine42 source code snapshot
@setlocal
@ECHO off
@SET path="C:\Program Files\WinRAR\";%path%

RAR a -ap"Vicuna" -ag"{ }YYYY.MM" -x@"exclude.txt" -r -m5 -mt3 -t -- "Vicuna.rar" "..\..\*.*"
REM RAR a -ap"Vicuna" -ag"{ }YYYY.MM" -x@"exclude.txt" -x@"lite.txt" -r -m5 -mt3 -t -- "Vicuna Lite.rar" "..\..\*.*"
REM RAR a -ap"Vicuna\Tools" -ag"{ }YYYY.MM" -x@"exclude.txt" -r -m5 -mt3 -t -- "Vicuna Tools.rar" "..\*.*"
REM > rar.log

@REM End of script