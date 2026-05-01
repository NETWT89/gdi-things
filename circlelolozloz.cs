//do not skid, if you use, then please give me the credit!!!
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;
public class circleig
{ 
    [DllImport("user32.dll")]
    static extern IntPtr GetDC(IntPtr hWnd);
    [DllImport("user32.dll")]
    static extern int GetSystemMetrics(int nIndex);
    [DllImport("gdi32.dll")]
    static extern bool Ellipse(IntPtr hdc, int l, int t, int r, int b);
    [DllImport("gdi32.dll")]
    static extern IntPtr CreateSolidBrush(uint crColor);
    [DllImport("gdi32.dll")]
    static extern IntPtr SelectObject(IntPtr hdc, IntPtr obj);
    [DllImport("gdi32.dll")]
    static extern bool DeleteObject(IntPtr obj);

    const int SM_CXSCREEN = 0;
    const int SM_CYSCREEN = 1;

    static uint RGB(byte r, byte g, byte b) => (uint)(r | (g << 8) | (b << 16));
    static Random rng = new Random();

    public static void Main()
    {
        int sw = GetSystemMetrics(SM_CXSCREEN);
        int sh = GetSystemMetrics(SM_CYSCREEN);

        float x = sw / 2f, y = sh / 2f;
        float vx = 50, vy = 45;//just changes speed of circle
        int radius = 60;//or whatever you want idk
        uint color = RGB((byte)rng.Next(256), (byte)rng.Next(256), (byte)rng.Next(256));
        List<float> trailX = new List<float>();
        List<float> trailY = new List<float>();
        List<uint> trailColors = new List<uint>();

        IntPtr hdc = GetDC(IntPtr.Zero);

        while (true)
        {
            x += vx;
            y += vy;

            if (x - radius <= 0) { x = radius; vx = -vx; color = RGB((byte)rng.Next(256), (byte)rng.Next(256), (byte)rng.Next(256)); }
            if (x + radius >= sw) { x = sw - radius; vx = -vx; color = RGB((byte)rng.Next(256), (byte)rng.Next(256), (byte)rng.Next(256)); }
            if (y - radius <= 0) { y = radius; vy = -vy; color = RGB((byte)rng.Next(256), (byte)rng.Next(256), (byte)rng.Next(256)); }
            if (y + radius >= sh) { y = sh - radius; vy = -vy; color = RGB((byte)rng.Next(256), (byte)rng.Next(256), (byte)rng.Next(256)); }

            trailX.Add(x);
            trailY.Add(y);
            trailColors.Add(color);
            if (trailX.Count > 25) { trailX.RemoveAt(0); trailY.RemoveAt(0); trailColors.RemoveAt(0); }

            for (int i = 0; i < trailX.Count; i++)
            {
                float Lolz = (float)i / trailX.Count;
                IntPtr hBr = CreateSolidBrush(trailColors[i]);
                IntPtr oldBr = SelectObject(hdc, hBr);
                Ellipse(hdc, (int)(trailX[i] - radius * Lolz), (int)(trailY[i] - radius * Lolz), (int)(trailX[i] + radius * Lolz), (int)(trailY[i] + radius * Lolz));
                SelectObject(hdc, oldBr);
                DeleteObject(hBr);
            }

            IntPtr hCircBr = CreateSolidBrush(color);
            IntPtr oldCircBr = SelectObject(hdc, hCircBr);
            Ellipse(hdc, (int)(x - radius), (int)(y - radius), (int)(x + radius), (int)(y + radius));
            SelectObject(hdc, oldCircBr);
            DeleteObject(hCircBr);

            Thread.Sleep(1);
        }
    }
}
