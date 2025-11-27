require 'fiddle'
require 'fiddle/import'

module WinAPI
  extend Fiddle::Importer
  dlload 'gdi32.dll', 'user32.dll'

  typealias 'HDC', 'void*'
  typealias 'HBITMAP', 'void*'

  extern 'void* CreateCompatibleDC(void*)'
  extern 'void* CreateCompatibleBitmap(void*, int, int)'
  extern 'void* SelectObject(void*, void*)'
  extern 'int BitBlt(void*, int, int, int, int, void*, int, int, int)'
  extern 'int DeleteObject(void*)'
  extern 'int DeleteDC(void*)'
end

SCREEN_W = 800
SCREEN_H = 600
PHASE3_DURATION = 4000

def sigmaalphaeffect(dc, elapsed)
  progress = elapsed.to_f / PHASE3_DURATION

  move_x  = (-200 * progress).to_i
  move_y  = (-100 * progress).to_i

  shake_x = rand(7) - 3
  shake_y = rand(7) - 3

  total_x = move_x + shake_x
  total_y = move_y + shake_y

  mem_dc  = WinAPI.CreateCompatibleDC(dc)
  mem_bmp = WinAPI.CreateCompatibleBitmap(dc, SCREEN_W, SCREEN_H)
  old_bmp = WinAPI.SelectObject(mem_dc, mem_bmp)

  WinAPI.BitBlt(mem_dc, 0, 0, SCREEN_W, SCREEN_H, dc, 0, 0, 0x00CC0020)

  WinAPI.BitBlt(dc, total_x, total_y, SCREEN_W, SCREEN_H, mem_dc, 0, 0, 0x00CC0020)

  WinAPI.SelectObject(mem_dc, old_bmp)
  WinAPI.DeleteObject(mem_bmp)
  WinAPI.DeleteDC(mem_dc)
end
