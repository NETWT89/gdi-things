#include <windows.h>
#include <stdlib.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define PAYLOAD3_DURATION 4000

void sigmaalphaeffect(HDC dc, int elapsed) {
    float progress = (float)elapsed / PHASE3_DURATION;

    int move_x = (int)(-200 * progress);
    int move_y = (int)(-100 * progress);

    int shake_x = (rand() % 7) - 3;
    int shake_y = (rand() % 7) - 3;

    int total_x = move_x + shake_x;
    int total_y = move_y + shake_y;

    HDC mem_dc = CreateCompatibleDC(dc);
    HBITMAP mem_bmp = CreateCompatibleBitmap(dc, SCREEN_W, SCREEN_H);
    HGDIOBJ old_bmp = SelectObject(mem_dc, mem_bmp);

    BitBlt(mem_dc, 0, 0, SCREEN_W, SCREEN_H, dc, 0, 0, 0x00CC0020);

    BitBlt(dc, total_x, total_y, SCREEN_W, SCREEN_H, mem_dc, 0, 0, 0x00CC0020);

    SelectObject(mem_dc, old_bmp);
    DeleteObject(mem_bmp);
    DeleteDC(mem_dc);
}
