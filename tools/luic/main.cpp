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

void print_usage() {
    printf("Usage: luic <input.lui> [-o output.hpp] [--verbose]\n");
    printf("\nOptions:\n");
    printf("  -o <file>    Output file path (default: <input>_ui.hpp)\n");
    printf("  --verbose    Print detailed information\n");
    printf("  --help       Show this help\n");
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
                    fprintf(stderr, "luic: cannot create directory %s: %s\n",
                            dir.c_str(), strerror(errno));
                    return false;
                }
            }
        }
    }
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        if (mkdir(path.c_str(), 0755) != 0 && errno != EEXIST) {
            fprintf(stderr, "luic: cannot create directory %s: %s\n",
                    path.c_str(), strerror(errno));
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string input_file;
    std::string output_file;
    bool verbose = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (input_file.empty()) {
            input_file = argv[i];
        }
    }

    if (input_file.empty()) {
        fprintf(stderr, "luic: no input file\n");
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
        printf("luic: parsed '%s' (%zu nodes)\n", doc->name.c_str(), nodes.size());
    }

    luic::ComponentRegistry registry;
    luic::Validator validator(registry);
    auto errors = validator.validate(*doc);

    if (!errors.empty()) {
        for (auto& err : errors) {
            fprintf(stderr, "luic: error: %s\n", err.c_str());
        }
        return 1;
    }

    luic::CodeGenerator codegen(registry);
    std::string code = codegen.generate(*doc);

    auto slash = output_file.rfind('/');
    if (slash != std::string::npos) {
        if (!mkdir_p(output_file.substr(0, slash))) {
            return 1;
        }
    }

    std::ofstream out(output_file);
    if (!out.is_open()) {
        fprintf(stderr, "luic: cannot write to %s\n", output_file.c_str());
        return 1;
    }
    out << code;
    out.close();

    printf("luic: %s -> %s (%zu bytes)\n",
           input_file.c_str(), output_file.c_str(), code.size());
    return 0;
}
