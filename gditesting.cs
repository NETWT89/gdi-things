using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

class simple
{
    [DllImport("user32.dll")] static extern IntPtr GetDC(IntPtr hWnd);
    [DllImport("user32.dll")] static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);
    [DllImport("gdi32.dll")] static extern IntPtr CreateSolidBrush(uint color);
    [DllImport("gdi32.dll")] static extern IntPtr SelectObject(IntPtr hdc, IntPtr obj);
    [DllImport("gdi32.dll")] static extern bool DeleteObject(IntPtr obj);
    [DllImport("gdi32.dll")] static extern bool PatBlt(IntPtr hdc, int x, int y, int w, int h, int rop);
    const int PATINVERT = 0x005A0049;
    static uint RGB(byte r, byte g, byte b) => (uint)(r | (g << 8) | (b << 16));
    static void Main()
    {
        int w = Screen.PrimaryScreen.Bounds.Width;
        int h = Screen.PrimaryScreen.Bounds.Height;
        IntPtr hdc = GetDC(IntPtr.Zero);
        Random rnd = new Random();
        double t = 0;
        while (true)
        {
            byte r = (byte)(128 + Math.Sin(t * 15) * 127);
            byte g = (byte)(128 + Math.Sin(t * 17 + 2) * 127);
            byte b = (byte)(128 + Math.Cos(t * 19 + 5) * 127);
            IntPtr brush = CreateSolidBrush(RGB(r, g, b));
            IntPtr old = SelectObject(hdc, brush);
            PatBlt(hdc, 0, 0, w, h, PATINVERT);
            SelectObject(hdc, old);
            DeleteObject(brush);
            int slideX = (int)(t * 20) % w; 
            int shakeX = rnd.Next(-8, 8);   
            int shakeY = rnd.Next(-6, 6);   
            BitBlt(hdc, shakeX - slideX, shakeY, w, h, hdc, 0, 0, 0x00CC0020); // SRCCOPY
            t += 0.05; 
        }
    }
    [DllImport("gdi32.dll")]
    public static extern bool BitBlt(IntPtr hdcDest, int nXDest, int nYDest,
        int nWidth, int nHeight, IntPtr hdcSrc, int nXSrc, int nYSrc, int dwRop);
}