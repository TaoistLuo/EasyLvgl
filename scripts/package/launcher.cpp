#include <windows.h>

#include <string>
#include <vector>

namespace {

std::wstring executableDir() {
    std::vector<wchar_t> buffer(32768);
    const DWORD n = GetModuleFileNameW(nullptr, buffer.data(),
                                       static_cast<DWORD>(buffer.size()));
    std::wstring path(buffer.data(), n);
    const std::wstring::size_type slash = path.find_last_of(L"\\/");
    return slash == std::wstring::npos ? std::wstring() : path.substr(0, slash);
}

std::wstring join(const std::wstring& a, const wchar_t* b) {
    if (a.empty()) return b;
    return a + L"\\" + b;
}

bool exists(const std::wstring& path) {
    return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

std::wstring quote(const std::wstring& value) {
    return L"\"" + value + L"\"";
}

void fail(const std::wstring& message) {
    MessageBoxW(nullptr, message.c_str(), L"EasyLVGL", MB_ICONERROR | MB_OK);
}

} // namespace

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    const std::wstring root = executableDir();
    if (root.empty()) {
        fail(L"无法确定程序目录。");
        return 1;
    }

    std::wstring appRoot = join(root, L"EasyLVGL");
    std::wstring designer = join(
        join(join(join(root, L"EasyLVGL"), L"tools"), L"lvgl-designer"),
        L"build-win\\lvgl-designer.exe");
    if (!exists(designer)) {
        appRoot = root;
        designer = join(
            join(join(root, L"tools"), L"lvgl-designer"),
            L"build-win\\lvgl-designer.exe");
    }
    if (!exists(designer)) {
        fail(L"找不到设计器：\n" + designer);
        return 1;
    }

    const std::wstring toolchainCandidates[] = {
        join(root, L"toolchain"),
        join(appRoot, L"toolchain"),
        join(join(root, L".."), L"toolchain"),
        join(join(join(root, L".."), L".."), L"toolchain"),
    };
    std::wstring toolchain;
    for (const auto& candidate : toolchainCandidates) {
        if (exists(join(join(join(candidate, L"mingw64"), L"bin"), L"g++.exe"))) {
            toolchain = candidate;
            break;
        }
    }
    if (toolchain.empty()) {
        fail(L"找不到 toolchain 目录。\n请保持 EasyLVGL.exe、EasyLVGL、toolchain 的相对结构。");
        return 1;
    }

    const std::wstring bin = join(appRoot, L"bin");
    std::wstring path;
    const wchar_t* dirs[] = {
        L"mingw64\\bin",
        L"cmake\\bin",
        L"python",
        L"python\\Scripts",
    };
    for (const wchar_t* dir : dirs) path += join(toolchain, dir) + L";";
    path += bin + L";";

    wchar_t oldPath[32768] = {};
    GetEnvironmentVariableW(L"PATH", oldPath, 32768);
    path += oldPath;

    SetEnvironmentVariableW(L"PATH", path.c_str());
    SetEnvironmentVariableW(L"EASYLVGL_ROOT", appRoot.c_str());

    std::wstring command = quote(designer);
    std::vector<wchar_t> commandLine(command.begin(), command.end());
    commandLine.push_back(0);

    STARTUPINFOW startup = {};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process = {};
    if (!CreateProcessW(designer.c_str(), commandLine.data(), nullptr, nullptr, FALSE,
                        CREATE_UNICODE_ENVIRONMENT, nullptr, appRoot.c_str(),
                        &startup, &process)) {
        fail(L"无法启动设计器：\n" + designer + L"\n\n错误码：" +
             std::to_wstring(GetLastError()));
        return 1;
    }

    WaitForSingleObject(process.hProcess, INFINITE);
    DWORD code = 0;
    GetExitCodeProcess(process.hProcess, &code);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    return static_cast<int>(code);
}
