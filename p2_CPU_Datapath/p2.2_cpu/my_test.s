addi t0,t0,0x00000000
addi t1,t1,0x0000000f
sltu t0,t0,t1
sra t0,t0,t1
remu t0,t0,t1
and t0,t0,t1