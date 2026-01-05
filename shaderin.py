import ctypes
import math
import random
import time
from ctypes import wintypes

user32 = ctypes.WinDLL('user32', use_last_error=True)
gdi32 = ctypes.WinDLL('gdi32', use_last_error=True)
msimg32 = ctypes.WinDLL('msimg32', use_last_error=True)

# Constants
SM_CXSCREEN = 0
SM_CYSCREEN = 1
BI_RGB = 0
DIB_RGB_COLORS = 0
AC_SRC_OVER = 0x00
AC_SRC_ALPHA = 0x01

# Structures
class BITMAPINFOHEADER(ctypes.Structure):
    _fields_ = [
        ('biSize', wintypes.DWORD),
        ('biWidth', wintypes.LONG),
        ('biHeight', wintypes.LONG),
        ('biPlanes', wintypes.WORD),
        ('biBitCount', wintypes.WORD),
        ('biCompression', wintypes.DWORD),
        ('biSizeImage', wintypes.DWORD),
        ('biXPelsPerMeter', wintypes.LONG),
        ('biYPelsPerMeter', wintypes.LONG),
        ('biClrUsed', wintypes.DWORD),
        ('biClrImportant', wintypes.DWORD),
    ]

class BITMAPINFO(ctypes.Structure):
    _fields_ = [
        ('bmiHeader', BITMAPINFOHEADER),
        ('bmiColors', wintypes.DWORD * 3),
    ]

class BLENDFUNCTION(ctypes.Structure):
    _fields_ = [
        ('BlendOp', wintypes.BYTE),
        ('BlendFlags', wintypes.BYTE),
        ('SourceConstantAlpha', wintypes.BYTE),
        ('AlphaFormat', wintypes.BYTE),
    ]

# Function prototypes
user32.GetSystemMetrics.restype = ctypes.c_int
user32.GetDC.argtypes = [wintypes.HWND]
user32.GetDC.restype = wintypes.HDC
user32.ReleaseDC.argtypes = [wintypes.HWND, wintypes.HDC]
user32.ReleaseDC.restype = ctypes.c_int

gdi32.CreateCompatibleDC.argtypes = [wintypes.HDC]
gdi32.CreateCompatibleDC.restype = wintypes.HDC
gdi32.DeleteDC.argtypes = [wintypes.HDC]
gdi32.DeleteDC.restype = wintypes.BOOL

gdi32.CreateDIBSection.argtypes = [wintypes.HDC, ctypes.POINTER(BITMAPINFO), wintypes.UINT, ctypes.POINTER(ctypes.c_void_p), wintypes.HANDLE, wintypes.DWORD]
gdi32.CreateDIBSection.restype = wintypes.HBITMAP

gdi32.SelectObject.argtypes = [wintypes.HDC, wintypes.HGDIOBJ]
gdi32.SelectObject.restype = wintypes.HGDIOBJ

gdi32.DeleteObject.argtypes = [wintypes.HGDIOBJ]
gdi32.DeleteObject.restype = wintypes.BOOL

msimg32.AlphaBlend.argtypes = [wintypes.HDC, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
                              wintypes.HDC, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
                              BLENDFUNCTION]
msimg32.AlphaBlend.restype = wintypes.BOOL

# Globals
g_running = ctypes.c_long(1)

def main():
    random.seed(int(time.time()))

    W = user32.GetSystemMetrics(SM_CXSCREEN)
    H = user32.GetSystemMetrics(SM_CYSCREEN)

    hdcScreen = user32.GetDC(None)
    hdcMem = gdi32.CreateCompatibleDC(hdcScreen)

    bmi = BITMAPINFO()
    ctypes.memset(ctypes.byref(bmi), 0, ctypes.sizeof(bmi))
    bmi.bmiHeader.biSize = ctypes.sizeof(BITMAPINFOHEADER)
    bmi.bmiHeader.biWidth = W
    bmi.bmiHeader.biHeight = -H  # top-down
    bmi.bmiHeader.biPlanes = 1
    bmi.bmiHeader.biBitCount = 32
    bmi.bmiHeader.biCompression = BI_RGB

    pBits = ctypes.c_void_p()
    hb = gdi32.CreateDIBSection(hdcScreen, ctypes.byref(bmi), DIB_RGB_COLORS, ctypes.byref(pBits), None, 0)
    if not hb or not pBits:
        if hb:
            gdi32.DeleteObject(hb)
        gdi32.DeleteDC(hdcMem)
        user32.ReleaseDC(None, hdcScreen)
        return 1

    old = gdi32.SelectObject(hdcMem, hb)

    # Fill pixels
    pixel_array = (ctypes.c_uint32 * (W * H)).from_address(pBits.value)
    for y in range(H):
        fy = y / float(H)
        for x in range(W):
            fx = x / float(W)
            s = 0.5 + 0.5 * math.sin(fx * 10.0 + fy * 6.0)
            r = int(120 + 80 * s)
            g = int(180 + 50 * (1.0 - s))
            b = int(200 + 40 * s)
            a = 110
            pixel_array[y * W + x] = ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF)

    bf = BLENDFUNCTION()
    bf.BlendOp = AC_SRC_OVER
    bf.BlendFlags = 0
    bf.SourceConstantAlpha = 255
    bf.AlphaFormat = AC_SRC_ALPHA

    # Message loop simulation
    while g_running.value == 1:
        hdcDesktop = user32.GetDC(None)
        msimg32.AlphaBlend(hdcDesktop, 0, 0, W, H, hdcMem, 0, 0, W, H, bf)
        user32.ReleaseDC(None, hdcDesktop)
        time.sleep(0.1)

    gdi32.SelectObject(hdcMem, old)
    gdi32.DeleteObject(hb)
    gdi32.DeleteDC(hdcMem)
    user32.ReleaseDC(None, hdcScreen)
    return 0

if __name__ == '__main__':
    main()
