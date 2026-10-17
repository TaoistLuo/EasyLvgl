import json, os, re, subprocess, sys
ROOT=os.getcwd(); LUIC=os.path.join(ROOT,'bin/luic'); OUT='/tmp/matrix_out'
os.makedirs(OUT, exist_ok=True)
defaults={}
for line in open('/tmp/schema_defaults.json'):
    line=line.strip().rstrip(',')
    if line: defaults.update(json.loads('{'+line+'}'))
CONTAINERS={"LVBoxLayout","LHBoxLayout","LGridLayout","LStackedLayout","LTabView","LMenu",
            "LMenuPage","LMenuCont","LMenuSection","LTileView","LTile","LList","LWin","LCanvas","LGroupBox"}
ok=[]; bad=[]
for typ, props in defaults.items():
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
    c=subprocess.run(["g++","-std=c++17","-fsyntax-only","-I"+ROOT,"-I"+os.path.join(ROOT,"lvgl"),
                      "-I"+os.path.join(ROOT,"lvgl++"),"-I"+os.path.join(ROOT,"build/lvgl"),cu],
                     capture_output=True,text=True)
    if c.returncode!=0:
        errs=[l for l in c.stderr.splitlines() if 'error:' in l][:3]
        bad.append((typ,"compile"," | ".join(e.split('error:')[-1].strip()[:90] for e in errs))); continue
    ok.append(typ)
print("PASS %d / %d"%(len(ok), len(defaults)))
for t,stage,msg in bad:
    print("  FAIL %-16s [%s] %s"%(t,stage,msg))
