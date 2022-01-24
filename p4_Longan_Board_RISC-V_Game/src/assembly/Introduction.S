# strings
.data
string1: .string "You, a SPACE TRUCK driver, come to rescue Astronauts attacked by the ALIENs."
string2: .string "TARGET:"
string3: .string "GREEN"
string4: .string "ENEMY:"
string5: .string "RED"
string6: .string "SHIP:"
string7: .string "BLUE"
string8: .string "DIFFCULTY 1"
string9: .string "A"
string10: .string "DIFFCULTY 2"
string11: .string "B"

.section .text
.globl Introduction
.type Introduction, @function

# no arg
Introduction:
# Save ra
addi	sp, sp,	-4
sw	ra,	0(sp)

# lcd clear
li	a0,	0x0000;
jal	LCD_Clear

# delay 100ms
li	a0,	100
jal	delay_1ms

# show string1
li	a0,	0
li	a1, 0
la	a2,	string1
li  a3, 0xFFE0
jal	LCD_ShowString

# delay 1000ms
li	a0,	1000
jal	delay_1ms

# lcd clear
li	a0,	0x0000;
jal	LCD_Clear

# show string
li	a0,	0
li	a1, 0
la	a2,	string2
li  a3, 0xFFFF   # color
jal	LCD_ShowString

# show string
li	a0,	80
li	a1, 0
la	a2,	string3
li  a3, 0x07E0   # color
jal	LCD_ShowString

# show string
li	a0,	0
li	a1, 30
la	a2,	string4
li  a3, 0xFFFF   # color
jal	LCD_ShowString

# show string
li	a0,	80
li	a1, 30
la	a2,	string5
li  a3, 0xF800   # color
jal	LCD_ShowString

# show string
li	a0,	0
li	a1, 60
la	a2,	string6
li  a3, 0xFFFF   # color
jal	LCD_ShowString

# show string
li	a0,	80
li	a1, 60
la	a2,	string7
li  a3, 0x001F   # color
jal	LCD_ShowString

# delay 1000ms
li	a0,	1000
jal	delay_1ms

# lcd clear
li	a0,	0x0000;
jal	LCD_Clear

# delay 200ms
li	a0,	200
jal	delay_1ms

# show string
li	a0,	0
li	a1, 0
la	a2,	string8
li  a3, 0xFFFF   # color
jal	LCD_ShowString

# show string
li	a0,	120
li	a1, 0
la	a2,	string9
li  a3, 0x07E0  # color
jal	LCD_ShowString

# show string
li	a0,	0
li	a1, 30
la	a2,	string10
li  a3, 0xFFFF  # color
jal	LCD_ShowString

# show string
li	a0,	120
li	a1, 30
la	a2,	string11
li  a3, 0xF800  # color
jal	LCD_ShowString

# load ra
lw	ra, 0(sp)
addi	sp,	sp,	4

# return
ret
