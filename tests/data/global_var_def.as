.text
.globl _start
.globl huvud

_start:
    call huvud
    mov %rax, %rdi
    mov $60, %rax
    syscall
huvud:
    push %rbp
    mov %rsp, %rbp
    movq $0, %rax
    jmp .exit_huvud
.exit_huvud:
    mov %rbp, %rsp
    pop %rbp
    ret
.data
global_var:
    .long 1621
.bss
