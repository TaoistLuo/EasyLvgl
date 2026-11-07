"""代码生成覆盖矩阵：逐组件、逐属性用 luic 生成并编译生成的头文件。

验证 "属性 -> 生成代码" 这条链路：luic 接受该属性，且生成的代码真的能编译。
用法：python scripts/designer/codegen_all.py
"""
import json
import os
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import matrix_all as m

ROOT = os.path.abspath(os.getcwd())
LUIC = os.path.join(ROOT, "bin", "luic.exe")
AUTOCONF = os.path.join(ROOT, "build-win", "lvgl", "autoconf.h").replace("\\", "/")
FLAGS = ["-std=c++17", "-fsyntax-only", "-DLV_CONF_SKIP",
         '-DLV_CONF_KCONFIG_EXTERNAL_INCLUDE="%s"' % AUTOCONF,
         "-I" + ROOT, "-I" + os.path.join(ROOT, "lvgl"),
         "-I" + os.path.join(ROOT, "easylvgl"),
         "-I" + os.path.join(ROOT, "examples", "embedded_image")]
REFERENCE_PROPS = {"textarea", "active_page"}


def compile_header(hpp, tmp, tag):
    cu = os.path.join(tmp, "cu_%s.cpp" % tag)
    with open(cu, "w", encoding="utf-8") as f:
        f.write('#include "%s"\nint main() { return 0; }\n' % hpp.replace("\\", "/"))
    r = subprocess.run(["g++"] + FLAGS + [cu], capture_output=True, text=True,
                       encoding="utf-8", errors="replace", timeout=180)
    if r.returncode == 0:
        return None
    for line in r.stderr.splitlines():
        if "error:" in line:
            return line.split("error:")[-1].strip()[:110]
    return "compile failed"


def main():
    comps, _ = m.parse_registry()
    tmp = tempfile.mkdtemp(prefix="codegen_matrix_")
    total = ok = 0
    luic_fail, compile_fail = [], []
    for ctype, props in sorted(comps.items()):
        if ctype in m.SUBELEMENTS:
            continue
        for prop, kind in props:
            if prop in REFERENCE_PROPS:
                continue
            total += 1
            doc = m.make_doc(ctype, {prop: m.value(kind, prop, 0)})
            lui = os.path.join(tmp, "case.lui")
            with open(lui, "w", encoding="utf-8") as f:
                json.dump(doc, f, ensure_ascii=False)
            hpp = os.path.join(tmp, "case_ui.hpp")
            r = subprocess.run([LUIC, lui, "-o", hpp], capture_output=True, text=True,
                               encoding="utf-8", errors="replace", timeout=120)
            if r.returncode != 0:
                msg = (r.stderr or r.stdout).strip().splitlines()
                luic_fail.append([ctype, prop, msg[-1][:110] if msg else "luic failed"])
                continue
            err = compile_header(hpp, tmp, "x")
            if err:
                compile_fail.append([ctype, prop, err])
                continue
            ok += 1
    print("cases=%d ok=%d luic_fail=%d compile_fail=%d" % (
        total, ok, len(luic_fail), len(compile_fail)))
    for row in luic_fail:
        print("  LUIC-FAIL %-14s %-18s %s" % tuple(row))
    for row in compile_fail:
        print("  COMPILE-FAIL %-14s %-18s %s" % tuple(row))
    return 0 if not luic_fail and not compile_fail else 1


if __name__ == "__main__":
    sys.exit(main())
