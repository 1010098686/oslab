# 系统启动时，BIOS会完成初始化，然后将磁盘第一个512字节保存到物理内存0x00007C00
# 位置，然后设置%CS=0000h, %IP=7C00h

#include "asm.h"
# 从此开始是16位代码
.code16
.globl start
start:
    #real mode code here
	#clear the screen
	movb $7,%ah
	movb $0,%al
	movb $0,%ch
	movb $0,%cl
	movb $20,%dh
	movb $80,%dl
	int $0x10
    #close the interrupt
	cli
    #open A20 address line
    #load gdt
	lgdt gdtdesc
    #set PE 1 
	movl %cr0,%eax
	orl $0x1,%eax
	movl %eax,%cr0
    #long jump to protection mode
	ljmp $GDT_ENTRY(1),$start32

#loop:
#    jmp loop
#hello: .string "Boot:Hello world!"

.code32
start32:
    #protection mode code here
    #set up the protexted-mode data segment registers
    movw $GDT_ENTRY(2),%ax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%ss
#	movw $GDT_ENTRY(3),%ax
#	movw %ax,%gs
    #set up a stack for C code
	movl $0x0,%ebp
	movl $(128<<20),%esp
    call bootmain
loop32:
    jmp loop32


# GDT
.p2align 2                         # 对齐
    #GDT definition here
gdt:
    SEG_NULLASM                    #NULL
    SEG_ASM(0x0a,0x0,0xffffffff)   #code
    SEG_ASM(0x02,0x0,0xffffffff)   #data
#	SEG_ASM(0x02,0xb8000,0xffffffff) #video

gdtdesc:                           # GDT描述符
	.word   (gdtdesc - gdt - 1)    # GDT长度，留意地址运算
	.long   gdt                    # GDT地址

