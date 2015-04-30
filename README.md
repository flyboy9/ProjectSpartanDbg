# Project Spartan Dbg
A simple command line exe to launch Project Spartan to open a URL and suspend
all threads in the process. It will output the id of the spartan.exe process.
This can be used to attach a debugger before Project Spartan opens the URL.
Tip: use `~*m` in WinDBG/cdb to resume all threads once you've attached the
debugger.

#Usage
1. Build the solution
2. Start Project Spartan with all threads suspended
    e.g. `ProjectSpartanDbg.exe http://%COMPUTERNAME%:28876` 
3. Attach your debugger using the provided process id
    e.g. `cdb -o -p [process id]`
4. Resume all threads
    e.g. `~*m`


