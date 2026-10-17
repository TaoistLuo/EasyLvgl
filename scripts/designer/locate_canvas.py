#!/usr/bin/env python3
"""从设计器截图里定位 LVGL 画布与表格（X 窗口坐标，相对窗口左上角）。

原理：LVGL 默认主题的屏幕底色是纯 #F5F5F5，表格单元格是纯 #FFFFFF，
而设计器控件区底色是 #B8B8B8、面板是深色。按行/列统计匹配像素数，
取"每行/列匹配数 >= MINCOUNT"的极值即可得到矩形，能过滤掉面板里的小色块。

用法：
    locate_canvas.py <screenshot.png>
输出：
    canvas=x,y,w,h
    table=x,y,w,h          (未找到则 w=h=0)
    empty=x,y              画布内但不在表格内的一点（用于点击"空白画布"）
"""
import subprocess
import sys

MINCOUNT = 50
TOL = 2


def load_ppm(path):
    out = subprocess.run(["convert", path, "-depth", "8", "ppm:-"],
                         capture_output=True, check=True).stdout
    if not out.startswith(b"P6"):
        raise SystemExit("expected binary PPM")
    nl1 = out.index(b"\n")
    nl2 = out.index(b"\n", nl1 + 1)
    w, h = map(int, out[nl1 + 1:nl2].split())
    data = out[nl2 + 1:]
    return w, h, data


def bbox(w, h, data, target):
    tr, tg, tb = target
    rows = [0] * h
    cols = [0] * w
    for y in range(h):
        base = y * w * 3
        for x in range(w):
            i = base + x * 3
            if (abs(data[i] - tr) <= TOL and abs(data[i + 1] - tg) <= TOL
                    and abs(data[i + 2] - tb) <= TOL):
                rows[y] += 1
                cols[x] += 1
    ys = [y for y in range(h) if rows[y] >= MINCOUNT]
    xs = [x for x in range(w) if cols[x] >= MINCOUNT]
    if not xs or not ys:
        return None
    return min(xs), min(ys), max(xs) - min(xs) + 1, max(ys) - min(ys) + 1


def main():
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    w, h, data = load_ppm(sys.argv[1])

    canvas = bbox(w, h, data, (0xF5, 0xF5, 0xF5))
    table = bbox(w, h, data, (0xFF, 0xFF, 0xFF))

    if canvas:
        print("canvas=%d,%d,%d,%d" % canvas)
    else:
        print("canvas=0,0,0,0")
    if table:
        print("table=%d,%d,%d,%d" % table)
    else:
        print("table=0,0,0,0")

    if canvas:
        cx, cy, cw, ch = canvas
        tx, ty, tw, th = table if table else (cx, cy, 0, 0)
        # 画布右下角往内收 30px，保证不在根部手柄命中区内
        ex = cx + cw - 30
        ey = cy + ch - 30
        if table and tw > 0:
            tx2, ty2 = tx + tw, ty + th
            if tx <= ex <= tx2 and ty <= ey <= ty2:
                ex = cx + cw - 30
                ey = cy + 6
        print("empty=%d,%d" % (ex, ey))
    print("image=%dx%d" % (w, h))


if __name__ == "__main__":
    main()
