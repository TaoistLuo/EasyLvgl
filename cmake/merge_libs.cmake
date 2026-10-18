file(WRITE "${CMAKE_BINARY_DIR}/merge.mri"
    "CREATE ${LIB_OUT}\n"
    "ADDLIB ${LIB_A}\n"
    "ADDLIB ${LIB_B}\n"
    "ADDLIB ${LIB_C}\n"
    "SAVE\n"
    "END\n"
)
execute_process(COMMAND ${AR} -M INPUT_FILE "${CMAKE_BINARY_DIR}/merge.mri")
