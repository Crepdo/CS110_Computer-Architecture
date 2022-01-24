.section .text
.globl DrawWall
.type DrawWall, @function
# no arg
DrawWall:

# Save ra
addi	sp, sp,	-4
sw	ra,	0(sp)

# draw rect
li  a0, 0
li  a1, 0
li  a2, 159
li  a3, 79
li  a4, 0xF800 # Color
jal LCD_DrawRectangle

# load ra
lw	ra, 0(sp)
addi	sp,	sp,	4

# return
ret