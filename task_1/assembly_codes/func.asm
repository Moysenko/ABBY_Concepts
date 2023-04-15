VARIABLES
    a = 1

CODE
    func:
        PRINT_INT a
        MOV_CONST a 3
        RET

    main:
        MOV_CONST a 2
        CALL func
        PRINT_INT a
        EXIT