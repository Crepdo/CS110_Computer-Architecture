.section .text
.globl SnakeUpdateFrontPos
.type SnakeUpdateFrontPos, @function

# a0: *snakePosX
# a1: *snakePosY
# a2: moveDirection
# a3: snakeFrontPosX
# a4: snakeFrontPosY
SnakeUpdateFrontPos:

# switch movedirection
# movedir = up
li	t0,	0
beq	t0,	a2,	moveDirection_up

# movedir = down
li	t0,	1
beq	t0,	a2,	moveDirection_down

# movedir = left
li	t0,	2
beq	t0,	a2,	moveDirection_left

# movedir = right
li	t0,	3
beq	t0,	a2,	moveDirection_right

# case Up
moveDirection_up:

# *snakeFrontPosX = snakePosX[0]
lw	t0,	0(a0)
sw	t0,	0(a3)

# *snakeFrontPosY = snakePosY[0] - 1
lw	t0,	0(a1)
addi	t0, t0, -1
sw	t0,	0(a4)

# break
j SnakeUpdateFrontPos_end

# case Down
moveDirection_down:

# *snakeFrontPosX = snakePosX[0]
lw	t0,	0(a0)
sw	t0,	0(a3)

# *snakeFrontPosY = snakePosY[0] + 1
lw	t0,	0(a1)
addi	t0, t0, 1
sw	t0,	0(a4)

# break
j SnakeUpdateFrontPos_end

# case Left
moveDirection_left:

# *snakeFrontPosX = snakePosX[0] -1
lw	t0,	0(a0)
addi	t0, t0, -1
sw	t0,	0(a3)

# *snakeFrontPosY = snakePosY[0]
lw	t0,	0(a1)
sw	t0,	0(a4)

# break
j SnakeUpdateFrontPos_end

# case Right
moveDirection_right:

# *snakeFrontPosX = snakePosX[0] + 1
lw	t0,	0(a0)
addi	t0, t0, 1
sw	t0,	0(a3)

# *snakeFrontPosY = snakePosY[0]
lw	t0,	0(a1)
sw	t0,	0(a4)

# break
j SnakeUpdateFrontPos_end

# end
SnakeUpdateFrontPos_end:

# Return
ret
# end