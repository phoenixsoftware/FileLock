# Windows FileLock Test Driver

This program was developed when we noticed some of our in-house Windows
applications stopped functioning properly when accessing files via NFS
instead of SMB. Using the Sysinternals tool "ProcMon.exe", we discovered
that one of these programs was suffering Windows error 50, "The request
is not supported" when issuing the __LockFile__ function.

__FileLock.exe__ simply opens a file and then issues a few variations of
__LockFile__, reporting the results after each test. The program
requires a single command-line argument: the fully-qualified path name
to the input file to be tested. For example:

    FileLock.exe P:\Viking\PCPASWRD\register.dbf

__FileLock.exe__ is a 32-bit Windows application that requires only the
standard C and Windows runtime libraries. It was built using Visual
Studio 2019.

