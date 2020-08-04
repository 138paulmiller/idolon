
pushd %cd%
cd ..
set PYTHONHOME="C:\Program Files\idolon\root;..\code\external\Python3"
set PYTHONPATH="%cd%\code\external\Python3;%cd%\code\external\Python3\Lib;%cd%\code\external\Python3\DLLs"
"build/x64/Release/idolon.exe"
popd %cd%
