"""设计器覆盖矩阵：逐组件、逐属性验证"修改是否生效"。

对每个 (组件, 属性)：
  生成两份 .lui（只差该属性取值），交给 designer_matrix 用真实 LvglRenderer 渲染，
  优先用 styleValue 回读比较（样式属性），否则比较帧缓冲哈希（可见变化）。
用法：python scripts/designer/matrix_all.py [--json out.json]
"""
import json
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.abspath(os.getcwd())
HARNESS = os.path.join(ROOT, "tools", "lvgl-designer", "build-win", "designer_matrix.exe")
REG = os.path.join(ROOT, "tools", "luic", "component_registry.hpp")

CONTAINERS = {
    "LVBoxLayout", "LHBoxLayout", "LGridLayout", "LStackedLayout", "LTabView",
    "LMenu", "LMenuPage", "LMenuCont", "LMenuSection", "LTileView", "LTile",
    "LList", "LWin", "LGroupBox",
}
FLEX_CONTAINERS = {"LVBoxLayout", "LHBoxLayout", "LGridLayout", "LStackedLayout", "LGroupBox"}
# 这些类型只能作为父容器的子节点，顶层创建不是合法用法（luic 已校验拒绝）
SUBELEMENTS = {"LTile", "LMenuPage", "LMenuCont", "LMenuSection"}
STYLE_TARGETS = ["LLabel", "LButton"]
IMAGE = os.path.join(ROOT, "examples", "embedded_image", "assets", "caret_down.png").replace("\\", "/")

# 被测组件需要有可见内容，否则属性变化无法体现在画面上
BASE_PROPS = {
    "LLabel": {"text": "This is a very long label text that needs wrapping"},
    "LButton": {"text": "Button"},
    "LCheckbox": {"text": "Check"},
    "LRadioButton": {"text": "Radio"},
    "LSpan": {"spans": "span one\nspan two"},
    "LBar": {"range": [0, 100], "value": 80, "start_value": 10, "mode": 2},
    "LSlider": {"range": [0, 100], "value": 80, "left_value": 10, "mode": 2},
    "LArc": {"range": [0, 100], "value": 50},
    "LLine": {"points": "0,0;60,30;120,0", "width": 4, "dash_gap": 4},
    "LTable": {"data": "A|B\nC|D", "row_count": 2, "column_count": 2},
    "LChart": {"series": "1,2,3"},
    "LRoller": {"options": "A\nB\nC", "selected": 0},
    "LComboBox": {"options": "A\nB\nC", "selected": 0},
    "LButtonMatrix": {"map": "A B\nC D", "one_checked": True},
    "LSpinbox": {"range": [0, 100], "value": 5},
    "LImage": {"src": "#caret"},
    "LImageButton": {"src_released": "#caret", "src_pressed": "#caret", "src_disabled": "#caret"},
    "LScale": {"mode": 16, "range": [0, 100], "total_tick_count": 11,
               "major_tick_every": 5, "size": [200, 200]},
    "LKeyboard": {"textarea": "ta"},
    "LLineEdit": {"text": "ABCDEF"},
    "LTextEdit": {"text": "ABCDEF"},
    "LArclabel": {"text": "Arced Text"},
    "LBarcode": {"data": "1234567890"},
    "LQrcode": {"data": "https://lvgl.io"},
}
NEEDS_IMAGE = {"LImage", "LImageButton"}


def parse_registry():
    src = open(REG, encoding="utf-8").read()
    comps = {}
    for m in re.finditer(r'reg\("([A-Za-z0-9_]+)",\s*\{(.*?)\n\s*\}\);', src, re.S):
        name, body = m.group(1), m.group(2)
        props = []
        for pm in re.finditer(r'\{"([a-z0-9_]+)",\s*"([a-z_]+)",\s*"([^"]*)"', body):
            props.append((pm.group(1), pm.group(2)))
        comps[name] = props
    sm = re.search(r'style_props\(\)\s*\{.*?std::map<std::string, std::string> m = \{(.*?)\n\s*\};',
                   src, re.S)
    styles = [(m.group(1), m.group(2)) for m in
              re.finditer(r'\{"([a-z0-9_]+)",\s*"([a-z_]+)"\}', sm.group(1))]
    return comps, styles


STRING_VALUES = {
    "text": ("AA", "BB"),
    "title": ("AA", "BB"),
    "placeholder": ("AA", "BB"),
    "label": ("AA", "BB"),
    "options": ("A\nB", "A\nB\nC"),
    "items": ("A B", "A B C"),
    "tabs": ("A\nB", "A\nB\nC"),
    "map": ("A B\nC D", "A B C\nD E F"),
    "points": ("0,0;40,20", "0,0;80,10"),
    "series": ("1,2,3", "3,1,2"),
    "data": ("A|B\nC|D", "A|B|C\nD|E|F"),
    "column_widths": ("20 30", "30 40 50"),
    "digit_format": ("%02d", "%03d"),
    "accepted_chars": ("0123456789", "0123456789ABCDEF"),
}


def value(kind, prop, which):
    if kind == "bool":
        return which == 1
    if kind == "hex":
        return "0xFF0000" if which == 0 else "0x00FF00"
    if prop == "theme":
        return "primary" if which == 0 else "danger"
    if kind == "int":
        pairs = {
            "font_size": (14, 28), "text_font_size": (14, 28),
            "brightness": (20, 220), "opacity": (20, 220), "radius": (0, 20),
            "left_value": (10, 40), "start_value": (10, 40),
            "row_count": (2, 3), "column_count": (2, 3),
            "rotation": (0, 90), "angle_range": (360, 180),
            "mode": (1, 2), "selected": (0, 1),
            "scale": (100, 300), "scale_x": (100, 250), "scale_y": (100, 250),
            "step": (1, 5), "knob_offset": (0, 40),
        }
        a, b = pairs.get(prop, (1, 2))
        return a if which == 0 else b
    if kind == "int_pair":
        if prop == "digit_format":
            return [3, 0] if which == 0 else [4, 1]
        if prop == "range":
            return [0, 100] if which == 0 else [20, 60]
        if prop == "angles":
            return [0, 90] if which == 0 else [30, 120]
        if prop == "pivot":
            return [0, 0] if which == 0 else [50, 50]
        return [10, 10] if which == 0 else [20, 15]
    if kind == "int_triple":
        return [2024, 1, 2] if which == 0 else [2025, 3, 4]
    if kind == "row":
        return 0 if which == 0 else 1
    a, b = STRING_VALUES.get(prop, ("aa", "bb"))
    return a if which == 0 else b


def make_doc(ctype, extra=None):
    props = {"pos": [10, 10], "size": [160, 60]}
    props.update(BASE_PROPS.get(ctype, {}))
    node = {"type": ctype, "id": "w", "props": props}
    if ctype in CONTAINERS:
        kids = [{"type": "LLabel", "id": "c1", "props": {"text": "one", "pos": [2, 2], "size": [60, 20]}},
                {"type": "LLabel", "id": "c2", "props": {"text": "two-two", "pos": [2, 30], "size": [100, 20]}}]
        node["children"] = kids if ctype in FLEX_CONTAINERS else kids[:1]
    if extra:
        node["props"].update(extra)
    roots = [node]
    if ctype == "LKeyboard":
        roots.insert(0, {"type": "LLineEdit", "id": "ta",
                         "props": {"pos": [2, 2], "size": [100, 30]}})
    doc = {"version": "1.0", "name": "M" + ctype, "size": [800, 480], "children": roots}
    if ctype in NEEDS_IMAGE:
        doc["resources"] = {"caret": IMAGE}
    return doc


def run(doc, prop, tmp):
    path = os.path.join(tmp, "case.lui")
    with open(path, "w", encoding="utf-8") as f:
        json.dump(doc, f, ensure_ascii=False)
    try:
        r = subprocess.run([HARNESS, path, "w", prop], capture_output=True, text=True,
                           encoding="utf-8", errors="replace", timeout=60)
    except subprocess.TimeoutExpired:
        return {"crash": "timeout"}
    if r.returncode != 0:
        tail = (r.stderr or "").strip().splitlines()[-1:]
        return {"crash": "rc=%d %s" % (r.returncode, tail)}
    out = {"style": None, "hash": None, "found": 0}
    for line in r.stdout.splitlines():
        if line.startswith("found "):
            out["found"] = int(line.split()[1])
        elif line.startswith("style "):
            v = line[6:].strip()
            out["style"] = None if v == "<none>" else v
        elif line.startswith("hash "):
            out["hash"] = line.split()[1]
    return out


def main():
    if not os.path.exists(HARNESS):
        print("harness missing:", HARNESS)
        return 2
    comps, styles = parse_registry()
    report = {"components": len(comps), "style_props": len(styles),
              "uncreatable": [], "prop_results": {}, "style_results": {}, "unstable": []}
    tmp = tempfile.mkdtemp(prefix="designer_matrix_")
    total = passed = noeffect = crashed = 0

    for ctype, props in sorted(comps.items()):
        if ctype in SUBELEMENTS:
            continue
        base = make_doc(ctype)
        b1 = run(base, "", tmp)
        b2 = run(base, "", tmp)
        if b1.get("crash") or b2.get("crash"):
            report["uncreatable"].append([ctype, b1.get("crash") or b2.get("crash")])
            continue
        if not b1["found"]:
            report["uncreatable"].append([ctype, "renderer returned no object"])
            continue
        stable = b1["hash"] == b2["hash"]
        if not stable:
            report["unstable"].append(ctype)
        for prop, kind in props:
            total += 1
            d0 = make_doc(ctype, {prop: value(kind, prop, 0)})
            d1 = make_doc(ctype, {prop: value(kind, prop, 1)})
            a, b = run(d0, prop, tmp), run(d1, prop, tmp)
            if a.get("crash") or b.get("crash"):
                crashed += 1
                report["prop_results"].setdefault(ctype, []).append(
                    [prop, "CRASH", a.get("crash") or b.get("crash")])
                continue
            if a["style"] and b["style"] and a["style"] != b["style"]:
                passed += 1
                continue
            if stable and a["hash"] != b["hash"]:
                passed += 1
                continue
            noeffect += 1
            why = "readback equal" if a["style"] else "pixels equal"
            report["prop_results"].setdefault(ctype, []).append([prop, "NO-EFFECT", why])

    for target in STYLE_TARGETS:
        for prop, kind in styles:
            total += 1
            d0 = make_doc(target, {prop: value(kind, prop, 0)})
            d1 = make_doc(target, {prop: value(kind, prop, 1)})
            a, b = run(d0, prop, tmp), run(d1, prop, tmp)
            if a.get("crash") or b.get("crash"):
                crashed += 1
                report["style_results"].setdefault(target, []).append([prop, "CRASH"])
                continue
            if a["style"] and b["style"]:
                if a["style"] != b["style"]:
                    passed += 1
                else:
                    noeffect += 1
                    report["style_results"].setdefault(target, []).append(
                        [prop, "NO-EFFECT", "readback equal"])
            elif a["hash"] != b["hash"]:
                passed += 1
            else:
                noeffect += 1
                report["style_results"].setdefault(target, []).append(
                    [prop, "NO-EFFECT", "no readback, pixels equal"])

    report.update({"cases": total, "pass": passed, "no_effect": noeffect, "crash": crashed})
    out = os.path.join(tempfile.gettempdir(), "designer_matrix_report.json")
    if "--json" in sys.argv:
        out = sys.argv[sys.argv.index("--json") + 1]
    with open(out, "w", encoding="utf-8") as f:
        json.dump(report, f, ensure_ascii=False, indent=1)
    print("cases=%d pass=%d no_effect=%d crash=%d uncreatable=%d unstable=%d" % (
        total, passed, noeffect, crashed, len(report["uncreatable"]), len(report["unstable"])))
    print("report:", out)
    return 0 if noeffect == 0 and crashed == 0 and not report["uncreatable"] else 1


if __name__ == "__main__":
    sys.exit(main())
