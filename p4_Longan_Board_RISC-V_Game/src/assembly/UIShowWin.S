# string
.data
string1: .string "Win!"

.section .text
.globl UIShowWin
.type UIShowWin, @function
# no arg
UIShowWin:

# Save ra
addi	sp, sp,	-4
sw	ra,	0(sp)

# lcd clear
li	a0,	0x0000;
jal	LCD_Clear

# delay 100ms
li	a0,	100
jal	delay_1ms

# show string
li	a0,	0
li	a1, 0
la	a2,	string1
li  a3, 0xFFFF   # color
jal	LCD_ShowString

# load ra
lw	ra, 0(sp)
addi	sp,	sp,	4

# return
ret