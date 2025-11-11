// Edit : Remove the Hotkey.

#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

static volatile LONG g_running = 1;

struct IconSprite { HICON icon; float x,y,vx,vy; int life,maxlife; int w,h; };
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
{
    std::srand((unsigned)std::time(NULL));
    int W = GetSystemMetrics(SM_CXSCREEN);
    int H = GetSystemMetrics(SM_CYSCREEN);
    HDC hdcScreen = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
    HDC hdcCap = CreateCompatibleDC(hdcScreen);
    HBITMAP hbCap = CreateCompatibleBitmap(hdcScreen, W, H);
    HGDIOBJ oldCap = SelectObject(hdcCap, hbCap);
    BitBlt(hdcCap, 0, 0, W, H, hdcScreen, 0, 0, SRCCOPY | CAPTUREBLT);
    BITMAPV5HEADER hdr; ZeroMemory(&hdr, sizeof(hdr));
    hdr.bV5Size = sizeof(hdr); hdr.bV5Width = W; hdr.bV5Height = -H; hdr.bV5Planes = 1; hdr.bV5BitCount = 32; hdr.bV5Compression = BI_BITFIELDS;
    hdr.bV5RedMask = 0x00FF0000; hdr.bV5GreenMask = 0x0000FF00; hdr.bV5BlueMask = 0x000000FF; hdr.bV5AlphaMask = 0xFF000000;
    void* pBackBits = nullptr;
    HBITMAP hbBack = CreateDIBSection(hdcScreen, (BITMAPINFO*)&hdr, DIB_RGB_COLORS, &pBackBits, NULL, 0);
    HDC hdcBack = CreateCompatibleDC(hdcScreen);
    HGDIOBJ oldBack = SelectObject(hdcBack, hbBack);
    std::vector<IconSprite> icons; icons.reserve(512);
    std::vector<unsigned int> pixels; pixels.resize(W*H);
    std::vector<HICON> preload;
    HCURSOR curs[] = { LoadCursor(NULL, IDC_ARROW), LoadCursor(NULL, IDC_HAND), LoadCursor(NULL, IDC_IBEAM), LoadCursor(NULL, IDC_CROSS), LoadCursor(NULL, IDC_WAIT), LoadCursor(NULL, IDC_APPSTARTING), LoadCursor(NULL, IDC_NO), LoadCursor(NULL, IDC_SIZEALL), LoadCursor(NULL, IDC_SIZENS), LoadCursor(NULL, IDC_SIZEWE), LoadCursor(NULL, IDC_UPARROW), LoadCursor(NULL, IDC_ICON) };
    int cursCount = sizeof(curs)/sizeof(curs[0]);
    CURSORINFO ci; ci.cbSize = sizeof(ci); HICON curIcon = NULL; if (GetCursorInfo(&ci)) curIcon = ci.hCursor;
    for (int i=0;i<cursCount;i++) preload.push_back((HICON)curs[i]);
    if (curIcon) preload.push_back(curIcon);
    double freq=0; LARGE_INTEGER f; QueryPerformanceFrequency(&f); freq=(double)f.QuadPart;
    LARGE_INTEGER last,now; QueryPerformanceCounter(&last);
    double acc=0.0; const double dtTarget=1.0/60.0;
    int yOffset=0; const int speed=220;
    int recaptureFrames=28; int frame=0;
    while (InterlockedCompareExchange(&g_running,1,1)==1) {
        QueryPerformanceCounter(&now); double dt=(double)(now.QuadPart-last.QuadPart)/freq; last=now; acc+=dt;
        while (acc>=dtTarget) {
            acc-=dtTarget;
            yOffset = (yOffset + speed) % H;
            for (int s=0;s<6 && icons.size()<600; ++s) {
                if ((std::rand()%100) < 80) {
                    IconSprite is;
                    HICON h = preload[std::rand()%preload.size()];
                    is.icon = (HICON)CopyImage(h, IMAGE_ICON, 0, 0, LR_COPYRETURNORG);
                    is.w = 80 + (std::rand()%240);
                    is.h = is.w;
                    is.x = (float)(std::rand()%W);
                    is.y = (float)(std::rand()%H);
                    is.vx = ((std::rand()%401)-200) * 0.02f;
                    is.vy = ((std::rand()%200)+40) * 0.02f;
                    is.maxlife = 40 + (std::rand()%140);
                    is.life = is.maxlife;
                    icons.push_back(is);
                }
            }
            for (int p=0;p<40 && pixels.size()<1; ++p) {}
            if ((frame % recaptureFrames) == 0 && frame!=0) BitBlt(hdcCap,0,0,W,H,hdcScreen,0,0,SRCCOPY | CAPTUREBLT);
            for (size_t i=0;i<icons.size();++i) {
                icons[i].x += icons[i].vx;
                icons[i].y += icons[i].vy;
                icons[i].vy += 0.12f;
                icons[i].life--;
            }
            int wi=0;
            for (int i=0;i<(int)icons.size();++i) if (icons[i].life>0 && icons[i].y < H+icons[i].h) icons[wi++]=icons[i]; else DestroyIcon(icons[i].icon);
            icons.resize(wi);
            ++frame;
        }
        int center = H/2;
        float collapse = 0.76f;
        int step = 4;
        for (int dy=0; dy<H; dy+=step) {
            float y0 = (dy - center) / (float)center;
            float dist = fabs(y0);
            float scale = 1.0f - collapse * dist;
            float srcYf = center + (dy - center) * scale + (float)(yOffset);
            while (srcYf >= H) srcYf -= H;
            while (srcYf < 0) srcYf += H;
            int srcY = (int)srcYf;
            int copyH = step;
            if (srcY + copyH <= H) {
                StretchBlt(hdcBack, 0, dy, W, copyH, hdcCap, 0, srcY, W, copyH, SRCCOPY);
            } else {
                int firstH = H - srcY;
                StretchBlt(hdcBack, 0, dy, W, firstH, hdcCap, 0, srcY, W, firstH, SRCCOPY);
                StretchBlt(hdcBack, 0, dy+firstH, W, copyH-firstH, hdcCap, 0, 0, W, copyH-firstH, SRCCOPY);
            }
        }
        for (size_t i=0;i<icons.size();++i) {
            int dx = (int)roundf(icons[i].x - icons[i].w/2.0f);
            int dy = (int)roundf(icons[i].y - icons[i].h/2.0f);
            HICON h = icons[i].icon;
            DrawIconEx(hdcBack, dx, dy, h, icons[i].w, icons[i].h, 0, NULL, DI_NORMAL);
        }
        for (size_t i=0;i<pixels.size();++i) {
            pixels[i] = 0x00000000;
        }
        BITMAPINFO bmi; ZeroMemory(&bmi, sizeof(bmi));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = W; bmi.bmiHeader.biHeight = H; bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
        StretchDIBits(hdcBack, 0,0,W,H,0,0,W,H, pixels.data(), &bmi, DIB_RGB_COLORS, SRCPAINT);
        BitBlt(hdcScreen, 0, 0, W, H, hdcBack, 0, 0, SRCCOPY);
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID) { InterlockedExchange(&g_running, 0); break; }
            TranslateMessage(&msg); DispatchMessage(&msg);
        }
        Sleep(1);
    }
    for (auto &ic : icons) DestroyIcon(ic.icon);
    for (auto h : preload) { }
    SelectObject(hdcCap, oldCap);
    SelectObject(hdcBack, oldBack);
    DeleteObject(hbCap); DeleteObject(hbBack);
    DeleteDC(hdcCap); DeleteDC(hdcBack); DeleteDC(hdcScreen);
    return 0;

}

