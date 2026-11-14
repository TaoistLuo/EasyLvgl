#!/usr/bin/env python3
"""Convert i18n/strings.xlsx into C++ arrays for LVGL's native lv_translation.

Usage:
    xlsx2i18n.py <strings.xlsx> <i18n_generated.hpp>

Sheet format (first row is the header):
    key | zh | en
The wrapper maps Lang::Zh/En/Ru to columns "zh"/"en"/"ru", so those three
columns are required and must be the first three language columns.
Only the Python standard library is used (xlsx is a zip of XML files).
"""

import re
import sys
import zipfile
import xml.etree.ElementTree as ET

NS = {"m": "http://schemas.openxmlformats.org/spreadsheetml/2006/main",
      "r": "http://schemas.openxmlformats.org/officeDocument/2006/relationships",
      "rel": "http://schemas.openxmlformats.org/package/2006/relationships"}
IDENT = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def first_sheet_path(zf):
    wb = ET.fromstring(zf.read("xl/workbook.xml"))
    sheet = wb.find("m:sheets/m:sheet", NS)
    if sheet is None:
        raise SystemExit("xlsx: no sheet found")
    rid = sheet.get("{%s}id" % NS["r"])
    rels = ET.fromstring(zf.read("xl/_rels/workbook.xml.rels"))
    for rel in rels.findall("rel:Relationship", NS):
        if rel.get("Id") == rid:
            target = rel.get("Target")
            if target.startswith("/"):
                return target.lstrip("/")
            return "xl/" + target
    raise SystemExit("xlsx: sheet relationship not found")


def shared_strings(zf):
    if "xl/sharedStrings.xml" not in zf.namelist():
        return []
    root = ET.fromstring(zf.read("xl/sharedStrings.xml"))
    out = []
    for si in root.findall("m:si", NS):
        out.append("".join(t.text or "" for t in si.iter("{%s}t" % NS["m"])))
    return out


def column_index(ref):
    letters = re.match(r"([A-Z]+)", ref).group(1)
    idx = 0
    for ch in letters:
        idx = idx * 26 + (ord(ch) - ord("A") + 1)
    return idx - 1


def read_rows(zf):
    strings = shared_strings(zf)
    root = ET.fromstring(zf.read(first_sheet_path(zf)))
    rows = []
    for row in root.iter("{%s}row" % NS["m"]):
        cells = {}
        for c in row.findall("m:c", NS):
            ref = c.get("r", "")
            ctype = c.get("t", "n")
            value = ""
            if ctype == "inlineStr":
                value = "".join(t.text or "" for t in c.iter("{%s}t" % NS["m"]))
            else:
                v = c.find("m:v", NS)
                if v is not None and v.text is not None:
                    value = v.text
                    if ctype == "s":
                        value = strings[int(value)]
            cells[column_index(ref)] = value
        if cells:
            width = max(cells) + 1
            rows.append([cells.get(i, "") for i in range(width)])
    return rows


def c_escape(text):
    out = []
    for ch in text:
        if ch == "\\":
            out.append("\\\\")
        elif ch == '"':
            out.append('\\"')
        elif ch == "\n":
            out.append("\\n")
        elif ch == "\r":
            out.append("\\r")
        elif ch == "\t":
            out.append("\\t")
        else:
            out.append(ch)
    return "".join(out)


def main():
    if len(sys.argv) != 3:
        raise SystemExit(__doc__)
    xlsx, output = sys.argv[1], sys.argv[2]
    with zipfile.ZipFile(xlsx) as zf:
        rows = read_rows(zf)
    if not rows:
        raise SystemExit("xlsx: empty sheet")

    header = [h.strip().lower() for h in rows[0]]
    if "key" not in header:
        raise SystemExit("xlsx: header must contain a key column")
    col_key = header.index("key")
    langs = [name for name in header if name and name not in ("key", "comment")]
    if langs[:3] != ["zh", "en", "ru"]:
        raise SystemExit("xlsx: language columns must start with zh, en, ru")
    lang_cols = [header.index(name) for name in langs]

    entries = []
    seen = set()
    for line_no, row in enumerate(rows[1:], start=2):
        if all(not str(v).strip() for v in row):
            continue
        key = row[col_key].strip() if col_key < len(row) else ""
        if not key:
            raise SystemExit("xlsx: row %d has an empty key" % line_no)
        if not IDENT.match(key):
            raise SystemExit("xlsx: row %d key '%s' is not a C identifier" % (line_no, key))
        if key in seen:
            raise SystemExit("xlsx: duplicate key '%s'" % key)
        seen.add(key)
        values = []
        for col in lang_cols:
            value = row[col].strip() if col < len(row) else ""
            if not value:
                raise SystemExit("xlsx: row %d (%s) has an empty %s translation"
                                 % (line_no, key, header[col]))
            values.append(value)
        entries.append((key, values))

    if not entries:
        raise SystemExit("xlsx: no translation rows")

    lines = []
    lines.append("// Auto-generated from i18n/strings.xlsx by tools/xlsx2i18n.py.")
    lines.append("// DO NOT EDIT: change the spreadsheet and rebuild instead.")
    lines.append("#pragma once")
    lines.append("#include <cstddef>")
    lines.append("")
    lines.append("namespace exc {")
    lines.append("")
    lines.append("constexpr const char* kLanguages[] = {%s};"
                 % (", ".join('"%s"' % name for name in langs) + ", nullptr"))
    lines.append("constexpr int kLanguageCount = %d;" % len(langs))
    lines.append("")
    lines.append("enum class K {")
    for key, _ in entries:
        lines.append("    %s," % key)
    lines.append("    Count")
    lines.append("};")
    lines.append("")
    lines.append("constexpr const char* kTags[] = {")
    for key, _ in entries:
        lines.append('    "%s",' % key)
    lines.append("    nullptr,")
    lines.append("};")
    lines.append("")
    lines.append("constexpr const char* kTranslations[] = {")
    for key, values in entries:
        for value in values:
            lines.append('    "%s",' % c_escape(value))
    lines.append("};")
    lines.append("")
    lines.append("inline const char* tag(K key) { return kTags[static_cast<int>(key)]; }")
    lines.append("")
    lines.append("static_assert(sizeof(kTags) / sizeof(kTags[0]) - 1 == "
                 "static_cast<size_t>(K::Count), \"tag count mismatch\");")
    lines.append("static_assert(sizeof(kTranslations) / sizeof(kTranslations[0]) == "
                 "static_cast<size_t>(K::Count) * kLanguageCount, \"translation count mismatch\");")
    lines.append("")
    lines.append("}  // namespace exc")
    lines.append("")

    with open(output, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines))
    print("i18n: %d strings -> %s" % (len(entries), output))


if __name__ == "__main__":
    main()
