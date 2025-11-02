#include <windows.h>
#include <cstdint>
#include <cmath>
#include <ctime>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "msimg32.lib")

static volatile LONG g_running = 1;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

    std::srand((unsigned)std::time(nullptr));

    const int W = GetSystemMetrics(SM_CXSCREEN);
    const int H = GetSystemMetrics(SM_CYSCREEN);
    if (W <= 0 || H <= 0) return 0;

    HDC hdcDesktop = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcDesktop);

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = W;
    bmi.bmiHeader.biHeight = -H; 
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hb = CreateDIBSection(hdcDesktop, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (!hb || !pBits) {
        if (hb) DeleteObject(hb);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcDesktop);
        return 1;
    }

    HGDIOBJ old = SelectObject(hdcMem, hb);

    uint32_t* ob = (uint32_t*)pBits;
    for (int y = 0; y < H; ++y) {
        float fy = y / (float)H;
        for (int x = 0; x < W; ++x) {
            float fx = x / (float)W;
            float s = 0.5f + 0.5f * sinf(fx * 10.0f + fy * 6.0f);

            int r = 0;
            int g = (int)roundf(200.0f + 55.0f * s);
            int b = (int)roundf(230.0f + 25.0f * s);            
            int a = 110; 

            if (r < 0) r = 0; if (r > 255) r = 255;
            if (g < 0) g = 0; if (g > 255) g = 255;
            if (b < 0) b = 0; if (b > 255) b = 255;

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
        HDC hdc = GetDC(NULL);
        AlphaBlend(hdc, 0, 0, W, H, hdcMem, 0, 0, W, H, bf);
        ReleaseDC(NULL, hdc);


            Sleep(100);
    }

    SelectObject(hdcMem, old);
    DeleteObject(hb);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcDesktop);
    return 0;
}
