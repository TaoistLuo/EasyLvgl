# Win32 输入注入：从 WSL 操作 WSLg 窗口（绕开 XWayland 不转发 XTEST 的问题）
#
# 用法（由 dtest.sh 调用，坐标为 Windows 屏幕坐标）：
#   win_input.ps1 -Action click      -X 100 -Y 200 [-Button 0|1|2] [-Mod 17]
#   win_input.ps1 -Action drag       -X 100 -Y 200 -X2 300 -Y2 400 [-Button 0|1|2] [-Mod 17]
#   win_input.ps1 -Action move       -X 100 -Y 200
#   win_input.ps1 -Action wheel      -X 100 -Y 200 [-Delta 120] [-Mod 17]
#   win_input.ps1 -Action key        -Mod 27            # 单键点按（VK 码）
#   win_input.ps1 -Action info                          # 仅打印窗口定位信息
#
# Button: 0=左 1=右 2=中
# Mod:    按下并保持的修饰键 VK 码（Ctrl=17, Shift=16, Alt=18, Win=91）
# Delta:  滚轮步进，正=上滚，负=下滚（120 为一格）

param(
  [string]$Action = 'info',
  [int]$X = 0, [int]$Y = 0,
  [int]$X2 = 0, [int]$Y2 = 0,
  [int]$Steps = 24,
  [int]$Button = 0,
  [int]$Mod = 0,
  [int]$Delta = 120
)

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class W {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumWindowsProc cb, IntPtr l);
  [DllImport("user32.dll")] public static extern int GetWindowText(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
  [DllImport("user32.dll")] public static extern void mouse_event(int f, int dx, int dy, int d, IntPtr e);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint flags, IntPtr extra);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
  public delegate bool EnumWindowsProc(IntPtr h, IntPtr l);
  public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }
  public struct POINT { public int X; public int Y; }
}
"@

$LD = 0x02; $LU = 0x04; $RD = 0x08; $RU = 0x10; $MD = 0x20; $MU = 0x40; $WHEEL = 0x0800
$KEYUP = 0x0002
$DOWN = $LD; $UP = $LU
if ($Button -eq 1) { $DOWN = $RD; $UP = $RU }
if ($Button -eq 2) { $DOWN = $MD; $UP = $MU }

$script:target = [IntPtr]::Zero
$cb = [W+EnumWindowsProc]{
  param($h, $l)
  if ([W]::IsWindowVisible($h)) {
    $sb = New-Object System.Text.StringBuilder 512
    [W]::GetWindowText($h, $sb, 512) | Out-Null
    if ($sb.ToString() -like "*LVGL++ Designer*") { $script:target = $h }
  }
  return $true
}
[W]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null

if ($script:target -eq [IntPtr]::Zero) { Write-Output "TARGET NOT FOUND"; exit 1 }

$wr = New-Object W+RECT; [W]::GetWindowRect($script:target, [ref]$wr) | Out-Null
$cr = New-Object W+RECT; [W]::GetClientRect($script:target, [ref]$cr) | Out-Null
$pt = New-Object W+POINT; [W]::ClientToScreen($script:target, [ref]$pt) | Out-Null

if ($Action -eq 'info') {
  Write-Output ("HANDLE={0}" -f $script:target)
  Write-Output ("WINDOWRECT={0},{1}-{2},{3} ({4}x{5})" -f $wr.Left, $wr.Top, $wr.Right, $wr.Bottom, ($wr.Right-$wr.Left), ($wr.Bottom-$wr.Top))
  Write-Output ("CLIENTORIGIN={0},{1} CLIENTSIZE={2}x{3}" -f $pt.X, $pt.Y, $cr.Right, $cr.Bottom)
  exit 0
}

[W]::SetForegroundWindow($script:target) | Out-Null
Start-Sleep -Milliseconds 250

if ($Mod -gt 0) { [W]::keybd_event([byte]$Mod, 0, 0, [IntPtr]::Zero); Start-Sleep -Milliseconds 150 }

switch ($Action) {
  'click' {
    [W]::SetCursorPos($X, $Y) | Out-Null
    Start-Sleep -Milliseconds 150
    [W]::mouse_event($DOWN, 0, 0, 0, [IntPtr]::Zero)
    Start-Sleep -Milliseconds 120
    [W]::mouse_event($UP, 0, 0, 0, [IntPtr]::Zero)
    Write-Output ("click {0},{1} btn={2} mod={3}" -f $X, $Y, $Button, $Mod)
  }
  'drag' {
    [W]::SetCursorPos($X, $Y) | Out-Null
    Start-Sleep -Milliseconds 250
    [W]::mouse_event($DOWN, 0, 0, 0, [IntPtr]::Zero)
    Start-Sleep -Milliseconds 250
    for ($i = 1; $i -le $Steps; $i++) {
      [W]::SetCursorPos([int]($X + ($X2-$X)*$i/$Steps), [int]($Y + ($Y2-$Y)*$i/$Steps)) | Out-Null
      Start-Sleep -Milliseconds 25
    }
    Start-Sleep -Milliseconds 200
    [W]::mouse_event($UP, 0, 0, 0, [IntPtr]::Zero)
    Write-Output ("drag {0},{1} -> {2},{3} btn={4} mod={5}" -f $X, $Y, $X2, $Y2, $Button, $Mod)
  }
  'move' {
    [W]::SetCursorPos($X, $Y) | Out-Null
    Write-Output ("move {0},{1}" -f $X, $Y)
  }
  'wheel' {
    [W]::SetCursorPos($X, $Y) | Out-Null
    Start-Sleep -Milliseconds 150
    [W]::mouse_event($WHEEL, 0, 0, $Delta, [IntPtr]::Zero)
    Write-Output ("wheel {0},{1} delta={2} mod={3}" -f $X, $Y, $Delta, $Mod)
  }
  'key' {
    [W]::keybd_event([byte]$Mod, 0, 0, [IntPtr]::Zero)
    Start-Sleep -Milliseconds 120
    [W]::keybd_event([byte]$Mod, 0, $KEYUP, [IntPtr]::Zero)
    Write-Output ("key vk={0}" -f $Mod)
  }
}

if ($Mod -gt 0 -and $Action -ne 'key') {
  [W]::keybd_event([byte]$Mod, 0, $KEYUP, [IntPtr]::Zero)
}
