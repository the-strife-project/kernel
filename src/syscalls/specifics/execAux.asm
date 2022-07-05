extern execPartTwo

global execSwitchStack
execSwitchStack:
    mov rsp, rdx
    ; Arguments are left intact
    jmp execPartTwo
