use std::ptr::null_mut;
use winapi::shared::minwindef::{BOOL, HBITMAP, HDC, INT};
use winapi::um::wingdi::{
    BitBlt, CreateCompatibleBitmap, CreateCompatibleDC, DeleteDC, DeleteObject, SelectObject,
};
use winapi::um::winuser::GetDC;
use rand::Rng;

const SCREEN_W: i32 = 800;
const SCREEN_H: i32 = 600;
const PAYLOAD3_DURATION: f32 = 4000.0;

fn sigmaalphaeffect(dc: HDC, elapsed: u32) {
    let progress = elapsed as f32 / PHASE3_DURATION;

    let move_x = (-200.0 * progress) as i32;
    let move_y = (-100.0 * progress) as i32;

    let mut rng = rand::thread_rng();
    let shake_x = rng.gen_range(-3..=3);
    let shake_y = rng.gen_range(-3..=3);

    let total_x = move_x + shake_x;
    let total_y = move_y + shake_y;

    unsafe {
        let mem_dc: HDC = CreateCompatibleDC(dc);
        let mem_bmp: HBITMAP = CreateCompatibleBitmap(dc, SCREEN_W, SCREEN_H);
        let old_bmp = SelectObject(mem_dc, mem_bmp as _);

        BitBlt(
            mem_dc,
            0,
            0,
            SCREEN_W,
            SCREEN_H,
            dc,
            0,
            0,
            0x00CC0020,
        );

        BitBlt(
            dc,
            total_x,
            total_y,
            SCREEN_W,
            SCREEN_H,
            mem_dc,
            0,
            0,
            0x00CC0020,
        );

        SelectObject(mem_dc, old_bmp);
        DeleteObject(mem_bmp as _);
        DeleteDC(mem_dc);
    }
}
