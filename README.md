# Project Spartan Dbg
A simple command line exe to launch Project Spartan to open a URL and suspend
all threads in the process. It will output the id of the spartan.exe process.
This can be used to attach a debugger before Project Spartan opens the URL.
Tip: use `~*m` in WinDBG/cdb to resume all threads once you've attached the
debugger.

#Usage
1. Build the solution or [download v0.2]
  (https://github.com/SkyLined/ProjectSpartanDbg/releases/download/0.2/ProjectSpartanDbg.zip)
2. Start Project Spartan with all threads suspended, e.g.
  ```
  > ProjectSpartanDbg.exe http://%COMPUTERNAME%:28876
  Project Spartan process id = 320
  >
  ```
3. Attach your debugger using the provided process id, e.g.
  ```
  > cdb -o -p 320
  Microsoft (R) Windows Debugger Version 6.3.9600.16384 X86
  Copyright (c) Microsoft Corporation. All rights reserved.
  <<<snip>>>
  (140.13c): Break instruction exception - code 80000003 (first chance)
  <<<snip>>>
  ntdll!DbgBreakPoint:
  77ec3060 cc              int     3
  0:021>
  ```
4. Resume all threads and run spartan, e.g. 
  ```
  0:021>~*m
  0:021>g
  ```


