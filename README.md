# Project Spartan Dbg
A simple command line exe to launch Project Spartan and immediately suspend all
threads in the process. It can then optionally start an application such as a
debugger and provide the process id for spartan.exe through the command-line.
A url can be provided that Project Spartan will open as soon as all threads are
resumed.

#Usage
1. Build the solution or [download v0.3]
  (https://github.com/SkyLined/ProjectSpartanDbg/releases/download/0.3/ProjectSpartanDbg.zip)
2. Start Project Spartan with all threads suspended and attach a debugger, e.g.
  
  ```
  C:\path> ProjectSpartanDbg.exe http://%COMPUTERNAME%:28876 cdb.exe -o -p @pid@
  Project Spartan process id = 320
  
  Microsoft (R) Windows Debugger Version 6.3.9600.16384 X86
  Copyright (c) Microsoft Corporation. All rights reserved.
  <<<snip>>>
  (140.13c): Break instruction exception - code 80000003 (first chance)
  <<<snip>>>
  ntdll!DbgBreakPoint:
  77ec3060 cc              int     3
  0:021>
  ```
3. Resume all threads and run spartan to have it open the specified URL, e.g. 
  
  ```
  0:021>~*m
  0:021>g
  ```
