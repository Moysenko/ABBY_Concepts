VARIABLES
    prev = 1
    current = 1
    next
    n

# input: fibonacci sequence element number
# output: fibonacci sequence element value
CODE
    fibonacci_loop:
        RET_ZERO n
        MOV_REG next current
        ADD_REG next prev
        MOV_REG prev current
        MOV_REG current next
        SUB_CONST n 1
        JMP fibonacci_loop

    main:
        READ_INT n
        SUB_CONST n 1
        CALL fibonacci_loop
        PRINT_INT current
