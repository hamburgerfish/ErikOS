import ctypes
import os

_conv = ctypes.CDLL("conv.so")
_conv.rgb2vga.argtypes = (ctypes.c_int, ctypes.c_int, ctypes.c_int)

def conv(r, g, b):
    global _conv
    out = _conv.rgb2vga(ctypes.c_int(r), ctypes.c_int(g), ctypes.c_int(b))
    return out

for file in os.listdir("bitmaps_orig"):
    with open("bitmaps_orig/"+file, "r", encoding="utf-8") as f:
        script = f.read()
        pre, script = script.split("{")
        pre += "{\n\t"
        start = 0
    for i in range(script.count(",")+1):
        if i == script.count(","):
            p = start + 3
        else:
            p = script[start:].find(",") + start - 6
        start =  p + 7
        r = int(script[p:p+2], 16)
        g = int(script[p+2:p+4], 16)
        b = int(script[p+4:p+6], 16)
        vga_colour = conv(r, g, b)
        pre += str(vga_colour) + ","
        if script[start+1] == "\n":
            pre += "\n\t"
        else:
            pre += " "
    pre = pre[:-2] + "\n};"
    with open("bitmaps/"+file, "w", encoding="utf-8") as f:
        f.write(pre)