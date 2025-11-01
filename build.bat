@echo off

:: Move to the path where the bat file itself is located
cd /D "%~dp0"

:: TODO: Remove this line
:: Call the batch file that sets up the MSVC compiler for 64-bit compilation
call vcvars64.bat > nul

:: --- Some usefull things to create variable of ------------------------------
set caller_path=%cd%
set out_dir_name="build"
set out_exe_name="main"

:: --- Creating the build dir ------------------------------------------------- 
rmdir %out_dir_name% /s /q
mkdir %out_dir_name%
:: Removing error messages that some files used by Visual Studio were not deleted, because captured 
cls

:: --- Unpack Arguments -------------------------------------------------------
for %%a in (%*) do set "%%a=1"

if not "%msvc%"=="1" echo [msvc compiler]

set debug_mode=1
set release_mode=0

if "%debug%"=="1" (
  set debug_mode=1
  set release_mode=0
)
if "%release%"=="1" (
  set debug_mode=0
  set release_mode=1
)

if "%debug_mode%"=="1"   echo [debug_mode]
if "%release_mode%"=="1" echo [release_mode]

if "%release_mode%"=="1" (echo [release_mode_not_supported] && exit /B 1)

pushd %out_dir_name%
    set forder_to_include=/I ../src ^
                          /I ../extern_includes 

    :: These will also be automatically linked together 
    set source_files_to_compile=../src/main.cpp
                              
    set libs_to_link_with=
                          :: ../includes/raylib/include/raylib.lib ^
                          :: user32.lib ^
                          :: gdi32.lib ^
                          :: shell32.lib ^
                          :: winmm.lib

    set compiler_macro_DEBUG_MODE=/D DEBUG_MODE=0
    if "%debug_mode%"=="1" set compiler_macro_DEBUG_MODE=/D DEBUG_MODE=1
    
    set compiler_macro_UNICODE=/D UNICODE=1

    set compile_time_values=%compiler_macro_DEBUG_MODE% ^
                            %compiler_macro_UNICODE%

    cl /nologo /Zi /MDd /EHsc  %compile_time_values% %forder_to_include% %source_files_to_compile% ^
      /link %libs_to_link_with%
popd  

cd %caller_path%

















