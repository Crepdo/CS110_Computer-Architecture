# string
.data
string1: .string "Your score"

.section .text
.globl UIShowScore
.type UIShowScore, @function

# int score
UIShowScore:

# Save ra
addi	sp, sp,	-8
sw	s0,	4(sp)
sw	ra,	0(sp)

# save a0
addi	s0, a0, 0

# show string
li	a0,	0
li	a1, 49
la	a2,	string1
li  a3, 0xFFFF   # color
jal	LCD_ShowString

# show score
li	a0,	119
li	a1, 49
addi	a2, s0, 0
li	a3, 2
li	a4, 0x07E0	# Color
jal	LCD_ShowNum

# load ra
lw	s0,	4(sp)
lw	ra, 0(sp)
addi	sp,	sp,	8

# return
ret