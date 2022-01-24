# strings
.data
string1: .string "SPACE RESCUE"
string2: .string "Press [A] to play"

.section .text
.globl UIUpdateMainMenu
.type UIUpdateMainMenu, @function

# No arg
UIUpdateMainMenu:

# Save ra
addi	sp, sp,	-4
sw	ra,	0(sp)

# draw rect
li  a0, 0
li  a1, 0
li  a2, 159
li  a3, 79
li  a4, 0x07E0 # Color
jal LCD_DrawRectangle

# draw rect
li  a0, 14
li  a1, 14
li  a2, 144
li  a3, 64
li  a4, 0x0000 # Color
jal LCD_DrawRectangle

# show string
li	a0,	15
li	a1, 10
la	a2,	string1
li  a3, 0xFFE0  # Color
jal	LCD_ShowString

# show string
li	a0,	15
li	a1, 40
la	a2,	string2
li  a3, 0xFFFF  # Color
jal	LCD_ShowString

# load ra
lw	ra, 0(sp)
addi	sp,	sp,	4

# return
ret