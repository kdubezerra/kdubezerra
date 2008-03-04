- - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - 
IMPORTANT (MSVC 8 update):

For Visual Studio 2003 (MSVC 7):

- Use minigame.vcproj / minigame.sln

For Visual Studio 2005 (MSVC 8):

- Use minigame_msvc8.vcproj / minigame_msvc8.sln

Unfortunately, the MSVC 8 project file converter doesn't work 
(upgrading the MSVC 7 project files turns them into crap).

- - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - 

The minigame.sln compiles both the Minigame app (minigame.vcproj) and 
its dependency, the ZIG library (ziglib/zig2.vcproj). 
See "ziglib/README.zig2.vcproj.txt" for more details


The minigame.vcproj:


- Should compile the minigame without too much hassle. 


- It doesn't provide you with the path to your own copies of the 
HawkNL 1.7 beta 1 <nl.h> header file and .lib file. You must make your 
VC IDE point to it (VC7/2003: Tools->Options->Projects->VC++Directories, 
and then fix "Include files" and "Library Files" to find your HawkNL 
header and binaries).
  
  HawkNL library is available at:  http://www.hawksoft.com/hawknl/


- It also doesn't provide paths to the Allegro library. See item above.

  Allegro library is available at:  http://alleg.sf.net/


- The generated executable files are placed on a "bin" subdirectory under 
the parent directory. 

So, if you are using:

  C:\zig2\minigame\minigame.vcproj

Then, you will get all .exe files generated like this: 

  C:\zig2\bin\minigame.exe        // Release build
  C:\zig2\bin\minigamed.exe       // Debug build


