.section .data

addr1:
.skip 512
addr2:
.skip 512
addr3:
.skip 512

.section .text

.global _start

_start:
    movq $0x69, addr1
    movq $0x69, addr2
    movq $0x69, addr3

    xor %rdi, %rdi
    mov $60, %rax
    syscall

