VARIABLES
    a = 1
    b = 3
    c

CODE
    ADD_REG a b
    PRINT_INT a
    SUB_REG a b
    PRINT_INT a
    SUB_CONST a 5
    PRINT_INT a
    MUL_REG a b
    PRINT_INT a

    MOV_REG c a
    PRINT_INT c
    MOV_CONST c 5
    PRINT_INT c
