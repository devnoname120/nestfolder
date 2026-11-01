#!/usr/bin/env python3
"""Kurulum uygulamasinin LiveArea resimlerini uretir (8 bit paletli PNG).
Kullanim: python3 make_livearea.py ../sce_sys"""
import sys, os
from PIL import Image, ImageDraw, ImageFont

out = sys.argv[1]
os.makedirs(os.path.join(out, "livearea/contents"), exist_ok=True)
BG, BAR, FOLD, FOLD2, WHITE = (24, 30, 42), (44, 74, 107), (242, 184, 79), (214, 140, 52), (240, 240, 240)
FONT = "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"

def folder(d, x, y, w, h, c, tab):
    d.rounded_rectangle([x, y, x + w * 0.45, y + h * 0.3], radius=h * 0.08, fill=tab)
    d.rounded_rectangle([x, y + h * 0.15, x + w, y + h], radius=h * 0.1, fill=c)

def icon(size):
    """Vita balonu yuvarlak: klasor, cember icinde kenarlardan bosluk birakir."""
    s = 4
    im = Image.new("RGB", (size * s, size * s), BG)
    d = ImageDraw.Draw(im)
    W = size * s
    # dis klasor: genislik %56, yukseklik %46, merkezde
    folder(d, W * 0.22, W * 0.27, W * 0.56, W * 0.46, FOLD, FOLD2)
    # ic klasor ve ok
    folder(d, W * 0.40, W * 0.46, W * 0.30, W * 0.24, BAR, (30, 52, 78))
    d.polygon([(W * 0.515, W * 0.625), (W * 0.585, W * 0.625), (W * 0.55, W * 0.665)], fill=WHITE)
    d.rectangle([W * 0.537, W * 0.53, W * 0.563, W * 0.625], fill=WHITE)
    return im.resize((size, size), Image.LANCZOS)

def save(im, path):
    im.convert("P", palette=Image.ADAPTIVE, colors=256).save(path, optimize=True)

save(icon(128), os.path.join(out, "icon0.png"))

bg = Image.new("RGB", (840, 500), BG)
d = ImageDraw.Draw(bg)
d.rectangle([0, 0, 840, 8], fill=BAR)
bg.paste(icon(200), (60, 150))
f1, f2 = ImageFont.truetype(FONT, 44), ImageFont.truetype(FONT.replace("-Bold", ""), 24)
d.text((300, 185), "nestfolder", font=f1, fill=WHITE)
d.text((300, 245), "Folders inside folders", font=f2, fill=FOLD)
save(bg, os.path.join(out, "livearea/contents/bg.png"))

st = Image.new("RGB", (280, 158), BAR)
d = ImageDraw.Draw(st)
st.paste(icon(96), (20, 31))
d.text((128, 64), "nestfolder", font=ImageFont.truetype(FONT, 24), fill=WHITE)
save(st, os.path.join(out, "livearea/contents/startup.png"))

open(os.path.join(out, "livearea/contents/template.xml"), "w").write(
    '<?xml version="1.0" encoding="utf-8"?>\n'
    '<livearea style="a1" format-ver="01.00" content-rev="1">\n'
    '  <livearea-background>\n    <image>bg.png</image>\n  </livearea-background>\n'
    '  <gate>\n    <startup-image>startup.png</startup-image>\n  </gate>\n'
    '</livearea>\n')
print("tamam")
