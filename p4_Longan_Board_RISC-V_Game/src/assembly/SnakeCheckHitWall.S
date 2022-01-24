.section .text
.globl SnakeCheckHitWall
.type SnakeCheckHitWall, @function

# a0: snakeFrontPosX
# a1: snakeFrontPosY
# ret
# a0: bool
SnakeCheckHitWall:

# check x pos
check_x_pos:

# snakeFrontPosX < 0
bltz	a0,	return_true

# snakeFrontPosX > MAP_WIDTH - 1
li	t0,	32
bge	a0,	t0,	return_true

# check y pos
check_y_pos:

# snakeFrontPosY < 0
bltz	a1,	return_true

# snakeFrontPosY > MAP_HEIGHT - 1
li	t0,	16
bge	a1,	t0,	return_true

# return false
return_false:
li	a0,	0
ret

# return true
return_true:
li	a0,	1
ret