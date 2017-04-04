@echo off
rem Copyright (C) 1996-2016 Free Software Foundation, Inc.
rem This file is part of GNU Make.
rem
rem GNU Make is free software; you can redistribute it and/or modify it under
rem the terms of the GNU General Public License as published by the Free
rem Software Foundation; either version 3 of the License, or (at your option)
rem any later version.
rem
rem GNU Make is distributed in the hope that it will be useful, but WITHOUT
rem ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
rem FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for.
rem more details.
rem
rem You should have received a copy of the GNU General Public License along
rem with this program.  If not, see <http://www.gnu.org/licenses/>.

call :Reset

if "%1" == "-h" goto Usage
if "%1" == "--help" goto Usage

set MAKE=gnumake
set GUILE=Y
set COMPILER=msvc

:ParseSW
if "%1" == "--debug" goto SetDebug
if "%1" == "--without-guile" goto NoGuile
if "%1" == "gcc" goto SetCC
if "%1" == "" goto DoneSW

:SetDebug
set DEBUG=Y
shift
goto ParseSW

:NoGuile
set GUILE=N
echo Building without Guile
shift
goto ParseSW

:SetCC
set COMPILER=gcc
echo Building with GCC
shift
goto ParseSW

rem Build with Guile is supported only on NT and later versions
:DoneSW
echo.
echo Creating GNU Make for Windows 9X/NT/2K/XP/Vista/7/8
if "%DEBUG%" == "Y" echo Building without compiler optimizations

if "%COMPILER%" == "gcc" goto GccBuild

set OUTDIR=.\WinRel
set "OPTS=/O2 /D NDEBUG"
set LINKOPTS=
if "%DEBUG%" == "Y" set OUTDIR=.\WinDebug
if "%DEBUG%" == "Y" set "OPTS=/Zi /Od /D _DEBUG"
if "%DEBUG%" == "Y" set LINKOPTS=/DEBUG
call :Build
goto Done

:GccBuild
set OUTDIR=.\GccRel
set OPTS=-O2
if "%DEBUG%" == "Y" set OPTS=-O0
if "%DEBUG%" == "Y" set OUTDIR=.\GccDebug
call :Build
goto Done

:Done
call :Reset
goto :EOF

:Build
:: Clean the directory if it exists
if exist %OUTDIR%\nul rmdir /S /Q %OUTDIR%

:: Recreate it
mkdir %OUTDIR%
mkdir %OUTDIR%\glob
mkdir %OUTDIR%\w32
mkdir %OUTDIR%\w32\compat
mkdir %OUTDIR%\w32\subproc

if "%GUILE%" == "Y" call :ChkGuile

echo.
echo Compiling %OUTDIR% version

if exist config.h.W32.template call :ConfigSCM
copy config.h.W32 %OUTDIR%\config.h

call :Compile ar
call :Compile arscan
call :Compile commands
call :Compile default
call :Compile dir
call :Compile expand
call :Compile file
call :Compile function
call :Compile getloadavg
call :Compile getopt
call :Compile getopt1
call :Compile glob\fnmatch
call :Compile glob\glob
call :Compile guile GUILE
call :Compile hash
call :Compile implicit
call :Compile job
call :Compile load
call :Compile loadapi
call :Compile main GUILE
call :Compile misc
call :Compile output
call :Compile read
call :Compile remake
call :Compile remote-stub
call :Compile rule
call :Compile signame
call :Compile strcache
call :Compile variable
call :Compile version
call :Compile vpath
call :Compile w32\compat\posixfcn
call :Compile w32\pathstuff
call :Compile w32\subproc\misc
call :Compile w32\subproc\sub_proc
call :Compile w32\subproc\w32err
call :Compile w32\w32os

if not "%COMPILER%" == "gcc" call :Compile w32\compat\dirent

call :Link

echo.
if not exist %OUTDIR%\%MAKE%.exe echo %OUTDIR% build FAILED!
if exist %OUTDIR%\%MAKE%.exe echo %OUTDIR% build succeeded.
goto :EOF

:Compile
set EXTRAS=
if "%2" == "GUILE" set "EXTRAS=%GUILECFLAGS%"
if "%COMPILER%" == "gcc" goto GccCompile

:: MSVC Compile
echo on
cl.exe /nologo /MT /W4 /EHsc %OPTS% /I %OUTDIR% /I . /I glob /I w32/include /D WINDOWS32 /D WIN32 /D _CONSOLE /D HAVE_CONFIG_H /FR%OUTDIR% /Fp%OUTDIR%\%MAKE%.pch /Fo%OUTDIR%\%1.obj /Fd%OUTDIR%\%MAKE%.pdb %EXTRAS% /c %1.c
@echo off
echo %OUTDIR%\%1.obj >>%OUTDIR%\link.sc
goto :EOF

:GccCompile
:: GCC Compile
echo on
gcc -mthreads -Wall -std=gnu99 -gdwarf-2 -g3 %OPTS% -I%OUTDIR% -I. -I./glob -I./w32/include -DWINDOWS32 -DHAVE_CONFIG_H %EXTRAS% -o %OUTDIR%\%1.o -c %1.c
@echo off
goto :EOF

:Link
echo Linking %OUTDIR%/%MAKE%.exe
if "%COMPILER%" == "gcc" goto GccLink

:: MSVC Link
echo %GUILELIBS% kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib >>%OUTDIR%\link.sc
echo on
link.exe /NOLOGO /SUBSYSTEM:console /PDB:%OUTDIR%\%MAKE%.pdb %LINKOPTS% /OUT:%OUTDIR%\%MAKE%.exe @%OUTDIR%\link.sc
@echo off
goto :EOF

:GccLink
:: GCC Link
echo on
gcc -mthreads -gdwarf-2 -g3 -o %OUTDIR%\%MAKE%.exe %OUTDIR%\variable.o %OUTDIR%\rule.o %OUTDIR%\remote-stub.o %OUTDIR%\commands.o %OUTDIR%\file.o %OUTDIR%\getloadavg.o %OUTDIR%\default.o %OUTDIR%\signame.o %OUTDIR%\expand.o %OUTDIR%\dir.o %OUTDIR%\main.o %OUTDIR%\getopt1.o %OUTDIR%\guile.o %OUTDIR%\job.o %OUTDIR%\output.o %OUTDIR%\read.o %OUTDIR%\version.o %OUTDIR%\getopt.o %OUTDIR%\arscan.o %OUTDIR%\remake.o %OUTDIR%\misc.o %OUTDIR%\hash.o %OUTDIR%\strcache.o %OUTDIR%\ar.o %OUTDIR%\function.o %OUTDIR%\vpath.o %OUTDIR%\implicit.o %OUTDIR%\loadapi.o %OUTDIR%\load.o %OUTDIR%\glob\glob.o %OUTDIR%\glob\fnmatch.o %OUTDIR%\w32\pathstuff.o %OUTDIR%\w32\compat\posixfcn.o %OUTDIR%\w32\w32os.o %OUTDIR%\w32\subproc\misc.o %OUTDIR%\w32\subproc\sub_proc.o %OUTDIR%\w32\subproc\w32err.o %GUILELIBS% -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -Wl,--out-implib=%OUTDIR%\libgnumake-1.dll.a
@echo off
goto :EOF

:ConfigSCM
echo Generating config from SCM templates
sed -n "s/^AC_INIT(\[GNU make\],\[\([^]]\+\)\].*/s,%%VERSION%%,\1,g/p" configure.ac > %OUTDIR%\config.h.W32.sed
echo s,%%PACKAGE%%,make,g >> %OUTDIR%\config.h.W32.sed
sed -f %OUTDIR%\config.h.W32.sed config.h.W32.template > config.h.W32
echo static const char *const GUILE_module_defn = ^" \> gmk-default.h
sed -e "s/;.*//" -e "/^[ \t]*$/d" -e "s/\"/\\\\\"/g" -e "s/$/ \\\/" gmk-default.scm >> gmk-default.h
echo ^";>> gmk-default.h
goto :EOF

:ChkGuile
if not "%OS%" == "Windows_NT" goto NoGuile
pkg-config --help > %OUTDIR%\guile.tmp 2> NUL
if ERRORLEVEL 1 goto NoPkgCfg

echo Checking for Guile 2.0
if not "%COMPILER%" == "gcc" set PKGMSC=--msvc-syntax
pkg-config --cflags --short-errors "guile-2.0" > %OUTDIR%\guile.tmp
if not ERRORLEVEL 1 set /P GUILECFLAGS= < %OUTDIR%\guile.tmp

pkg-config --libs --static --short-errors %PKGMSC% "guile-2.0" > %OUTDIR%\guile.tmp
if not ERRORLEVEL 1 set /P GUILELIBS= < %OUTDIR%\guile.tmp

if not "%GUILECFLAGS%" == "" goto GuileDone

echo Checking for Guile 1.8
pkg-config --cflags --short-errors "guile-1.8" > %OUTDIR%\guile.tmp
if not ERRORLEVEL 1 set /P GUILECFLAGS= < %OUTDIR%\guile.tmp

pkg-config --libs --static --short-errors %PKGMSC% "guile-1.8" > %OUTDIR%\guile.tmp
if not ERRORLEVEL 1 set /P GUILELIBS= < %OUTDIR%\guile.tmp

if not "%GUILECFLAGS%" == "" goto GuileDone

echo No Guile found, building without Guile
goto GuileDone

:NoPkgCfg
echo pkg-config not found, building without Guile

:GuileDone
if "%GUILECFLAGS%" == "" goto :EOF

echo Guile found, building with Guile
set "GUILECFLAGS=%GUILECFLAGS% -DHAVE_GUILE"
goto :EOF

:Usage
echo Usage: %0 [options] [gcc]
echo Options:
echo.  --debug           For GCC only, make a debug build
echo.                    (MSVC build always makes both debug and release)
echo.  --without-guile   Do not compile Guile support even if found
echo.  --help            Display these instructions and exit
goto :EOF

:Reset
set COMPILER=
set DEBUG=
set GUILE=
set GUILECFLAGS=
set GUILELIBS=
set LINKOPTS=
set MAKE=
set NOGUILE=
set OPTS=
set OUTDIR=
set PKGMSC=
goto :EOF
