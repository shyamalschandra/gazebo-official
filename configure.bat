@set build_type=Release
@if not "%1"=="" set build_type=%1
@echo Configuring for build type %build_type%

@set BOOST_PATH=%cd%\..\..\boost_1_56_0
@set BOOST_LIBRARY_DIR=%BOOST_PATH%\lib64-msvc-12.0

@set PROTOBUF_PATH=%cd%\..\..\protobuf-2.6.0-win64-vc12

@set CURL_PATH=%cd%\..\..\libcurl-vc12-x64-release-static-ipv6-sspi-winssl
@set CURL_INCLUDE_DIR=%CURL_PATH%\include
@set CURL_LIBRARY_DIR=%CURL_PATH%\lib

@set FREEIMAGE_PATH=%cd%\..\..\FreeImage-vc12-x64-release-debug
@set FREEIMAGE_LIBRARY_DIR=%FREEIMAGE_PATH%\x64\%build_type%
@set FREEIMAGE_INCLUDE_DIR=%FREEIMAGE_PATH%\Source

@set SDFORMAT_PATH=%cd%\..\..\sdformat\build\install\%build_type%

@set TBB_PATH=%cd%\..\..\tbb43_20141023oss
@set TBB_LIBRARY_DIR=%TBB_PATH%\lib\intel64\vc12
@set TBB_INCLUDE_DIR=%TBB_PATH%\include

@set OGRE_VERSION=1.9.0
@set OGRE_PATH=%cd%\..\..\OgreSDK_vc11_x64_v1-9-0unstable
@set OGRE_INCLUDE_DIR=%OGRE_PATH%\include;%OGRE_PATH%\include\OGRE;%OGRE_PATH%\include\OGRE\RTShaderSystem;%OGRE_PATH%\include\OGRE\Terrain;%OGRE_PATH%\include\OGRE\Paging
@set OGRE_LIBRARY_DIR=%OGRE_PATH%\lib\%build_type%
@set OGRE_PLUGIN_DIR=%OGRE_LIBRARY_DIR%\opt
@set OGRE_LIBS=%OGRE_LIBRARY_DIR%\OgreMain.lib;%OGRE_LIBRARY_DIR%\OgreOverlay.lib;%OGRE_LIBRARY_DIR%\OgreRTShaderSystem.lib;%OGRE_LIBRARY_DIR%\OgreTerrain.lib;%OGRE_LIBRARY_DIR%\OgrePaging.lib

@set DLFCN_WIN32_PATH=%cd%\..\..\dlfcn-win32-vc12-x64-release-debug\build\install\%build_type%
@set DLFCN_WIN32_LIBRARY_DIR=%DLFCN_WIN32_PATH%\lib
@set DLFCN_WIN32_INCLUDE_DIR=%DLFCN_WIN32_PATH%\include

@set QT4_PATH=C:\Qt\4.8.6\x64\msvc2013
@set QT4_BIN_DIR=%QT4_PATH%\bin

@set INCLUDE=%FREEIMAGE_INCLUDE_DIR%;%TBB_INCLUDE_DIR%;%DLFCN_WIN32_INCLUDE_DIR%;%INCLUDE%
@set LIB=%FREEIMAGE_LIBRARY_DIR%;%BOOST_LIBRARY_DIR%;%TBB_LIBRARY_DIR%;%DLFCN_WIN32_LIBRARY_DIR%;%LIB%
@set PATH=%QT4_BIN_DIR%;%PATH%

cmake -G "NMake Makefiles" -DCMAKE_PREFIX_PATH="%SDFORMAT_PATH%" -DFREEIMAGE_RUNS=1 -DPROTOBUF_SRC_ROOT_FOLDER="%PROTOBUF_PATH%" -DBOOST_ROOT:STRING="%BOOST_PATH%" -DBOOST_LIBRARYDIR:STRING="%BOOST_LIBRARY_DIR%" -DOGRE_FOUND=1 -DOGRE-RTShaderSystem_FOUND=1 -DOGRE-Terrain_FOUND=1 -DOGRE_VERSION=%OGRE_VERSION% -DOGRE_PLUGINDIR="%OGRE_PLUGIN_DIR%" -DOGRE_INCLUDE_DIRS="%OGRE_INCLUDE_DIR%" -DOGRE_LIBRARIES="%OGRE_LIBS%" -DCURL_FOUND=1 -DCURL_INCLUDEDIR="%CURL_INCLUDE_DIR%" -DCURL_LIBDIR="%CURL_LIBRARY_DIR%" -DCURL_LIBRARIES="libcurl_a" -DCMAKE_INSTALL_PREFIX="install\%build_type%" -DCMAKE_BUILD_TYPE="%build_type%" ..
