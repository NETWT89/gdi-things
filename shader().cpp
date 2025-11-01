#include <windows.h>
#include <cstdint>
#include <cmath>
#include <ctime>
#include <cstdlib>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "msimg32.lib")

static volatile LONG g_running = 1;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    std::srand((unsigned)std::time(nullptr));

    const int W = GetSystemMetrics(SM_CXSCREEN);
    const int H = GetSystemMetrics(SM_CYSCREEN);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = W;
    bmi.bmiHeader.biHeight = -H; 
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hb = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (!hb || !pBits) {
        if (hb) DeleteObject(hb);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
        return 1;
    }

    HGDIOBJ old = SelectObject(hdcMem, hb);

    uint32_t* ob = (uint32_t*)pBits;
    for (int y = 0; y < H; ++y) {
        float fy = y / (float)H;
        for (int x = 0; x < W; ++x) {
            float fx = x / (float)W;
            float s = 0.5f + 0.5f * sinf(fx * 10.0f + fy * 6.0f);
            int r = (int)(120 + 80 * s);
            int g = (int)(180 + 50 * (1.0f - s));
            int b = (int)(200 + 40 * s);
            int a = 110;
            ob[y * W + x] = ((uint32_t)(a & 0xFF) << 24) | ((uint32_t)(r & 0xFF) << 16) | ((uint32_t)(g & 0xFF) << 8) | (uint32_t)(b & 0xFF);
        }
    }

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;

    MSG msg;
    while (InterlockedCompareExchange(&g_running, 1, 1) == 1) {
        HDC hdcDesktop = GetDC(NULL);
        AlphaBlend(hdcDesktop, 0, 0, W, H, hdcMem, 0, 0, W, H, bf);
        ReleaseDC(NULL, hdcDesktop);

 Sleep(100);

    }
    SelectObject(hdcMem, old);
    DeleteObject(hb);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    return 0;
}
