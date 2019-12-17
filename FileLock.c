#include <windows.h>
#include <stdio.h>


static void ReportError(char* tag)
{
    char errorText[512];

    DWORD lastError = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        errorText, sizeof(errorText) / sizeof(errorText[0]), NULL);

    printf("%s. GetLastError %d: %s.\n", tag, lastError, errorText);
}


static void LockTest(
    HANDLE hFile,
    DWORD  dwFileOffsetLow,
    DWORD  dwFileOffsetHigh,
    DWORD  nNumberOfBytesToLockLow,
    DWORD  nNumberOfBytesToLockHigh,
    char* tag
    )
{
    printf("%s\n", tag);

    if (!LockFile(hFile,
                dwFileOffsetLow,
                dwFileOffsetHigh,
                nNumberOfBytesToLockLow,
                nNumberOfBytesToLockHigh))
        ReportError("LockFile fails");

    else {
        if (!UnlockFile(hFile,
            dwFileOffsetLow,
            dwFileOffsetHigh,
            nNumberOfBytesToLockLow,
            nNumberOfBytesToLockHigh))

            ReportError("UnlockFile fails");
    }

    printf("LockTest finished.\n\n");
}


void main(int argc, char *argv[])
{
    HANDLE hFile;

    if (argc < 2) {
        printf("Command line is missing an input file name!\n");
        return;
    }

    hFile = CreateFile(argv[1],
        GENERIC_WRITE | GENERIC_READ,
        0,                          // do not share
        NULL,                       // no security
        OPEN_EXISTING,              // existing file only
        FILE_ATTRIBUTE_NORMAL,      // normal file
        NULL);                      // no attr. template

    if (hFile == INVALID_HANDLE_VALUE)
        ReportError("Cannot open input file");

    else {
        BY_HANDLE_FILE_INFORMATION fileInfo;

        if (!GetFileInformationByHandle(hFile, &fileInfo))
            ReportError("GetFileInformationByHandle fails");

        else {
            LockTest(hFile, 0, 0, fileInfo.nFileSizeLow, fileInfo.nFileSizeHigh, "Locking entire file.");
            LockTest(hFile, 0, 0, 1, 0, "Locking first byte.");
            LockTest(hFile, fileInfo.nFileSizeLow - 1, 0, 1, 0, "Locking last byte.");
            LockTest(hFile, 2147481429, 0, 1, 0, "Locking weird offset issued by CodeBase.");
        }

        CloseHandle(hFile);
    }
}
