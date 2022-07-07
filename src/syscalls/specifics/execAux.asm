extern execPartTwo

global execSwitchStack
execSwitchStack:
    mov rsp, rcx
    ; Arguments are left intact
    jmp execPartTwo
