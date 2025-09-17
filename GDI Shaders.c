#include <windows.h>    // For HBITMAP, BITMAP, BITMAPINFO, SetDIBits, ZeroMemory
#include <stdint.h>     // For uint32_t
#include <stdlib.h>     // For malloc, free
#include <string.h>     // For memcpy
#include <math.h>       // For abs (though you could also use stdlib.h's abs)

// ---------- epik shaders ----------
static void apply_edge_shader_inplace(uint32_t *buf, int w, int h, int threshold, float alpha, unsigned int seed) {
    // does copy instead
    uint32_t *copy = (uint32_t*)malloc(w * h * sizeof(uint32_t));
    if (!copy) return;
    memcpy(copy, buf, w * h * sizeof(uint32_t));

    for (int y = 1; y < h - 1; ++y) {
        for (int x = 1; x < w - 1; ++x) {
            uint32_t c = copy[y * w + x];
            uint32_t rpx = copy[y * w + (x + 1)];
            uint32_t pny = copy[(y + 1) * w + x];
            int cr = (c >> 16) & 0xFF;
            int cg = (c >> 8) & 0xFF;
            int cb = (c) & 0xFF;
            int xr = (rpx >> 16) & 0xFF;
            int xg = (rpx >> 8) & 0xFF;
            int xb = (rpx) & 0xFF;
            int yr = (pny >> 16) & 0xFF;
            int yg = (pny >> 8) & 0xFF;
            int yb = (pny) & 0xFF;
            int contrast = abs(cr - xr) + abs(cg - xg) + abs(cb - xb)
                         + abs(cr - yr) + abs(cg - yg) + abs(cb - yb);
            if (contrast > threshold) {
                // why did you download this???????
                // half wayyyy
                unsigned int rseed = (unsigned int)(seed + x * 1315423911u + y * 2654435761u);
                int rv = (rseed ^ (rseed >> 7)) & 0xFF;
                int gv = ((rseed >> 8) ^ (rseed >> 3)) & 0xFF;
                int bv = ((rseed >> 16) ^ (rseed >> 5)) & 0xFF;
                uint32_t col = (rv << 16) | (gv << 8) | bv;
                uint32_t blended = blend_pixel(c, col, alpha);
                // draw a thin outline: set current pixel and a couple neighbors lightly
                buf[y * w + x] = blended;
                // neighbor accents but faint
                if (x + 1 < w) buf[y * w + (x + 1)] = blend_pixel(buf[y * w + (x + 1)], col, alpha * 0.6f);
                if (y + 1 < h) buf[(y + 1) * w + x] = blend_pixel(buf[(y + 1) * w + x], col, alpha * 0.6f);
            }
        }
    }

    free(copy);
}
// ---------------------------------------------------------------------------

// ---------- this helps the epik shaders ----------
static void update_hbitmap_from_pixels(HBITMAP hbmp, uint32_t *pixels, int w, int h) {
    // bitmapppp✨✨✨✨✨
    BITMAP bmp;
    if (!GetObject(hbmp, sizeof(bmp), &bmp)) return;

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    SetDIBits(NULL, hbmp, 0, h, pixels, &bmi, DIB_RGB_COLORS);
}
