.data

# Constant integer specifying the lines of machine codes
.globl lines_of_machine_codes
lines_of_machine_codes:
    .word 3

# Store machine codes to be converted
.globl machine_codes
machine_codes:
    .word 0x00008067    # jalr
    .word 0x7FF28293    # addi t0,t0,2047
    .word 0x7FF2E293    # ori t0,t0,2047

# Temp valuable for recording converted register name
reg_name:       .asciiz "000"

# R type
# inst name for opcode_0x33
inst_add_name:  .asciiz "add"
inst_or_name:   .asciiz "or"
inst_slt_name:  .asciiz "slt"
inst_sltu_name: .asciiz "sltu"
inst_sll_name:  .asciiz "sll"

# I type
# inst name for opcode_0x67
inst_jalr_name: .asciiz "jalr"

# inst name for opcode_0x13
inst_addi_name: .asciiz "addi"
inst_ori_name:  .asciiz "ori"

# I type load
# inst name for opcode_0x03
inst_lb_name:   .asciiz "lb"
inst_lbu_name:  .asciiz "lbu"
inst_lw_name:   .asciiz "lw"

# S type
# inst name for opcode_0x23
inst_sb_name:   .asciiz "sb"
inst_sw_name:   .asciiz "sw"

# B type
# inst name for opcode_0x63
inst_beq_name:  .asciiz "beq"
inst_bne_name:  .asciiz "bne"
inst_blt_name:  .asciiz "blt"
inst_bge_name:  .asciiz "bge"

# UJ type
# inst name for opcode_0x6f
inst_jal_name:  .asciiz "jal"

# U type
# inst name for opcode_0x37
inst_lui_name:  .asciiz "lui"

.text
.globl main
# Function Name:        main
# Argument:             null
# Return value:         null
main:
        call    disassemble_lines

end:
        # Exit with 0
	add	a1,	x0,	x0
	addi	a0,	x0,	17
	ecall


# Function Name:        disassemble_lines
# Argument:             null
# Return value:         null
disassemble_lines:
        # Save ra, s0, s1 because those will be changed
        addi    sp,     sp,     -12
        sw      ra,     0(sp)
        sw      s0,     4(sp)
        sw      s1,     8(sp)

        # Load lines of code to s0
        la      s0,     lines_of_machine_codes
        lw      s0,     0(s0)

        # Use s1 as counter
        li      s1,     0

disassemble_next_line:
        # disassemble one line
        mv      a0,     s1
        call    disassemble_one_line

        # Loop while counter(s0) != lines_of_machine_codes(s1)
        addi    s1,     s1,     1
        bne     s0,     s1,     disassemble_next_line

disassemble_line_end:
        # Load ra, s0, s1
        lw      s1,     8(sp)
        lw      s0,     4(sp)
        lw      ra,     0(sp)
        addi    sp,     sp,     12

        # Return
        jr      ra


# Function Name:        disassemble_one_line
# Argument:             a0 = line_num
# Return value:         null
disassemble_one_line:
        # Save ra, s0, s1 because those will be changed
        addi    sp,     sp,     -28
        sw      ra,     0(sp)
        sw      s0,     4(sp)
        sw      s1,     8(sp)
        sw      s2,     12(sp)
        sw      s3,     16(sp)
        sw      s4,     20(sp)
        sw      s5,     24(sp)

        # Get current line and save into s0
        call    get_machine_code
        mv      s0,     a0

        # Get opcode and store in s1
        andi    s1,     s0,     0x0000007f

opcode_0x33:
        # R type: add/or/slt/sltu/sll
        li      t0,     0x33
        bne     s1,     t0,     opcode_0x67

        # Get rd_raw and store in s2
        srli    s2,     s0,     7
        andi    s2,     s2,     0x1f

        # Get func3 and store in s3
        srli    s3,     s0,     12
        andi    s3,     s3,     0x07

        # Get rs1_raw and store in s4
        srli    s4,     s0,     15
        andi    s4,     s4,     0x1f

        # Get rs2_raw and store in s5
        srli    s5,     s0,     20
        andi    s5,     s5,     0x1f

inst_add:
        # func3
        li      t0,     0x0
        bne     s3,     t0,     inst_or

        # Print
        la      a1,     inst_add_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x33_get_inst_end

inst_or:
        # func3
        li      t0,     0x6
        bne     s3,     t0,     inst_slt

        # Print
        la      a1,     inst_or_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x33_get_inst_end

inst_slt:
        # func3
        li      t0,     0x2
        bne     s3,     t0,     inst_sltu

        # Print "slt"
        la      a1,     inst_slt_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x33_get_inst_end

inst_sltu:
        # func3
        li      t0,     0x3
        bne     s3,     t0,     inst_sll

        # Print
        la      a1,     inst_sltu_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x33_get_inst_end

inst_sll:
        # func3
        li      t0,     0x1
        bne     s3,     t0,     opcode_0x33_get_inst_end

        # Print
        la      a1,     inst_sll_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x33_get_inst_end

opcode_0x33_get_inst_end:
        # Print " "
        li      a1,     ' '
        li      a0,     11
        ecall
        
        # Get rd name
        mv      a0,     s2
        call    get_reg_name

        # Print rd
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ","
        li      a1,     ','
        li      a0,     11
        ecall

        # Get rs1 name
        mv      a0,     s4
        call    get_reg_name

        #Print rs1
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ","
        li      a1,     ','
        li      a0,     11
        ecall

        # Get rs2 name
        mv      a0,     s5
        call    get_reg_name

        #Print rs2
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print "\n"
        li      a1,     '\n'
        li      a0,     11
        ecall

        j       disassemble_one_line_end

opcode_0x67:
        # I type: jalr
        li      t0,     0x67
        bne     s1,     t0,     opcode_0x13

        # Get rd_raw and store in s2
        srli    s2,     s0,     7
        andi    s2,     s2,     0x1f

        # Get func3 and store in s3
        srli    s3,     s0,     12
        andi    s3,     s3,     0x07

        # Get rs1_raw and store in s4
        srli    s4,     s0,     15
        andi    s4,     s4,     0x1f

        # Get imm_0_11_raw and store in s5
        srli    s5,     s0,     20

        # Get imm_0_11 from imm_0_11_raw and store in s5
        mv      a0,     s5
        addi    a1,     x0,     12
        call    get_immediate_signed
        mv      s5,     a0

inst_jalr:
        # Print instruction name
        la      a1,     inst_jalr_name
        li      a0,     4
        ecall

opcode_0x67_get_inst_end:
        # Print " "
        li      a1,     ' '
        li      a0,     11
        ecall

        # Get rd name
        mv      a0,     s2
        call    get_reg_name

        # Print rd
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ","
        li      a1,     ','
        li      a0,     11
        ecall

        # Get rs1 name
        mv      a0,     s4
        call    get_reg_name

        # Print rs1
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ","
        li      a1,     ','
        li      a0,     11
        ecall

        # Print immediate
        mv      a1,     s5
        li      a0,     1
        ecall

        # Print "\n"
        li      a1,     '\n'
        li      a0,     11
        ecall

        j       disassemble_one_line_end

opcode_0x13:
        # I type: addi/ori
        li      t0,     0x13
        bne     s1,     t0,     opcode_0x03

        # Get rd_raw and store in s2
        srli    s2,     s0,     7
        andi    s2,     s2,     0x1f

        # Get func3 and store in s3
        srli    s3,     s0,     12
        andi    s3,     s3,     0x07

        # Get rs1_raw and store in s4
        srli    s4,     s0,     15
        andi    s4,     s4,     0x1f

        # Get imm_0_11_raw and store in s5
        srli    s5,     s0,     20

        # Get imm_0_11 from imm_0_11_raw and store in s5
        mv      a0,     s5
        addi    a1,     x0,     12
        call    get_immediate_signed
        mv      s5,     a0

inst_addi:
        # func3
        li      t0,     0x0
        bne     s3,     t0,     inst_ori

        # Print
        la      a1,     inst_addi_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x13_get_inst_end

inst_ori:
        # func3
        li      t0,     0x6
        bne     s3,     t0,     opcode_0x13_get_inst_end

        # Print
        la      a1,     inst_ori_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x13_get_inst_end

opcode_0x13_get_inst_end:
        # Print " "
        li      a1,     ' '
        li      a0,     11
        ecall

        # Get rd name
        mv      a0,     s2
        call    get_reg_name

        # Print rd
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ","
        li      a1,     ','
        li      a0,     11
        ecall

        # Get rs1 name
        mv      a0,     s4
        call    get_reg_name

        # Print rs1
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ","
        li      a1,     ','
        li      a0,     11
        ecall

        # Print immediate
        mv      a1,     s5
        li      a0,     1
        ecall

        # Print "\n"
        li      a1,     '\n'
        li      a0,     11
        ecall

        j       disassemble_one_line_end

opcode_0x03:
        # I type: lb/lbu/lw
        li      t0,     0x03
        bne     s1,     t0,     opcode_0x23

        # Get rd_raw and store in s2
        srli    s2,     s0,     7
        andi    s2,     s2,     0x1f

        # Get func3 and store in s3
        srli    s3,     s0,     12
        andi    s3,     s3,     0x07

        # Get rs1_raw and store in s4
        srli    s4,     s0,     15
        andi    s4,     s4,     0x1f

        # Get imm_0_11_raw and store in s5
        srli    s5,     s0,     20

        # Get imm_0_11 from imm_0_11_raw and store in s5
        mv      a0,     s5
        addi    a1,     x0,     12
        call    get_immediate_signed
        mv      s5,     a0

inst_lb:
        # func3
        li      t0,     0x0
        bne     s3,     t0,     inst_lbu

        # Print
        la      a1,     inst_lb_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x03_get_inst_end

inst_lbu:
        # func3
        li      t0,     0x4
        bne     s3,     t0,     inst_lw

        # Print
        la      a1,     inst_lbu_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x03_get_inst_end

inst_lw:
        # func3
        li      t0,     0x2
        bne     s3,     t0,     opcode_0x03_get_inst_end

        # Print
        la      a1,     inst_lw_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x03_get_inst_end

opcode_0x03_get_inst_end:
        # Print " "
        li      a1,     ' '
        li      a0,     11
        ecall

        # Get rd name
        mv      a0,     s2
        call    get_reg_name

        # Print rd
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ","
        li      a1,     ','
        li      a0,     11
        ecall

        # Print immediate
        mv      a1,     s5
        li      a0,     1
        ecall

        # Print "("
        li      a1,     40
        li      a0,     11
        ecall

        # Get rs1 name
        mv      a0,     s4
        call    get_reg_name

        # Print rs1
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ")"
        li      a1,     ')'
        li      a0,     11
        ecall

        # Print "\n"
        li      a1,     '\n'
        li      a0,     11
        ecall

        j       disassemble_one_line_end

opcode_0x23:
        # S type: sb/sw
        li      t0,     0x23
        bne     s1,     t0,     opcode_0x63

        # Get imm_0_11_raw and store in s2
        srli    s2,     s0,     7
        andi    s2,     s2,     0x1f

        # Get func3 and store in s3
        srli    s3,     s0,     12
        andi    s3,     s3,     0x07

        # Get rs1_raw and store in s4
        srli    s4,     s0,     15
        andi    s4,     s4,     0x1f

        # Get rs2_raw and store in s5
        srli    s5,     s0,     20
        andi    s5,     s5,     0x1f

        # imm_0_11_raw += (current_line >> 25) << 5
        srli    t0,     s0,     25
        slli    t0,     t0,     5
        add     s2,     s2,     t0

        # Get imm_0_11 from imm_0_11_raw and store in s2
        mv      a0,     s2
        addi    a1,     x0,     12
        call    get_immediate_signed
        mv      s2,     a0

inst_sb:
        # func3
        li      t0,     0x0
        bne     s3,     t0,     inst_sw

        # Print
        la      a1,     inst_sb_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x23_get_inst_end

inst_sw:
        # func3
        li      t0,     0x2
        bne     s3,     t0,     opcode_0x23_get_inst_end

        # Print
        la      a1,     inst_sw_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x23_get_inst_end

opcode_0x23_get_inst_end:
        # Print " "
        li      a1,     ' '
        li      a0,     11
        ecall

        # Get rs2 name
        mv      a0,     s5
        call    get_reg_name

        # Print rs2
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ","
        li      a1,     ','
        li      a0,     11
        ecall

        # Print imm_0_11
        mv      a1,     s2
        li      a0,     1
        ecall

        # Print "("
        li      a1,     40
        li      a0,     11
        ecall

        # Get rs1 name
        mv      a0,     s4
        call    get_reg_name

        # Print rs1
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ")"
        li      a1,     ')'
        li      a0,     11
        ecall

        # Print "\n"
        li      a1,     '\n'
        li      a0,     11
        ecall

        # Skip to end
        j       disassemble_one_line_end


opcode_0x63:
        # SB type: beq/bne/blt/bge
        li      t0,     0x63
        bne     s1,     t0,     opcode_0x6f

        # Get imm_0_12_raw and store in s2
        srli    s2,     s0,     7
        andi    s2,     s2,     0x01
	slli	s2,	s2,	11

        # imm_0_12_raw += ((current_line >> 8) & 0x0f) << 1;
        srli	t0,	s0,	8
	andi	t0,	t0,	0x0f
	slli	t0,	t0,	1
	add	s2,	s2,	t0

        # Get func3 and store in s3
        srli    s3,     s0,     12
        andi    s3,     s3,     0x07

        # Get rs1_raw and store in s4
        srli    s4,     s0,     15
        andi    s4,     s4,     0x1f

        # Get rs2_raw and store in s5
        srli    s5,     s0,     20
        andi    s5,     s5,     0x1f

	# imm_0_12_raw += ((current_line >> 25) & 0x3f) << 5;
	srli	t0,	s0,	25
	andi	t0,	t0,	0x3f
	slli	t0,	t0,	5
	add	s2,	s2,	t0

	# imm_0_12_raw += ((current_line >> 31) & 0x01) << 12
	srli	t0,	s0,	31
	andi	t0,	t0,	0x01
	slli	t0,	t0,	12
	add	s2,	s2,	t0

	# Get imm_0_12 from imm_o_12_raw and store in s2
	mv      a0,     s2
        addi    a1,     x0,     13
        call    get_immediate_signed
        mv      s2,     a0

inst_beq:
        # func3
        li      t0,     0x0
        bne     s3,     t0,     inst_bne

        # Print
        la      a1,     inst_beq_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x63_get_inst_end

inst_bne:
        # func3
        li      t0,     0x1
        bne     s3,     t0,     inst_blt

        # Print
        la      a1,     inst_bne_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x63_get_inst_end

inst_blt:
        # func3
        li      t0,     0x4
        bne     s3,     t0,     inst_bge

        # Print
        la      a1,     inst_blt_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x63_get_inst_end

inst_bge:
        # func3
        li      t0,     0x5
        bne     s3,     t0,     opcode_0x63_get_inst_end

        # Print
        la      a1,     inst_bge_name
        li      a0,     4
        ecall

        # Return
        j       opcode_0x63_get_inst_end

opcode_0x63_get_inst_end:
        # Print " "
        li      a1,     ' '
        li      a0,     11
        ecall

        # Get rs1 name
        mv      a0,     s4
        call    get_reg_name

        # Print rs1
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ","
        li      a1,     ','
        li      a0,     11
        ecall

        # Get rs2 name
        mv      a0,     s5
        call    get_reg_name

        # Print rs2
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ","
        li      a1,     ','
        li      a0,     11
        ecall

        # Print imm_0_12
        mv      a1,     s2
        li      a0,     1
        ecall

        # Print "\n"
        li      a1,     '\n'
        li      a0,     11
        ecall

        # Skip to end
        j       disassemble_one_line_end

opcode_0x6f:
        # UJ type: jal
        li      t0,     0x6f
        bne     s1,     t0,     opcode_0x37

        # Get rd_raw and store in s2
        srli    s2,     s0,     7
        andi    s2,     s2,     0x1f

        # Get imm_0_20_raw and store in s3
        srli    s3,     s0,     12
        andi    s3,     s3,     0xff
	slli	s3,	s3,	12

        # imm_0_20_raw += ((current_line >> 20) & 0x01) << 11
        srli    t0,     s0,     20
        andi    t0,     t0,     0x01
        slli    t0,     t0,     11
        add     s3,     t0,     s3

        # imm_0_20_raw += ((current_line >> 21) & 0x3ff) << 1
        srli    t0,     s0,     21
        andi    t0,     t0,     0x3ff
        slli    t0,     t0,     1
        add     s3,     t0,     s3

        # imm_0_20_raw += ((current_line >> 31) & 0x01) << 20
        srli    t0,     s0,     31
        andi    t0,     t0,     0x01
        slli    t0,     t0,     20
        add     s3,     t0,     s3

	# Get imm_0_20 from imm_0_20_raw and store in s3
	mv      a0,     s3
        addi    a1,     x0,     13
        call    get_immediate_signed
        mv      s3,     a0

inst_jal:
        # Print
        la      a1,     inst_jal_name
        li      a0,     4
        ecall

opcode_0x6f_get_inst_end:
        # Print " "
        li      a1,     ' '
        li      a0,     11
        ecall

        # Get rd name
        mv      a0,     s2
        call    get_reg_name

        # Print rd
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ","
        li      a1,     ','
        li      a0,     11
        ecall

        # Print imm_0_20
        mv      a1,     s3
        li      a0,     1
        ecall

        # Print "\n"
        li      a1,     '\n'
        li      a0,     11
        ecall

        # Skip to end
        j       disassemble_one_line_end

opcode_0x37:
        # U type: lui
        li      t0,     0x37
        bne     s1,     t0,     disassemble_one_line_end

	# Get rd_raw and store in s2
        srli    s2,     s0,     7
        andi    s2,     s2,     0x1f

	# Get imm_12_31_raw and store in s3
        srli    s3,     s0,     12

inst_lui:
        # Print
        la      a1,     inst_lui_name
        li      a0,     4
        ecall

opcode_0x37_get_inst_end:
        # Print " "
        li      a1,     ' '
        li      a0,     11
        ecall

        # Get rd name
        mv      a0,     s2
        call    get_reg_name

        # Print rd
        la      a1,     reg_name
        li      a0,     4
        ecall

        # Print ","
        li      a1,     ','
        li      a0,     11
        ecall

        # Print imm_12_31
        mv      a1,     s3
        li      a0,     1
        ecall

        # Print "\n"
        li      a1,     '\n'
        li      a0,     11
        ecall

        # Skip to end
        j       disassemble_one_line_end

disassemble_one_line_end:
        # Load ra, s0, s1
        lw      s5,     24(sp)
        lw      s4,     20(sp)
        lw      s3,     16(sp)
        lw      s2,     12(sp)
        lw      s1,     8(sp)
        lw      s0,     4(sp)
        lw      ra,     0(sp)
        addi    sp,     sp,     28
        
        # Return
        jr      ra


# Function Name:        get_machine_code
# Argument:             a0 = line_num
# Return value:         a0 = machinecode
get_machine_code:
        # Load address of machine_codes
        la      t0,     machine_codes

get_machine_code_loop:
        # while(--a0>=0)
        addi    a0,     a0,     -1
        blt     a0,     x0,     get_machine_code_end

        # address(t0) += 4
        addi    t0,     t0,     4

        # Loop to increase address
        j       get_machine_code_loop

get_machine_code_end:
        # Store machine code in a0
        lw      a0,     0(t0)

        # Return
        jr      ra


# Function Name:        get_immediate_signed
# Argument:             a0 = imm_raw
#                       a1 = bit_num
# Return value:         a0 = Converted value
get_immediate_signed:

        # t0 = bit_num(a1) - 1
        addi    t0,     a1,     -1
        
        # t0 = imm_raw >> t0
        srl     t0,     a0,     t0    

        # If imm_raw >> (bit_num - 1) == 0 then don't do anything return imm_raw directly
        li      t1,     0
        beq     t0,     t1,     get_immediate_signed_end

get_negative_num:
        # num = -(((~imm_raw + 1) << (32 - bit_num)) >> (32 - bit_num))
        # t0 = 32 - bit_num
        addi    t0,     x0,     32
        neg     t1,     a1
        add     t0,     t0,     t1

        # t1 = ~imm_raw + 1
        not     t1,     a0
        addi    t1,     t1,     1

        # (t1 << t0) >> t0
        sll     t1,     t1,     t0
        srl     t1,     t1,     t0

        # num = -t1
        neg     a0,     t1

get_immediate_signed_end:
        # Return
        jr      ra


# Function Name:        get_reg_name
# Argument:             a0 = Register number to be converted
# Return value:         null
get_reg_name:
detect_x0:
        # x0
        li      t0,     0
        bne     a0,     t0,     detect_ra

        # Get reg_name address
        la      t1,     reg_name  

        # Modify reg_name
        li      t0,     'x'  
        sb      t0,     0(t1)
        li      t0,     '0'  
        sb      t0,     1(t1)

        # Fill last byte
        li      t0,     0  
        sb      t0,     2(t1)

        # Jump to end of function
        j       get_reg_name_end

detect_ra:
        # ra
        li      t0,     1
        bne     a0,     t0,     detect_sp

        # Get reg_name address
        la      t1,     reg_name  

        # Modify reg_name
        li      t0,     'r'  
        sb      t0,     0(t1)
        li      t0,     'a'
        sb      t0,     1(t1)

        # Fill last byte
        li      t0,     0  
        sb      t0,     2(t1)

        # Jump to end of function
        j       get_reg_name_end

detect_sp:
        # sp
        li      t0,     2
        bne     a0,     t0,     detect_gp

        # Get reg_name address
        la      t1,     reg_name  

        # Modify reg_name
        li      t0,     's'  
        sb      t0,     0(t1)
        li      t0,     'p'  
        sb      t0,     1(t1)

        # Fill last byte
        li      t0,     0  
        sb      t0,     2(t1)

        # Jump to end of function
        j       get_reg_name_end

detect_gp:
        # gp
        li      t0,     3
        bne     a0,     t0,     detect_tp

        # Get reg_name address
        la      t1,     reg_name  

        # Modify reg_name
        li      t0,     'g'  
        sb      t0,     0(t1)
        li      t0,     'p'  
        sb      t0,     1(t1)

        # Fill last byte
        li      t0,     0  
        sb      t0,     2(t1)

        # Jump to end of function
        j       get_reg_name_end

detect_tp:
        # tp
        li      t0,     4
        bne     a0,     t0,     detect_t_0_2

        # Get reg_name address
        la      t1,     reg_name  

        # Modify reg_name
        li      t0,     't'  
        sb      t0,     0(t1)
        li      t0,     'p'  
        sb      t0,     1(t1)

        # Fill last byte
        li      t0,     0  
        sb      t0,     2(t1)

        # Jump to end of function
        j       get_reg_name_end

detect_t_0_2:
        # t(reg_num-5)
        li      t0,     8
        bge     a0,     t0,     detect_s_0_1

        # Get reg_name address
        la      t1,     reg_name  

        # Modify reg_name
        li      t0,     't'  
        sb      t0,     0(t1)

        # Calculate reg num
        addi    t0,     a0,     '0'
        addi    t0,     t0,     -5
        sb      t0,     1(t1)

        # Fill last byte
        li      t0,     0  
        sb      t0,     2(t1)

        # Jump to end of function
        j       get_reg_name_end

detect_s_0_1:
        # s(reg_num-8)
        li      t0,     10
        bge     a0,     t0,     detect_a_0_7

        # Get reg_name address
        la      t1,     reg_name  

        # Modify reg_name
        li      t0,     's'  
        sb      t0,     0(t1)

        # Calculate reg num
        addi    t0,     a0,     '0'
        addi    t0,     t0,     -8
        sb      t0,     1(t1)

        # Fill last byte
        li      t0,     0  
        sb      t0,     2(t1)

        # Jump to end of function
        j       get_reg_name_end

detect_a_0_7:
        # a(reg_num-10)
        li      t0,     18
        bge     a0,     t0,     detect_s_2_9

        # Get reg_name address
        la      t1,     reg_name  

        # Modify reg_name
        li      t0,     'a'  
        sb      t0,     0(t1)

        # Calculate reg num
        addi    t0,     a0,     '0'
        addi    t0,     t0,     -10
        sb      t0,     1(t1)

        # Fill last byte
        li      t0,     0  
        sb      t0,     2(t1)

        # Jump to end of function
        j       get_reg_name_end

detect_s_2_9:
        # s(reg_num-16)
        li      t0,     26
        bge     a0,     t0,     detect_s_10_11

        # Get reg_name address
        la      t1,     reg_name  

        # Modify reg_name
        li      t0,     's'  
        sb      t0,     0(t1)

        # Calculate reg num
        addi    t0,     a0,     '0'
        addi    t0,     t0,     -16
        sb      t0,     1(t1)

        # Fill last byte
        li      t0,     0  
        sb      t0,     2(t1)

        # Jump to end of function
        j       get_reg_name_end

detect_s_10_11:
        # s(reg_num-16)
        li      t0,     28
        bge     a0,     t0,     detect_t_3_6

        # Get reg_name address
        la      t1,     reg_name  

        # Modify reg_name
        li      t0,     's'  
        sb      t0,     0(t1)

        # Calculate reg num 1
        li      t0,     '1'  
        sb      t0,     1(t1)

        # Calculate reg num 2
        addi    t0,     a0,     '0'
        addi    t0,     t0,     -26
        sb      t0,     2(t1)

        # Jump to end of function
        j       get_reg_name_end

detect_t_3_6:
        # t(reg_num-25)
        li      t0,     32
        bge     a0,     t0,     get_reg_name_end

        # Get reg_name address
        la      t1,     reg_name  

        # Modify reg_name
        li      t0,     't'  
        sb      t0,     0(t1)

        # Fill last byte
        li      t0,     0  
        sb      t0,     2(t1)

        # Calculate reg num
        addi    t0,     a0,     '0'
        addi    t0,     t0,     -25
        sb      t0,     1(t1)

get_reg_name_end:
        jr      ra
