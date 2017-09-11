@echo off

%UT%  > %TT_CASE%.log 2>&1
if %ERRORLEVEL% neq 0 (
    type %TT_CASE%.log
    exit /b -1
)
