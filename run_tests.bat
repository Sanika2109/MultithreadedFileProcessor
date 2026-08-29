@echo off
setlocal

echo.
echo ========================================
echo   MULTITHREADED FILE PROCESSOR TESTS
echo ========================================
echo.

REM --------------------------------------------------
REM Check compiler
REM --------------------------------------------------

where g++ >nul 2>&1

if %errorlevel% neq 0 (
    echo ERROR: g++ was not found in PATH.
    echo.
    echo Run this script from an MSYS2 UCRT64 terminal
    echo or add the UCRT64 compiler to your PATH.
    exit /b 1
)

echo Compiler found:
g++ --version | findstr /C:"g++"
echo.


REM --------------------------------------------------
REM Compile FileProcessor tests
REM --------------------------------------------------

echo ----------------------------------------
echo Compiling FileProcessor tests...
echo ----------------------------------------

g++ -std=c++17 ^
tests/FileProcessorTest.cpp ^
src/FileProcessor.cpp ^
-I/ucrt64/include ^
-I./src ^
-L/ucrt64/lib ^
-lgtest_main ^
-lgtest ^
-pthread ^
-o FileProcessorTest.exe

if %errorlevel% neq 0 (
    echo.
    echo ERROR: FileProcessorTest compilation failed.
    exit /b 1
)

echo Compilation successful.
echo.


REM --------------------------------------------------
REM Compile TaskQueue tests
REM --------------------------------------------------

echo ----------------------------------------
echo Compiling TaskQueue tests...
echo ----------------------------------------

g++ -std=c++17 ^
tests/TaskQueueTest.cpp ^
src/TaskQueue.cpp ^
-I/ucrt64/include ^
-I./src ^
-L/ucrt64/lib ^
-lgtest_main ^
-lgtest ^
-pthread ^
-o TaskQueueTest.exe

if %errorlevel% neq 0 (
    echo.
    echo ERROR: TaskQueueTest compilation failed.
    exit /b 1
)

echo Compilation successful.
echo.


REM --------------------------------------------------
REM Compile ThreadPool tests
REM --------------------------------------------------

echo ----------------------------------------
echo Compiling ThreadPool tests...
echo ----------------------------------------

g++ -std=c++17 ^
tests/ThreadPoolTest.cpp ^
src/ThreadPool.cpp ^
src/TaskQueue.cpp ^
src/FileProcessor.cpp ^
-I/ucrt64/include ^
-I./src ^
-L/ucrt64/lib ^
-lgtest_main ^
-lgtest ^
-pthread ^
-o ThreadPoolTest.exe

if %errorlevel% neq 0 (
    echo.
    echo ERROR: ThreadPoolTest compilation failed.
    exit /b 1
)

echo Compilation successful.
echo.


REM --------------------------------------------------
REM Run FileProcessor tests
REM --------------------------------------------------

echo ========================================
echo   FILE PROCESSOR TESTS
echo ========================================
echo.

FileProcessorTest.exe

if %errorlevel% neq 0 (
    echo.
    echo FileProcessor tests FAILED.
    exit /b 1
)

echo.


REM --------------------------------------------------
REM Run TaskQueue tests
REM --------------------------------------------------

echo ========================================
echo   TASK QUEUE TESTS
echo ========================================
echo.

TaskQueueTest.exe

if %errorlevel% neq 0 (
    echo.
    echo TaskQueue tests FAILED.
    exit /b 1
)

echo.


REM --------------------------------------------------
REM Run ThreadPool tests
REM --------------------------------------------------

echo ========================================
echo   THREAD POOL TESTS
echo ========================================
echo.

ThreadPoolTest.exe

if %errorlevel% neq 0 (
    echo.
    echo ThreadPool tests FAILED.
    exit /b 1
)

echo.


REM --------------------------------------------------
REM Final result
REM --------------------------------------------------

echo ========================================
echo   ALL TESTS PASSED
echo ========================================
echo.
echo FileProcessor : 5/5 PASSED
echo TaskQueue     : 6/6 PASSED
echo ThreadPool    : 6/6 PASSED
echo.
echo TOTAL          : 17/17 PASSED
echo.
echo ========================================

exit /b 0