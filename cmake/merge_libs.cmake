# 把多个静态库合并成一个（GNU ar / LLVM ar 的 MRI 脚本模式）。
#
# 由顶层 CMakeLists.txt 以 -D 传入：
#   LIB_OUT  合并后的库（绝对路径）
#   LIB_A/B/C  待合并的库（绝对路径）
#   AR       ar 可执行文件
# 自定义命令的工作目录是 CMAKE_BINARY_DIR，这里把路径转成相对路径，
# 避免 Windows 盘符（如 F:/...）让 ar 解析失败。

file(TO_CMAKE_PATH "${LIB_OUT}" LIB_OUT)
file(TO_CMAKE_PATH "${LIB_A}" LIB_A)
file(TO_CMAKE_PATH "${LIB_B}" LIB_B)
file(TO_CMAKE_PATH "${LIB_C}" LIB_C)

file(RELATIVE_PATH LIB_OUT_REL "${CMAKE_BINARY_DIR}" "${LIB_OUT}")
file(RELATIVE_PATH LIB_A_REL "${CMAKE_BINARY_DIR}" "${LIB_A}")
file(RELATIVE_PATH LIB_B_REL "${CMAKE_BINARY_DIR}" "${LIB_B}")
file(RELATIVE_PATH LIB_C_REL "${CMAKE_BINARY_DIR}" "${LIB_C}")

file(WRITE "${CMAKE_BINARY_DIR}/merge.mri"
    "CREATE ${LIB_OUT_REL}\n"
    "ADDLIB ${LIB_A_REL}\n"
    "ADDLIB ${LIB_B_REL}\n"
    "ADDLIB ${LIB_C_REL}\n"
    "SAVE\n"
    "END\n"
)

execute_process(
    COMMAND "${AR}" -M INPUT_FILE "${CMAKE_BINARY_DIR}/merge.mri"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    RESULT_VARIABLE MERGE_RESULT
    OUTPUT_VARIABLE MERGE_STDOUT
    ERROR_VARIABLE MERGE_STDERR
)

if(NOT MERGE_RESULT EQUAL 0)
    message(FATAL_ERROR "ar merge failed (${MERGE_RESULT}):\n${MERGE_STDOUT}\n${MERGE_STDERR}")
endif()
