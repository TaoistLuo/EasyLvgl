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
HANDLE_BOX = 12
TOL = 2


def load_ppm(path):
    out = subprocess.run(["convert", path, "-depth", "8", "ppm:-"],
                         capture_output=True, check=True).stdout
    if not out.startswith(b"P6"):
        raise SystemExit("expected binary PPM")
    nl1 = out.index(b"\n")
    nl2 = out.index(b"\n", nl1 + 1)
    nl3 = out.index(b"\n", nl2 + 1)
    w, h = map(int, out[nl1 + 1:nl2].split())
    data = out[nl3 + 1:]
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


def handle_centers(w, h, data, region):
    """直接找 LVGL 缩放手柄：8x8 白底 + 2px 蓝边，中心是 4x4 白核。

    不能靠"选中框外接矩形"推角点：组件自身的蓝色背景/阴影会把外接框撑大
    （实测按钮 160x50 却得到 175x63），推出来偏差 3~4px，而手柄判定区只有
    8x8，一偏就落到组件主体上变成拖动。
    """
    rx, ry, rw, rh = region

    def pixel(x, y):
        i = (y * w + x) * 3
        return data[i], data[i + 1], data[i + 2]

    def is_blue(c):
        r, g, b = c
        return abs(r - 0x21) <= 30 and abs(g - 0x96) <= 35 and abs(b - 0xF3) <= 30

    def is_white(c):
        return c[0] > 235 and c[1] > 235 and c[2] > 235

    def blueness(cx, cy, r1, r2):
        n = 0
        for d in range(r1, r2 + 1):
            for e in range(-r2, r2 + 1):
                if is_blue(pixel(cx + e, cy + d)): n += 1
                if is_blue(pixel(cx + d, cy + e)): n += 1
        return n

    cand = []
    for y in range(max(8, ry), min(h - 8, ry + rh)):
        for x in range(max(8, rx), min(w - 8, rx + rw)):
            if not is_white(pixel(x, y)):
                continue
            # 手柄是薄蓝边：3~4px 有蓝、6~7px 出界无蓝；蓝底白字两者都命中故被排除
            if blueness(x, y, 3, 4) >= 10 and blueness(x, y, 6, 7) <= 4:
                cand.append((x, y))

    centers = []
    used = [False] * len(cand)
    for i, (x, y) in enumerate(cand):
        if used[i]:
            continue
        used[i] = True
        grp = [(x, y)]
        qi = 0
        while qi < len(grp):
            gx, gy = grp[qi]; qi += 1
            for j, (x2, y2) in enumerate(cand):
                if not used[j] and abs(x2 - gx) <= 3 and abs(y2 - gy) <= 3:
                    used[j] = True
                    grp.append((x2, y2))
        centers.append((sum(p[0] for p in grp) // len(grp),
                        sum(p[1] for p in grp) // len(grp)))
    return centers


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

    # 拖手柄必须用这里检出的坐标：手柄判定区只有 8x8，靠估算会打偏变成拖动
    reg = (canvas[0] - 12, canvas[1] - 12, canvas[2] + 24, canvas[3] + 24) if canvas else (0, 0, w, h)
    hc = handle_centers(w, h, data, reg)
    # 输出方框左上角而非中心：点击左上角那个像素必然落在方框内
    half = HANDLE_BOX // 2
    if hc:
        hc.sort(key=lambda p: (p[1], p[0]))
        for i, (hx, hy) in enumerate(hc[:8]):
            print("handle%d=%d,%d" % (i, hx - half, hy - half))
        br = max(hc, key=lambda p: p[0] + p[1])
        print("handle_br=%d,%d" % (br[0] - half, br[1] - half))
    else:
        print("handle_br=0,0")

    print("image=%dx%d" % (w, h))


if __name__ == "__main__":
    main()
