.section .text
.globl SnakeInit
.type SnakeInit, @function

# a0: int *snakePosX
# a1: int *snakePosY
# a2: int *snakeLength
SnakeInit:
  # *snakeLength = 3
  li  t0, 3
  sw  t0, 0(a2)

  # Init snake posX 0
  li  t0, 16
  sw  t0, 0(a0)

  # Init snake posX 1
  li  t0, 15
  sw  t0, 4(a0)

  # Init snake posX 2
  li  t0, 14
  sw  t0, 8(a0)

  # Init snake posY 0
  li  t0, 8
  sw  t0, 0(a1)

  # Init snake posY 1
  li  t0, 8
  sw  t0, 4(a1)

  # Init snake posY 2
  li  t0, 8
  sw  t0, 8(a1)

  # return
  ret