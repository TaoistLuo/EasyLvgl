#!/usr/bin/env bash
# 设计器自动化测试入口 —— 在 WSLg 下驱动 WSLg 窗口的鼠标/键盘。
#
# 背景：WSLg 里 XWayland 不转发 XTEST 合成事件（指针会动，但应用收不到），
#       所以输入改由 Windows 侧 Win32 (SetCursorPos + mouse_event/keybd_event)
#       注入，见 win/win_input.ps1。
#
# 用法（坐标一律用 X 窗口坐标，脚本负责换算到 Windows 屏幕坐标）：
#   dtest.sh env                     # 定位窗口并标定缩放，查看参数
#   dtest.sh locate                  # 从截图定位画布/表格矩形
#   dtest.sh shot [out.png]          # 截整窗
#   dtest.sh status [tag]            # 截状态栏，打印文件与 md5（用于比对变化）
#   dtest.sh click X Y [btn]         # 点击（btn: 0左 1右 2中）
#   dtest.sh drag X1 Y1 X2 Y2 [btn]  # 拖拽
#   dtest.sh wheel X Y [delta]       # 滚轮（传给 LVGL 滚动）
#   dtest.sh zoom X Y [delta]        # Ctrl+滚轮（画布缩放）
#   dtest.sh key VK                  # 单键点按（VK: Esc=27, Ctrl=17, Shift=16）
#
# 例：点画布空白处
#   eval "$(dtest.sh locate)"; dtest.sh click "${empty%,*}" "${empty#*,}"
set -uo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WIN_DIR="$(cd "$HERE/../win" && pwd)"
GALLERY="${DTEST_GALLERY:-/tmp/dtest}"
ENVFILE="${DTEST_ENV:-/tmp/dtest.env}"
DISPLAY="${DISPLAY:-:0}"
TITLE_MATCH="LVGL++ Designer"

mkdir -p "$GALLERY"

die() { echo "dtest: $*" >&2; exit 1; }

find_wid() {
  DISPLAY="$DISPLAY" xwininfo -root -tree 2>/dev/null \
    | grep -F "$TITLE_MATCH" | head -1 | awk '{print $1}'
}

# 把 ps1 放到 Windows 可访问的位置，回显 Windows 路径
stage_ps() {
  local name="$1"
  local wt
  wt=$(timeout 60 powershell.exe -NoProfile -Command '$env:TEMP' 2>/dev/null | tr -d '\r')
  [ -n "$wt" ] || die "取不到 Windows TEMP"
  local wsl
  wsl="${wt//\\//}"; wsl="/mnt/c${wsl#C:}"
  cp "$WIN_DIR/$name" "$wsl/$name" || die "复制 $name 失败"
  echo "$wt\\$name"
}

run_ps() {
  local ps_path
  ps_path=$(stage_ps win_input.ps1)
  timeout 120 powershell.exe -NoProfile -ExecutionPolicy Bypass -File "$ps_path" "$@" 2>&1 | tr -d '\r'
}

# 标定：X 窗口尺寸 -> Windows 客户区尺寸
write_env() {
  local wid
  wid=$(find_wid)
  [ -n "$wid" ] || die "没找到设计器窗口（标题含 '$TITLE_MATCH'）"
  local xw xh
  xw=$(DISPLAY="$DISPLAY" xwininfo -id "$wid" | awk '/Width:/{print $2}')
  xh=$(DISPLAY="$DISPLAY" xwininfo -id "$wid" | awk '/Height:/{print $2}')
  local info org size cox coy cw ch
  info=$(run_ps -Action info)
  org=$(echo "$info"  | grep -o 'CLIENTORIGIN=[0-9-]*,[0-9-]*' | cut -d= -f2)
  size=$(echo "$info" | grep -o 'CLIENTSIZE=[0-9]*x[0-9]*'     | cut -d= -f2)
  cox=${org%,*}; coy=${org#*,}
  cw=${size%x*}; ch=${size#*x}
  [ -n "$cox" ] && [ -n "$cw" ] && [ -n "$ch" ] || die "解析 Windows 窗口信息失败: $info"
  local sx sy
  sx=$(awk -v a="$cw" -v b="$xw" 'BEGIN{print a/b}')
  sy=$(awk -v a="$ch" -v b="$xh" 'BEGIN{print a/b}')
  cat > "$ENVFILE" <<EOF
WID=$wid
XW=$xw
XH=$xh
COX=$cox
COY=$coy
CW=$cw
CH=$ch
SX=$sx
SY=$sy
EOF
  cat "$ENVFILE"
}

load_env() {
  [ -f "$ENVFILE" ] || write_env >/dev/null
  # shellcheck disable=SC1090
  . "$ENVFILE"
  WID=$(find_wid)
  [ -n "$WID" ] || die "窗口已关闭"
}

# X 窗口坐标 -> Windows 屏幕坐标
xw_to_win() {
  awk -v x="$1" -v y="$2" -v cx="$COX" -v cy="$COY" -v sx="$SX" -v sy="$SY" \
      'BEGIN{printf "%d %d", cx + x*sx, cy + y*sy}'
}

shot() {
  local out="${1:-$GALLERY/shot.png}"
  DISPLAY="$DISPLAY" import -window "$WID" "$out" 2>/dev/null || die "截图失败"
  echo "$out"
}

cmd="${1:-}"; shift || true
case "$cmd" in
  env)
    write_env
    ;;
  locate)
    load_env
    img=$(shot "$GALLERY/locate.png")
    python3 "$HERE/locate_canvas.py" "$img"
    ;;
  shot)
    load_env
    shot "${1:-$GALLERY/shot.png}"
    ;;
  status)
    load_env
    tag="${1:-cur}"
    out="$GALLERY/status_$tag.png"
    DISPLAY="$DISPLAY" import -window "$WID" -crop 700x32+0+866 +repage "$out" 2>/dev/null
    echo "$out $(md5sum "$out" | cut -c1-10)"
    ;;
  click|drag|wheel|zoom|key)
    load_env
    A1="${1:-0}"; A2="${2:-0}"; A3="${3:-}"; A4="${4:-0}"; A5="${5:-0}"
    case "$cmd" in
      click)
        read -r wx wy <<< "$(xw_to_win "$A1" "$A2")"
        run_ps -Action click -X "$wx" -Y "$wy" -Button "$A5"
        ;;
      drag)
        read -r wx1 wy1 <<< "$(xw_to_win "$A1" "$A2")"
        read -r wx2 wy2 <<< "$(xw_to_win "$A3" "$A4")"
        run_ps -Action drag -X "$wx1" -Y "$wy1" -X2 "$wx2" -Y2 "$wy2" -Button "$A5"
        ;;
      wheel|zoom)
        read -r wx wy <<< "$(xw_to_win "$A1" "$A2")"
        [ -n "$A3" ] || A3=120
        mod=0; [ "$cmd" = zoom ] && mod=17
        run_ps -Action wheel -X "$wx" -Y "$wy" -Delta "$A3" -Mod "$mod"
        ;;
      key)
        run_ps -Action key -Mod "$A1"
        ;;
    esac
    ;;
  ""|-h|--help|help)
    sed -n '2,30p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'
    ;;
  *)
    die "未知子命令: $cmd（用 dtest.sh help）"
    ;;
esac
