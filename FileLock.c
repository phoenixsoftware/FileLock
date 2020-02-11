/*
 ============================================================================
 Name        : FileLock.c
 Author      : Eric Chevalier
 Version     :
 Copyright   : Copyright 2020 Phoenix Software International
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#define TRUE	1
#define	FALSE	0


static void ReportError(char* tag)
{
    int lastError = errno;
    printf("%s. errno %d: %s.\n", tag, lastError, strerror(lastError));
}


static void AdvancedLockTest(int fd, struct stat *fileInfo, int lock, char* tag)
{
	size_t bytesRead, bytesToRead;
    unsigned char Buffer[203];
    time_t startTime, endTime, duration;
    struct tm durStruct;
    struct flock lockParms;
    off_t fileSize = fileInfo->st_size;
    int count = 0;

    startTime = time(NULL);

    while (fileSize > 0) {
    	count += 1;

        if (lock) {
        	lockParms.l_type = F_RDLCK;
        	lockParms.l_whence = SEEK_SET;
        	lockParms.l_start = 2147481429;
        	lockParms.l_len = 1;
            if (-1 == fcntl(fd, F_GETLK, &lockParms)) {
                ReportError("LockFile fails");
                break;
            }
        }

        bytesToRead = fileSize > sizeof Buffer ? sizeof Buffer : fileSize;
        bytesRead = read(fd, Buffer, bytesToRead);

        if (lock) {
            if (-1 == fcntl(fd, F_SETLK, &lockParms)) {
                ReportError("UnlockFile fails");
                break;
            }
        }

        if (-1 == bytesRead) {
            ReportError("ReadFile fails");
            break;
        }

        fileSize -= bytesRead;
    }

    endTime = time(NULL);
    duration = endTime - startTime;
    gmtime_r(&duration, &durStruct);
    strftime((char*) Buffer, sizeof Buffer, "Duration: %H:%M:%S", &durStruct);
    printf("%-32s%s  %6d loop iterations\n", tag, Buffer, count);
}


int main(int argc, char **argv) {
    int fd;

    if (argc < 2) {
        printf("Command line is missing an input file name!\n");
        return EXIT_FAILURE;
    }

    fd = open(argv[1], O_RDONLY);

    if (-1 == fd)
        ReportError("Cannot open input file");

    else {
        struct stat fileInfo;

        if (-1 == fstat(fd, &fileInfo))
            ReportError("GetFileInformationByHandle fails");

        else {
            AdvancedLockTest(fd, &fileInfo, TRUE,  "Testing with locking.");
            lseek(fd, 0, SEEK_SET);
            AdvancedLockTest(fd, &fileInfo, FALSE, "Testing without locking.");
        }

        close(fd);
    }
	return EXIT_SUCCESS;
}
