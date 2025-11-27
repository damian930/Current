@echo off

:: Damian: This is here, just cause i cant memorise the cloc flag to remove the folder
:: cloc src --exclude-dir=third_party --by-file

setlocal enabledelayedexpansion

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
echo --------------------------------------------------------------------------

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

:: NOTE: In the bat, when using if statements, the '(' has to be on the same line as the if stmt.
if "%samples%"=="1" (
  echo [sample_build]
  pushd "src/_samples_"
  (
    if "%text_loading%"=="1" echo [text_loading] & cl /nologo /I ../ /Zi text_loading.cpp 
    if "%os_mem%"=="1" echo [os_mem] & cl /nologo /I ../ /Zi os_mem.cpp 
    if "%process_data%"=="1" echo [process_data] & cl /nologo /I ../ /Zi process_data.cpp 
    if "%defere_init_macro%"=="1" echo [defere_init_macro] & cl /nologo /I ../ /Zi defere_init_macro.cpp
    if "%just_main%"=="1" echo [just_main] & cl /nologo /I ../ /Zi just_main.cpp 
    
    
    :: This is a special case of a sample program, this one need special compilation 3rt party code
    if "%dear_ui%"=="1" (
      echo [dear_ui]
      pushd "custom_init_for_dear_im_gui_1_00"
      (
        cl /nologo /I ../../ /I "dear_im_gui" /I /Zi "main.cpp" "dear_im_gui\imgui.cpp" 
      )
      popd
    )

    if EXIST *.obj del *.obj
    if EXIST *.ilk del *.ilk
  )
  popd
  exit /b
)

:: ELSE

if "%debug_mode%"=="1"   echo [debug_mode]
if "%release_mode%"=="1" echo [release_mode]

if "%release_mode%"=="1" (echo [release_mode_not_supported] && exit /B 1)

pushd %out_dir_name%
    set forder_to_include=/I ../src

    :: These will also be automatically linked together 
    set source_files_to_compile=../src/main.cpp
                              
    set libs_to_link_with=

    set compiler_macro_DEBUG_MODE=/D DEBUG_MODE=0
    if "%debug_mode%"=="1" set compiler_macro_DEBUG_MODE=/D DEBUG_MODE=1
    
    set compiler_macro_UNICODE=/D UNICODE=1

    set compile_time_values=%compiler_macro_DEBUG_MODE% ^
                            %compiler_macro_UNICODE%

    set compile_warning_to_suppress=/wd4201 ^
                                    /wd4189 ^
                                    /wd4996

    cl /nologo /Zi /MDd /EHsc /W4 %compile_time_values% %forder_to_include% %compile_warning_to_suppress% %source_files_to_compile% ^
      /link %libs_to_link_with%

  :: Notes on suppressed warning:
  :: C4201 --> nonstandard extension used: nameless struct/union
  :: C4189 --> local variable is initialized but not referenced
  :: C4996 --> function is deprecated (this is for stb stuff)
popd  



cd %caller_path%

















