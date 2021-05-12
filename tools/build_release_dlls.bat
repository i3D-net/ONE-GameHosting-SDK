@REM For Windows machines, there's an option to build both Windows and Linux libraries using build_release_dlls.bat script.
@REM This script creates libraries for both systems and both architectures (x86 and x64).

@REM It does the following steps:

@REM 1. Creates the folder structure for the build (root folder called shared_lib_build).
@REM 2. Creates DLL files using build_release_windows_dll_32_mt.bat and build_release_windows_dll_64_mt.bat.
@REM 3. Builds the Docker image:
@REM     3.1. Creates the internal folder structure.
@REM     3.2. Builds SO files using build_release_linux_so_32.sh and build_release_linux_so_64.sh.
@REM 4. Creates a temporary Docker container.
@REM 5. Copies SO files from the container file system into the shared_lib_build folder.
@REM 6. Removes the temporary container.

SET OutputDir=shared_lib_build
SET ContainerOutputPath=/one/lib_build
SET ScriptsFolder=tools
SET Dockerfile=LinuxReleaseSharedBuild.dockerfile
SET DockerImageName=one-sdk
SET TempContainerName=one-sdk-temp

IF EXIST ..\%OutputDir%\ (
	rmdir /s /q ..\%OutputDir%\
)

mkdir ..\%OutputDir%

IF EXIST ..\build\ (
	CALL .\clean_windows.bat
	cd %ScriptsFolder%
)

CALL .\build_release_windows_dll_64_mt.bat
cd %ScriptsFolder%

mkdir ..\%OutputDir%\Arcus
mkdir ..\%OutputDir%\Arcus\Windows
mkdir ..\%OutputDir%\Arcus\Windows\x64
mkdir ..\%OutputDir%\Ping
mkdir ..\%OutputDir%\Ping\Windows
mkdir ..\%OutputDir%\Ping\Windows\x64
copy ..\build\one\arcus\Release\one_arcus.dll ..\%OutputDir%\Arcus\Windows\x64
copy ..\build\one\ping\Release\one_ping.dll ..\%OutputDir%\Ping\Windows\x64

CALL .\clean_windows.bat
cd %ScriptsFolder%

CALL .\build_release_windows_dll_32_mt.bat
cd %ScriptsFolder%

mkdir ..\%OutputDir%\Arcus\Windows\x86
mkdir ..\%OutputDir%\Ping\Windows\x86
copy ..\build\one\arcus\Release\one_arcus.dll ..\%OutputDir%\Arcus\Windows\x86
copy ..\build\one\ping\Release\one_ping.dll ..\%OutputDir%\Ping\Windows\x86

CALL .\clean_windows.bat

mkdir %OutputDir%\Arcus\Linux
mkdir %OutputDir%\Arcus\Linux\x86
mkdir %OutputDir%\Arcus\Linux\x64
mkdir %OutputDir%\Ping\Linux
mkdir %OutputDir%\Ping\Linux\x86
mkdir %OutputDir%\Ping\Linux\x64

docker build -t %DockerImageName%:latest -f %ScriptsFolder%/%Dockerfile% .
docker rm %TempContainerName% >nul 2>&1
docker run --name %TempContainerName% %DockerImageName%:latest
docker cp %TempContainerName%:%ContainerOutputPath%/x86/libone_arcus.so ./%OutputDir%/Arcus/Linux/x86
docker cp %TempContainerName%:%ContainerOutputPath%/x64/libone_arcus.so ./%OutputDir%/Arcus/Linux/x64
docker cp %TempContainerName%:%ContainerOutputPath%/x86/libone_ping.so ./%OutputDir%/Ping/Linux/x86
docker cp %TempContainerName%:%ContainerOutputPath%/x64/libone_ping.so ./%OutputDir%/Ping/Linux/x64
docker rm %TempContainerName%
