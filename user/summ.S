.data
    msg_err_space: .asciz "Incorrect format: no space\n"
    msg_fmt: .asciz "|%s|\n"
    msg_fmt_int: .asciz "%d\n"

#define MAX_BUF_SIZE 100

.bss
    buf: .skip 100

.text
.global main

main:
    la a0, buf
    li a1, MAX_BUF_SIZE
    call gets
    
    la a0, msg_fmt
    la a1, buf
    call printf
    
    la a0, buf
    li t0, 0
    li t1, -1
    li t4, 32
    
find:
    lb t2, 0(a0)
    beqz t2, done
    beq t2, t4, found
    addi a0, a0, 1
    addi t0, t0, 1
    j find

found:
    mv t1, t0
    mv t5, a0
    j done

done: 
    li t3, -1
    beq t1, t3, fail
    li t2, 0
    sb t2, 0(a0)
    
    la a0, buf
    call atoi
    mv t2, a0
    
    addi a0, t5, 1
    call atoi
    
    add a1, t2, a0
    la a0, msg_fmt_int
    call printf
    
    li a7, 2
    li a0, 0
    ecall
    
fail:
    li a7, 16
    li a0, 2
    la a1, msg_err_space
    li a2, 27
    ecall
    
    li a7, 2
    li a0, 1
    ecall
