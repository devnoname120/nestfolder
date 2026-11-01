#!/usr/bin/env python3
"""Kurulum uygulamasinin ekran fontunu uretir (src/font.h).
DejaVu Sans Mono'dan 12x24 piksel, 8 bit alfa glif tablosu.
Kullanim: python3 make_font.py DejaVuSansMono.ttf ../src/font.h"""
import sys
from PIL import Image, ImageDraw, ImageFont

W, H, SIZE = 12, 24, 19
chars = [chr(c) for c in range(32, 127)] + list("ÇĞİÖŞÜçğıöşüÂÎÛâîû·•…→↑↓✓✗")
font = ImageFont.truetype(sys.argv[1], SIZE)
asc, desc = font.getmetrics()
top = (H - (asc + desc)) // 2
out = ["/* make_font.py ile uretildi: DejaVu Sans Mono, %dx%d, 8 bit alfa */" % (W, H),
       "#define FONT_W %d" % W, "#define FONT_H %d" % H,
       "#define FONT_COUNT %d" % len(chars),
       "static const unsigned int font_cp[FONT_COUNT] = {" +
       ",".join(str(ord(c)) for c in chars) + "};",
       "static const unsigned char font_px[FONT_COUNT][FONT_W * FONT_H] = {"]
for c in chars:
    im = Image.new("L", (W, H), 0)
    d = ImageDraw.Draw(im)
    adv = font.getlength(c)
    d.text(((W - adv) / 2, top), c, font=font, fill=255)
    out.append("{" + ",".join(str(p) for p in im.tobytes()) + "},")
out.append("};")
open(sys.argv[2], "w").write("\n".join(out) + "\n")
print(len(chars), "glif")
