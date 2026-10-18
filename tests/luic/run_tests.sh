#!/bin/bash
set -e

LUIC="$(dirname "$0")/../../bin/luic"
TEST_DIR="$(dirname "$0")"
OUT_DIR="/tmp/luic_test_output"
ROOT_DIR="$(cd "$TEST_DIR/../.." && pwd)"
PASS=0
FAIL=0

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

compile_output() {
    local output="$1"
    local compile_unit="$OUT_DIR/compile_$(basename "$output" .hpp).cpp"
    local object_file="$OUT_DIR/$(basename "$output" .hpp).o"

    cat > "$compile_unit" <<EOF
#include "$output"
int main() { return 0; }
EOF

    g++ -std=c++17 -I"$ROOT_DIR" -I"$ROOT_DIR/lvgl" -I"$ROOT_DIR/easylvgl" -I"$ROOT_DIR/tools/luic" -c "$compile_unit" -o "$object_file"
}

run_test() {
    local name="$1"
    local input="$2"
    local expect_fail="$3"
    
    printf "  %-30s " "$name"
    
    if [ "$expect_fail" = "fail" ]; then
        if $LUIC "$input" -o "$OUT_DIR/$(basename "$input" .lui)_ui.hpp" 2>/dev/null; then
            printf "FAIL (expected error but succeeded)\n"
            FAIL=$((FAIL + 1))
        else
            printf "PASS (correctly rejected)\n"
            PASS=$((PASS + 1))
        fi
    else
        if $LUIC "$input" -o "$OUT_DIR/$(basename "$input" .lui)_ui.hpp" 2>/dev/null; then
            local output="$OUT_DIR/$(basename "$input" .lui)_ui.hpp"
            if grep -q "class.*_UI" "$output" && grep -q "#pragma once" "$output" && compile_output "$output" >/dev/null 2>&1; then
                printf "PASS\n"
                PASS=$((PASS + 1))
            else
                printf "FAIL (invalid or uncompilable output)\n"
                FAIL=$((FAIL + 1))
            fi
        else
            printf "FAIL (compilation error)\n"
            FAIL=$((FAIL + 1))
        fi
    fi
}

printf "=== luic Unit Tests ===\n\n"

# 正常用例
run_test "minimal (1 widget)"        "$TEST_DIR/test_minimal.lui"
run_test "core widgets (8 widgets)"  "$TEST_DIR/test_core_widgets.lui"
run_test "nested containers"         "$TEST_DIR/test_nested.lui"
run_test "led bool mapping"          "$TEST_DIR/test_led_bool.lui"
run_test "main_screen (full)"        "$TEST_DIR/../../ui/main_screen.lui"

# 错误用例
run_test "invalid type (expect fail)" "$TEST_DIR/test_invalid_type.lui" "fail"
run_test "empty children (expect fail)" "$TEST_DIR/test_empty.lui" "fail"
run_test "bad size type (expect fail)" "$TEST_DIR/test_bad_prop_type_size.lui" "fail"
run_test "bad bool type (expect fail)" "$TEST_DIR/test_bad_prop_type_bool.lui" "fail"
run_test "bad range type (expect fail)" "$TEST_DIR/test_bad_prop_type_range.lui" "fail"
run_test "bad options type (expect fail)" "$TEST_DIR/test_bad_prop_type_options.lui" "fail"
run_test "bad textarea types (expect fail)" "$TEST_DIR/test_bad_prop_type_textarea.lui" "fail"
run_test "non-container children (fail)" "$TEST_DIR/test_invalid_children_non_container.lui" "fail"
run_test "invalid theme value (expect fail)" "$TEST_DIR/test_invalid_theme_value.lui" "fail"

printf "\n=== Results ===\n"
printf "  PASS: %d\n" $PASS
printf "  FAIL: %d\n" $FAIL
printf "  TOTAL: %d\n" $((PASS + FAIL))

if [ $FAIL -eq 0 ]; then
    printf "\n✅ All tests passed!\n"
    exit 0
else
    printf "\n❌ %d test(s) failed!\n" $FAIL
    exit 1
fi
