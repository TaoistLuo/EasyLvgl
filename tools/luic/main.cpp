#include "parser.hpp"
#include "validator.hpp"
#include "codegen.hpp"
#include "component_registry.hpp"
#include <fstream>
#include <cstdio>
#include <cstring>

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

    // 1. Parse
    luic::Parser parser;
    auto doc = parser.parse(input_file);
    if (!doc) {
        return 1;
    }

    if (verbose) {
        auto nodes = doc->all_nodes();
        printf("luic: parsed '%s' (%zu nodes)\n", doc->name.c_str(), nodes.size());
    }

    // 2. Validate
    luic::ComponentRegistry registry;
    luic::Validator validator(registry);
    auto errors = validator.validate(*doc);

    if (!errors.empty()) {
        for (auto& err : errors) {
            fprintf(stderr, "luic: error: %s\n", err.c_str());
        }
        return 1;
    }

    // 3. Generate
    luic::CodeGenerator codegen(registry);
    std::string code = codegen.generate(*doc);

    // 4. Write output
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
