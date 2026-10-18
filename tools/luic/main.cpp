#include "parser.hpp"
#include "validator.hpp"
#include "codegen.hpp"
#include "component_registry.hpp"
#include <fstream>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include "third_party/nlohmann/json.hpp"

void print_usage() {
    printf("用法：\n");
    printf("  luic <输入.lui> [-o 输出.hpp] [--assets-include <路径>] [--verbose]\n");
    printf("  luic --project <工程清单.json> [--out-dir <目录>] [--emit-cmake <文件>]\n");
    printf("\n选项：\n");
    printf("  -o <文件>    输出文件路径（默认 <输入>_ui.hpp）\n");
    printf("  --assets-include <路径>  嵌入图片资源头文件的 include 路径\n");
    printf("                 （默认 assets/lvgl_assets.h，由 scripts/assets/convert_assets.py 生成）\n");
    printf("  --project <文件>  工程模式：编译清单中的全部 .lui，并解析 \"@组件\" 实例\n");
    printf("  --out-dir <目录>  工程模式的输出目录（默认取清单的 outDir 或 ui_generated）\n");
    printf("  --emit-cmake <文件>  工程模式输出 CMake 片段（头文件列表/包含目录）\n");
    printf("  --verbose    打印详细信息\n");
    printf("  --help       显示本帮助\n");
}

std::string replace_ext(const std::string& path, const std::string& new_ext) {
    auto dot = path.rfind('.');
    if (dot == std::string::npos) return path + new_ext;
    return path.substr(0, dot) + new_ext;
}

bool mkdir_p(const std::string& path) {
    for (size_t i = 1; i < path.size(); i++) {
        if (path[i] == '/') {
            std::string dir = path.substr(0, i);
            struct stat st;
            if (stat(dir.c_str(), &st) != 0) {
                if (mkdir(dir.c_str(), 0755) != 0 && errno != EEXIST) {
                    fprintf(stderr, "luic: 无法创建目录 %s：%s\n",
                            dir.c_str(), strerror(errno));
                    return false;
                }
            }
        }
    }
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        if (mkdir(path.c_str(), 0755) != 0 && errno != EEXIST) {
            fprintf(stderr, "luic: 无法创建目录 %s：%s\n",
                    path.c_str(), strerror(errno));
            return false;
        }
    }
    return true;
}

/* ---------------- 工程模式 ---------------- */
namespace {

std::string dir_of(const std::string& path) {
    auto slash = path.find_last_of('/');
    return slash == std::string::npos ? std::string(".") : path.substr(0, slash);
}

std::string base_noext(const std::string& path) {
    auto slash = path.find_last_of('/');
    std::string name = slash == std::string::npos ? path : path.substr(slash + 1);
    auto dot = name.rfind('.');
    return dot == std::string::npos ? name : name.substr(0, dot);
}

void collect_component_refs(const std::vector<luic::widget_node_t>& nodes,
                            std::set<std::string>& out) {
    for (auto& n : nodes) {
        if (!n.type.empty() && n.type[0] == '@') out.insert(n.type.substr(1));
        collect_component_refs(n.children, out);
    }
}

int run_project(const std::string& manifest, const std::string& out_dir_opt,
                const std::string& cmake_out, const std::string& assets_include,
                bool verbose) {
    using json = nlohmann::json;
    json j;
    {
        std::ifstream f(manifest);
        if (!f.is_open()) {
            fprintf(stderr, "luic: 无法打开工程清单 %s\n", manifest.c_str());
            return 1;
        }
        try { f >> j; } catch (const json::parse_error& e) {
            fprintf(stderr, "luic: 清单 JSON 错误：%s\n", e.what());
            return 1;
        }
    }

    std::string proj_name = j.value("name", "Project");
    std::string entry = j.value("entry", "");
    std::string base = dir_of(manifest);
    std::string out_dir = !out_dir_opt.empty() ? out_dir_opt : j.value("outDir", std::string("ui_generated"));
    if (!out_dir.empty() && out_dir[0] != '/') out_dir = base + "/" + out_dir;

    std::vector<std::string> rel_files;
    if (j.contains("files") && j["files"].is_array())
        for (auto& v : j["files"]) if (v.is_string()) rel_files.push_back(v.get<std::string>());
    if (rel_files.empty()) {
        fprintf(stderr, "luic: 清单里没有 files\n");
        return 1;
    }

    luic::ComponentRegistry registry;
    luic::Parser parser;

    /* 1) 解析全部文档 */
    std::vector<std::unique_ptr<luic::lui_document_t>> docs;
    std::map<std::string, const luic::lui_document_t*> by_name;
    std::map<std::string, std::string> rel_of;      // name -> 相对路径
    for (auto& rel : rel_files) {
        std::string path = (rel[0] == '/') ? rel : base + "/" + rel;
        auto doc = parser.parse(path);
        if (!doc) return 1;
        if (by_name.count(doc->name)) {
            fprintf(stderr, "luic: 组件/屏幕重名：%s（%s 与 %s）\n",
                    doc->name.c_str(), rel_of[doc->name].c_str(), rel.c_str());
            return 1;
        }
        rel_of[doc->name] = rel;
        docs.push_back(std::make_unique<luic::lui_document_t>(std::move(*doc)));
        by_name[docs.back()->name] = docs.back().get();
    }

    /* 2) 校验（带组件表） */
    luic::Validator validator(registry);
    validator.setComponents(&by_name);
    bool ok = true;
    for (auto& d : docs) {
        for (auto& err : validator.validate(*d)) {
            fprintf(stderr, "luic: 错误：%s: %s\n", d->name.c_str(), err.c_str());
            ok = false;
        }
    }
    if (!ok) return 1;

    /* 3) 依赖图 + 环检测 */
    std::map<std::string, std::set<std::string>> deps;
    for (auto& d : docs) collect_component_refs(d->children, deps[d->name]);

    std::map<std::string, int> color;   // 0=未访问 1=访问中 2=完成
    std::vector<std::string> order;
    std::vector<std::string> stack;
    std::function<bool(const std::string&)> dfs = [&](const std::string& name) -> bool {
        color[name] = 1;
        stack.push_back(name);
        for (auto& dep : deps[name]) {
            if (!by_name.count(dep)) continue;         // 已由校验器报错
            if (color[dep] == 1) {
                std::string path;
                bool in = false;
                for (auto& s : stack) {
                    if (s == dep) in = true;
                    if (in) path += (path.empty() ? "" : " -> ") + s;
                }
                path += " -> " + dep;
                fprintf(stderr, "luic: 错误：组件循环引用：%s\n", path.c_str());
                return false;
            }
            if (color[dep] == 0 && !dfs(dep)) return false;
        }
        stack.pop_back();
        color[name] = 2;
        order.push_back(name);
        return true;
    };
    for (auto& d : docs) {
        if (color[d->name] == 0 && !dfs(d->name)) return 1;
    }

    /* 4) 输出目录：扁平放置，要求文件名不冲突（include 时只用一个 -I 就够了） */
    if (!mkdir_p(out_dir)) return 1;
    std::map<std::string, std::string> out_of;   // name -> 输出文件
    std::set<std::string> seen_output;
    for (auto& d : docs) {
        std::string out = out_dir + "/" + base_noext(rel_of[d->name]) + "_ui.hpp";
        if (!seen_output.insert(out).second) {
            fprintf(stderr, "luic: 生成文件名冲突：%s（请修改文件名）\n", out.c_str());
            return 1;
        }
        out_of[d->name] = out;
    }

    /* 5) 按依赖顺序生成 */
    std::map<std::string, const luic::lui_document_t*> doc_by_name;
    for (auto& d : docs) doc_by_name[d->name] = d.get();

    std::vector<std::string> headers;
    std::set<std::string> include_dirs;
    include_dirs.insert(out_dir);
    for (auto& name : order) {
        auto* d = doc_by_name[name];
        luic::CodeGenerator codegen(registry);
        codegen.setComponents(&by_name);
        if (!assets_include.empty()) codegen.setAssetsInclude(assets_include);
        std::string code = codegen.generate(*d);

        std::ofstream out(out_of[name]);
        if (!out.is_open()) {
            fprintf(stderr, "luic: 无法写入 %s\n", out_of[name].c_str());
            return 1;
        }
        out << code;
        headers.push_back(out_of[name]);
        if (!d->resources.empty()) include_dirs.insert(dir_of(out_of[name]) + "/..");  // 资源头相对 .lui 目录
        if (verbose)
            printf("luic: %s (%s) -> %s（%zu 字节）\n", name.c_str(),
                   d->kind.c_str(), out_of[name].c_str(), code.size());
        else
            printf("luic: %s -> %s\n", name.c_str(), out_of[name].c_str());
    }

    /* 6) CMake 片段 */
    if (!cmake_out.empty()) {
        std::ofstream cm(cmake_out);
        if (!cm.is_open()) {
            fprintf(stderr, "luic: 无法写入 %s\n", cmake_out.c_str());
            return 1;
        }
        cm << "# Auto-generated by luic --project（勿手动编辑）\n";
        cm << "set(LVGLPP_PROJECT_NAME \"" << proj_name << "\")\n";
        cm << "set(LVGLPP_ENTRY \"" << entry << "\")\n";
        cm << "set(LVGLPP_UI_HEADERS\n";
        for (auto& h : headers) cm << "    \"" << h << "\"\n";
        cm << ")\n";
        cm << "set(LVGLPP_UI_INCLUDE_DIRS\n";
        for (auto& d : include_dirs) cm << "    \"" << d << "\"\n";
        cm << ")\n";
    }

    printf("luic: 工程 '%s'：%zu 个文件已生成到 %s%s\n", proj_name.c_str(),
           headers.size(), out_dir.c_str(),
           entry.empty() ? "" : ("，入口 " + entry).c_str());
    return 0;
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string input_file;
    std::string output_file;
    std::string assets_include;
    std::string project_file;
    std::string out_dir;
    std::string cmake_out;
    bool verbose = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (strcmp(argv[i], "--assets-include") == 0 && i + 1 < argc) {
            assets_include = argv[++i];
        } else if (strcmp(argv[i], "--project") == 0 && i + 1 < argc) {
            project_file = argv[++i];
        } else if (strcmp(argv[i], "--out-dir") == 0 && i + 1 < argc) {
            out_dir = argv[++i];
        } else if (strcmp(argv[i], "--emit-cmake") == 0 && i + 1 < argc) {
            cmake_out = argv[++i];
        } else if (input_file.empty()) {
            input_file = argv[i];
        }
    }

    if (!project_file.empty()) {
        if (!input_file.empty()) {
            fprintf(stderr, "luic: --project 与单文件输入不能同时使用\n");
            return 1;
        }
        return run_project(project_file, out_dir, cmake_out, assets_include, verbose);
    }

    if (input_file.empty()) {
        fprintf(stderr, "luic: 未指定输入文件\n");
        return 1;
    }

    if (output_file.empty()) {
        output_file = replace_ext(input_file, "_ui.hpp");
    }

    if (verbose) {
        printf("luic: %s -> %s\n", input_file.c_str(), output_file.c_str());
    }

    luic::Parser parser;
    auto doc = parser.parse(input_file);
    if (!doc) {
        return 1;
    }

    if (verbose) {
        auto nodes = doc->all_nodes();
        printf("luic: 已解析 '%s'（%zu 个节点）\n", doc->name.c_str(), nodes.size());
    }

    luic::ComponentRegistry registry;
    luic::Validator validator(registry);
    auto errors = validator.validate(*doc);

    if (!errors.empty()) {
        for (auto& err : errors) {
            fprintf(stderr, "luic: 错误：%s\n", err.c_str());
        }
        return 1;
    }

    luic::CodeGenerator codegen(registry);
    if (!assets_include.empty()) codegen.setAssetsInclude(assets_include);
    std::string code = codegen.generate(*doc);

    auto slash = output_file.rfind('/');
    if (slash != std::string::npos) {
        if (!mkdir_p(output_file.substr(0, slash))) {
            return 1;
        }
    }

    std::ofstream out(output_file);
    if (!out.is_open()) {
        fprintf(stderr, "luic: 无法写入 %s\n", output_file.c_str());
        return 1;
    }
    out << code;
    out.close();

    printf("luic: %s -> %s（%zu 字节）\n",
           input_file.c_str(), output_file.c_str(), code.size());
    return 0;
}
