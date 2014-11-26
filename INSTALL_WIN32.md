# Installation on Windows

This documentation describes how to set up a workspace for trying to compile Gazebo on Windows.  It does not actually work yet.

## Supported compilers

At this moment, compilation has been testewd on Windows 7 and is supported 
when using Visual Studio 2013. Patches for other versions are welcome.

## Installation

Totally experimental, using pre-compiled binaries in a local workspace.  To make things easier, use a MinGW shell for your editing work, and only use the
Windows `cmd` for configuring and building.  You might also need to [disable the Windows firewall](http://windows.microsoft.com/en-us/windows/turn-windows-firewall-on-off#turn-windows-firewall-on-off=windows-7).  Not sure about that.

1. Make a directory to work in, e.g.:

        mkdir gz-ws
        cd gz-ws

1. Download the following things into that directory:

    - [Protobuf 2.6.0](http://packages.osrfoundation.org/win32/deps/protobuf-2.6.0-win64-vc12.zip)
    - [Boost 1.56.0](http://packages.osrfoundation.org/win32/deps/boost_1_56_0.zip)
    - [OGRE 1.8.1](http://packages.osrfoundation.org/win32/deps/OgreSDK_vc10_v1-8-1.zip)

1. Unzip each of them in gz-ws.

1. Also download and execute the Qt 4.8 installer.  It should end up somewhere like C:\Qt.  As far as I can tell, this installation is not relocatable, so cannot be put into a .zip and dropped into a local workspace:

    - [Qt 4.8.6](http://packages.osrfoundation.org/win32/deps/qt-opensource-windows-x86-vs2010-4.8.6.exe)

1. Clone sdformat and gazebo:

        hg clone https://bitbucket.org/osrf/sdformat
        hg clone https://bitbucket.org/osrf/gazebo

1. Load your compiler setup, e.g. (note that we are asking for the 64-bit toolchain here):

        "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64

1. Configure and build sdformat:

        cd sdformat
        hg up win_gerkey
        mkdir build
        cd build
        ..\configure
        nmake
        nmake install

    You should now have an installation of sdformat in gz-ws/sdformat/build/install/Release.

1. Configure and build gazebo:

        cd gazebo
        hg up win_gerkey
        mkdir build
        cd build
        ..\configure
        nmake
        nmake install

    Once this all works (which it currently does not, by a long shot): you should now have an installation of gazebo in gz-ws/sdformat/build/install/Release.
