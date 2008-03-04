- - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - 
IMPORTANT (MSVC 8 update):

For Visual Studio 2003 (MSVC 7):

- Use zig2.vcproj / zig2.sln

For Visual Studio 2005 (MSVC 8):

- Use zig2_msvc8.vcproj / zig2_msvc8.sln

Unfortunately, the MSVC 8 project file converter doesn't work 
(upgrading the MSVC 7 project files turns them into crap).

- - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - 

The zig2.vcproj:


- Should compile zig2 library without too much hassle. At least it knows 
where to find its own include files now! :-)


- It doesn't provide you with the path to your own copies of the 
HawkNL 1.7 beta 1 <nl.h> header file and .lib file. You must make your 
VC IDE point to it (VC7/2003: Tools->Options->Projects->VC++Directories, 
and then fix "Include files" and "Library Files" to find your HawkNL 
header and binaries).


- The generated library files are placed on a "bin" subdirectory under 
the parent directory. 

So, if you are using:

  C:\zig2\ziglib\zig2.vcproj

Then, you will get all .lib files generated like this: 

  C:\zig2\bin\zig2.lib        // Release build
  C:\zig2\bin\zig2d.lib       // Debug build


- Since this zig2.vcproj generates different .lib file names for Debug 
(zig2d.lib) and Release (zig2.lib) builds, you can match the Release 
and Debug configs of your game project to link to the appropriate .lib 
file.

