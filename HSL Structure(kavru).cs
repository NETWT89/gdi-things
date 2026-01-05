using System;
using System.Drawing;

public struct HSL
{
    public double H; 
    public double S; 
    public double L; 

    public HSL(double h, double s, double l)
    {
        H = h;
        S = s;
        L = l;
    }

    
    public static HSL FromColor(Color color)
    {
        double r = color.R / 255.0;
        double g = color.G / 255.0;
        double b = color.B / 255.0;

        double max = Math.Max(r, Math.Max(g, b));
        double min = Math.Min(r, Math.Min(g, b));
        double h = 0, s = 0, l = (max + min) / 2.0;

        if (max != min)
        {
            double d = max - min;
            s = l > 0.5 ? d / (2 - max - min) : d / (max + min);

            if (max == r)
                h = (g - b) / d + (g < b ? 6 : 0);
            else if (max == g)
                h = (b - r) / d + 2;
            else
                h = (r - g) / d + 4;

            h *= 60;
        }

        return new HSL(h, s, l);
    }

    
    public Color ToColor()
    {
        double r, g, b;

        if (S == 0)
        {
            r = g = b = L; 
        }
        else
        {
            double q = L < 0.5 ? L * (1 + S) : L + S - L * S;
            double p = 2 * L - q;
            r = HueToRgb(p, q, H / 360 + 1.0 / 3.0);
            g = HueToRgb(p, q, H / 360);
            b = HueToRgb(p, q, H / 360 - 1.0 / 3.0);
        }

        return Color.FromArgb(
            Clamp((int)(r * 255)),
            Clamp((int)(g * 255)),
            Clamp((int)(b * 255))
        );
    }

    private static double HueToRgb(double p, double q, double t)
    {
        if (t < 0) t += 1;
        if (t > 1) t -= 1;
        if (t < 1.0 / 6.0) return p + (q - p) * 6 * t;
        if (t < 1.0 / 2.0) return q;
        if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6;
        return p;
    }

    private static int Clamp(int v)
    {
        return v < 0 ? 0 : (v > 255 ? 255 : v);
    }

    public override string ToString()
    {
        return $"H: {H:0.0}, S: {S:0.00}, L: {L:0.00}";
    }
}
