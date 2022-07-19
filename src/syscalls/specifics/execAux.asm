extern execPartTwo

global execSwitchStack
execSwitchStack:
    mov rsp, r9
    ; Arguments are left intact
    jmp execPartTwo
