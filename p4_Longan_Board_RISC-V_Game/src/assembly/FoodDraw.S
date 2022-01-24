.section .text
.globl fooddraw
.type fooddraw, @function
fooddraw:

# /*Draw both type of food*/
# void FoodDraw(int foodPosX, int foodPosY, int food_color)
# {
#    LCD_Fill(foodPosX * 5, foodPosY * 5, (foodPosX + 1) * 5 - 1, (foodPosY + 1) * 5 - 1, food_color);
#     LCD_Fill(foodPosX * 5 + 1, foodPosY * 5 + 1, (foodPosX + 1) * 5 - 2, (foodPosY + 1) * 5 - 2, WHITE);
# }
# Save ra
# Save sp s

addi	sp, sp, -32
sw  s6, 28(sp)
sw  s5, 24(sp)
sw  s4, 20(sp)
sw  s3, 16(sp)
sw  s2, 12(sp)
sw  s1, 8(sp)
sw  s0, 4(sp)
sw	ra,	0(sp)

# a0 food posx
# a1 food posY
# a2 food color

# save arguments
addi  s0, a0, 0
addi  s1, a1, 0
addi  s2, a2, 0

#Draw outer
# Draw head
# a0
li	t1,	5
mul	a0, t0, t1

# Draw head
# a0
lw	t0,	0(s0)
li	t1,	5
mul	a0, t0, t1

# a1
lw	t0,	0(s1)
li	t1,	5
mul	a1,	t0,	t1

# a2
lw	t0,	0(s0)
addi	t0,	t0,	1
li	t1,	5
mul	t0,	t0,	t1
addi	a2,	t0,	-1

# a3
lw	t0,	0(s1)
addi	t0,	t0,	1
li	t1,	5
mul	t0,	t0,	t1
addi	a3,	t0,	-1

# a4 RED
li	a4,	0xF800

# LCD_Fill
jal	LCD_Fill

# a0
lw	t0,	0(s0)
li	t1,	5
mul	t0, t0, t1
addi	a0,	t0,	1

# a1
lw	t0,	0(s1)
li	t1,	5
mul	t0,	t0,	t1
addi	a1,	t0,	1

# a2
lw	t0,	0(s0)
addi	t0,	t0,	1
li	t1,	5
mul	t0,	t0,	t1
addi	a2,	t0,	-2

# a3
lw	t0,	0(s1)
addi	t0,	t0,	1
li	t1,	5
mul	t0,	t0,	t1
addi	a3,	t0,	-2

# a4 RED
li	a4,	0xFFFF

# LCD_Fill
jal	LCD_Fill

# For loop
li  s3, 1
draw_snake_body:

# a0
# get snakepos[i]
li  t0, 4
mul t0, s3, t0
add t0, t0, s0
lw	t0,	0(t0)

# mul 5
li	t1,	5
mul	a0, t0, t1

# a1
# get snakepos[i]
li  t0, 4
mul t0, s3, t0
add t0, t0, s1
lw	t0,	0(t0)

# mul 5
li	t1,	5
mul	a1,	t0,	t1

# a2
# get snakepos[i]
li  t0, 4
mul t0, s3, t0
add t0, t0, s0
lw	t0,	0(t0)
addi	t0,	t0,	1

# mul 5
li	t1,	5
mul	t0,	t0,	t1

# minus 1
addi	a2,	t0,	-1

# a3
# get snakepos[i]
li  t0, 4
mul t0, s3, t0
add t0, t0, s1
lw	t0,	0(t0)

# add 1
addi	t0,	t0,	1

# mul 5
li	t1,	5
mul	t0,	t0,	t1
addi	a3,	t0,	-1

# a4 BLUE
li	a4,	0x001F

# Fill
jal LCD_Fill

# a0
# get snakepos[i]
li  t0, 4
mul t0, s3, t0
add t0, t0, s0
lw	t0,	0(t0)

# mul 5
li	t1,	5
mul	a0, t0, t1

# add 1
addi a0, a0, 1

# a1
# get snakepos[i]
li  t0, 4
mul t0, s3, t0
add t0, t0, s1
lw	t0,	0(t0)

# mul 5
li	t1,	5
mul	a1,	t0,	t1

# add 1
addi a1, a1, 1

# a2
# get snakepos[i]
li  t0, 4
mul t0, s3, t0
add t0, t0, s0
lw	t0,	0(t0)
addi	t0,	t0,	1

# mul 5
li	t1,	5
mul	t0,	t0,	t1

# minus 2
addi	a2,	t0,	-2

# a3
# get snakepos[i]
li  t0, 4
mul t0, s3, t0
add t0, t0, s1
lw	t0,	0(t0)

# add 1
addi	t0,	t0,	1

# mul 5
li	t1,	5
mul	t0,	t0,	t1

# minus 2
addi	a3,	t0,	-2

# a4 WHITE
li	a4,	0xFFFF

# Fill
jal LCD_Fill

# i++
addi  s3, s3, 1


jal LCD_Fill

# load ra
lw  s6, 28(sp)
lw  s5, 24(sp)
lw  s4, 20(sp)
lw  s3, 16(sp)
lw  s2, 12(sp)
lw  s1, 8(sp)
lw  s0, 4(sp)
lw	ra,	0(sp)
addi	sp, sp, 32

# Return
ret
# end