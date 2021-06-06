copy src\bscaero.ico bscaero.ico

call %QTDIR%/bin/qtenv2.bat

cd /D "%~dp0"

echo %cd%

rmdir /S /Q bscaero

mkdir bscaero
mkdir bscaero\bin
mkdir bscaero\data
copy /y bin\*.exe bscaero\bin
xcopy data bscaero\data /E
copy "%QTDIR%\bin\Qt5OpenGL.dll" bscaero\bin
copy "%QWT_ROOT%\lib\qwt.dll" bscaero\bin

rem cd qt\bin
rem call "%QTDIR%\bin\windeployqt.exe" --release mscsim.exe
rem cd ..\..
call "%QTDIR%\bin\windeployqt.exe" --release bscaero\bin\bscaero.exe

"C:\Program Files\7-Zip\7z.exe" a bscaero_0.1_win64.zip -r bscaero\

pause
