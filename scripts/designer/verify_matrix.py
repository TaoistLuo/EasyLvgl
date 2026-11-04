import json, os, re, subprocess, sys
ROOT=os.getcwd(); LUIC=os.path.join(ROOT,'bin/luic'); OUT='/tmp/matrix_out'
os.makedirs(OUT, exist_ok=True)
SKIP={"LLottie"}   # LV_USE_LOTTIE=0 时该组件不存在
lv_conf_flags=["-DLV_CONF_SKIP"]
_autoconf=os.path.join(ROOT,'build/lvgl/autoconf.h')
if os.path.exists(_autoconf):
    lv_conf_flags.append('-DLV_CONF_KCONFIG_EXTERNAL_INCLUDE="%s"'%_autoconf)
def _load_defaults():
    path='/tmp/schema_defaults.json'
    if os.path.exists(path):
        out={}
        for line in open(path):
            line=line.strip().rstrip(',')
            if line: out.update(json.loads('{'+line+'}'))
        return out
    src=open(os.path.join(ROOT,'tools/luic/component_registry.hpp')).read()
    return {t:({"col":0,"row":0,"dir":0} if t=="LTile" else {})
            for t in re.findall(r'reg\("([A-Za-z0-9_]+)"', src)}
defaults=_load_defaults()
CONTAINERS={"LVBoxLayout","LHBoxLayout","LGridLayout","LStackedLayout","LTabView","LMenu",
            "LMenuPage","LMenuCont","LMenuSection","LTileView","LTile","LList","LWin","LGroupBox"}
ok=[]; bad=[]
for typ, props in defaults.items():
    if typ in SKIP: continue
    node={"id":"w","type":typ,"props":dict(props)}
    node["props"]["pos"]=[10,10]; node["props"]["size"]=[120,60]
    if typ in CONTAINERS:
        node["children"]=[{"id":"c1","type":"LLabel","props":{"text":"child","pos":[2,2],"size":[80,24]}}]
    doc={"version":"1.0","name":"M"+typ,"size":[400,300],"children":[node]}
    lui=os.path.join(OUT,"%s.lui"%typ); json.dump(doc, open(lui,'w'), indent=1)
    hpp=os.path.join(OUT,"%s_ui.hpp"%typ)
    r=subprocess.run([LUIC,lui,'-o',hpp],capture_output=True,text=True)
    if r.returncode!=0:
        bad.append((typ,"luic",(r.stdout+r.stderr).strip().splitlines()[-1][:120])); continue
    cu=os.path.join(OUT,"u_%s.cpp"%typ)
    open(cu,'w').write('#include "%s"\nint main(){return 0;}\n'%hpp)
    c=subprocess.run(["g++","-std=c++17","-fsyntax-only"]+lv_conf_flags+["-I"+ROOT,"-I"+os.path.join(ROOT,"lvgl"),
                      "-I"+os.path.join(ROOT,"easylvgl"),"-I"+os.path.join(ROOT,"build/lvgl"),cu],
                     capture_output=True,text=True)
    if c.returncode!=0:
        errs=[l for l in c.stderr.splitlines() if 'error:' in l][:3]
        bad.append((typ,"compile"," | ".join(e.split('error:')[-1].strip()[:90] for e in errs))); continue
    ok.append(typ)
print("PASS %d / %d"%(len(ok), len(defaults)))
for t,stage,msg in bad:
    print("  FAIL %-16s [%s] %s"%(t,stage,msg))
