.data
    msg_err_overflow: .asciz "Buffer overflow\n"
    msg_err_read: .asciz "Read fail\n"
    msg_err_space: .asciz "Incorrect format: no space\n"

#define MAX_BUF_SIZE 100

.bss
    buf: .skip 100

.text
.global main

main:
    la a0, buf
    li a1, MAX_BUF_SIZE
    call gets
    call printf