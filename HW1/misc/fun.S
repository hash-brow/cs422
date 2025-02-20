.section .data

.align 512

addr1:
.skip 512
addr2:
.skip 512
addr3:
.skip 10
addr4:
.skip 10
addr5:
.skip 12
addr6:
.skip 512

.section .text

.global _start

_start:
    movl $0x69, addr2
    #movl $0x70, addr3

    #movl $0x69, addr4
    #movl $0x69, addr5
    #movl $0x69, addr6

    #inc addr1

    mov $addr6, %eax
    inc (%eax)

    mov $1, %eax
    xor %ebx, %ebx
    int $0x80

