@echo off

%UT%  > %TT_CASE%.log 2>&1
if %ERRORLEVEL% neq 0 (
    type %TT_CASE%.log
    exit /b -1
)

set LEAK=0

findstr /c:"[0 ipc] are opened" %TT_CASE%.log > NULL 2>&1
if %ERRORLEVEL% neq 0 (
    echo ====================================
    echo ipc LEAK:
    findstr /c:"<<IPC>>" %TT_CASE%.log
    echo ====================================
    set LEAK=1
)

findstr /c:"[0 sockets] are opened" %TT_CASE%.log > NULL 2>&1
if %ERRORLEVEL% neq 0 (
    echo ====================================
    echo socket LEAK:
    findstr /c:"<<Socket>>" %TT_CASE%.log
    echo ====================================
    set LEAK=1
)

findstr /c:"[0 blocks][0 bytes] are allocated" %TT_CASE%.log > NULL 2>&1
if %ERRORLEVEL% neq 0 (
    echo ====================================
    echo memory LEAK:
    findstr /c:"<<Memory>>" %TT_CASE%.log
    echo ====================================
    set LEAK=1
)

if %LEAK% neq 0 (
    exit /b -1
)
