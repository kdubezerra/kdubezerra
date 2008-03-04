
ZIG Library - A game-oriented networking library

Version 2.1.0 - BETA QUALITY

Date: 5/9/2007 (DMY)

*****************************************************************************
  PLEASE REFER TO THE PROJECT WEBSITE FOR UP-TO-DATE INFORMATION:
  http://sf.net/projects/zige/
*****************************************************************************

*****************************************************************************
  QUESTIONS? E-mail Fabio Cecin (fcecin AT gmail DOT com) or join the 
  zige-users mailing list (see http://sf.net/projects/zige/)
*****************************************************************************

Copyright acknowledgements:
        
* The ZIG library includes, in it's sources, the sources of the bzip2 
  library (http://sources.redhat.com/bzip2) version 1.02 into ZIG. 
  The BZIP2 Library sources are Copyright (C) 1996-2002 Julian R Seward.  
  All rights reserved. For details about the BZIP2 license, check the 
  full Copyright notice at the file "bzip2.h" or visit the bzip2 website.

----------------
Release contents
----------------

  NOTE: ZIG's library headers documentation is generated thanks to 
        Doxygen (http://www.stack.nl/~dimitri/doxygen/), and is now
        available as a separate download ("zig-documentation" package).
        get it at: http://sf.net/projects/zige


ziglib/  - the ziglite library, sources, headers, Visual Studio 2003 
           and 2005 project files, and Linux (gcc) Makefile.
  
           For Visual Studio 2003 (MSVC7):
            - zig2.sln contains only zig2.vcproj
            - zig2.vcproj builds zig2.lib (Release) and zig2d.lib (Debug) 
            for static-linking with your application.

           For Visual Studio 2005 (MSVC8):
            - zig2_msvc8.sln contains only zig2_msvc8.vcproj
            - zig2_msvc8.vcproj builds zig2.lib (Release) and zig2d.lib 
            (Debug) for static-linking with your application.

           For Linux (gcc):
            - Makefile

ziglib/include/ - This is the directory which contains the include files 
                 (*.h) of ZIG. You will need to somehow inform this 
                 directory, as an "additional include directory", to your 
                 compiler so that you can compile ZIG 2 apps.


bin/     - This directory is created when binaries are produced from the 
           project files (minigame .exe files and zig2 .lib files). You 
           will need to direct your linker to this directory as "Additional 
           library directories" (or the like). Unless you plan to compile 
           ZIG on your own and/or move the generated .lib files somewhere 
           else.


minigame/ - A very clean example of using the ziglite library. This is a 
            VERY MINIMAL client-server game where players move coloured 
            balls by moving their mouses. Directory contains the source, 
            Visual Studio 2003 and 2005 project files, and Linux (gcc) 
            Makefile.

            For Visual Studio 2003 (MSVC7):
             - minigame.sln contains minigame.vcproj & zig2.vcproj, so if 
               the ziglib/ sources are altered or if the zig library is 
               not built yet, it will build zig's .lib file first, then 
               the minigame, and then will link the minigame executable.

            For Visual Studio 2005 (MSVC8):
             - The same, but files are named instead: minigame_msvc8.sln, 
               minigame_msvc8.vcproj & zig2_msvc8.vcproj.
   
            For Linux (gcc):
             - Makefile

            NOTE: PRE-BUILD windows executables of "minigame" and the 
            DLLs needed to play it have been moved to a different 
            download package. please download it from the "zig-minigame" 
            package at: http://sf.net/projects/zige/ if you can't build 
            it yourself or if you just want to play.


streamtest/ - An application that we use to test the "streams" feature.


doc/     - This directory should be created to contain the documentation.
           You can download the documentation package from:  

              http://sf.net/projects/zige

           ...or you can generate it yourself with Doxygen version 1.5.2 
           or later.


Obj/     - This is a temporary directory created by the MSVC project 
           files to receive intermediate files. you can always delete it.


