#include <stdio.h>
#include <stdlib.h>

unsigned int lines_of_machine_codes = 10;
unsigned int machine_codes[10] = {
    0x000502B3, // add  t0, a0, x0
    0x00100313, // addi t1, x0, 1
    0x00028863, // beq  t0, x0, 16
    0x01DE13B3, // sll  t2, t3, t4
    0xFFF28293, // addi t0, t0, -1
    0xFF5FF06F, // jal  x0, -12
    0x00600533, // add  a0, x0, t1
    0x00008067, // jalr x0, ra, 0
    0x00530223, // sb t0, 4(t1)
    0xFFFFF2B7  // lui t0, 0xfffff
};

char reg_name[2] = {0};

void get_reg_name(unsigned int reg_num)
{
        if (reg_num == 0)
        {
                // x0
                reg_name[0] = 'x';
                reg_name[1] = '0';
        }
        else if (reg_num == 1)
        {
                // ra
                reg_name[0] = 'r';
                reg_name[1] = 'a';
        }
        else if (reg_num == 2)
        {
                // sp
                reg_name[0] = 's';
                reg_name[1] = 'p';
        }
        else if (reg_num == 3)
        {
                // gp
                reg_name[0] = 'g';
                reg_name[1] = 'p';
        }
        else if (reg_num == 4)
        {
                // tp
                reg_name[0] = 't';
                reg_name[1] = 'p';
        }
        else if (reg_num >= 28)
        {
                // t(reg_num-25)
                reg_name[0] = 't';
                reg_name[1] = reg_num - 25 + '0';
        }
        else if (reg_num >= 18)
        {
                // s(reg_num-16)
                reg_name[0] = 's';
                reg_name[1] = reg_num - 16 + '0';
        }
        else if (reg_num >= 10)
        {
                // a(reg_num-10)
                reg_name[0] = 'a';
                reg_name[1] = reg_num - 10 + '0';
        }
        else if (reg_num >= 8)
        {
                // s(reg_num-8)
                reg_name[0] = 's';
                reg_name[1] = reg_num - 8 + '0';
        }
        else if (reg_num >= 5)
        {
                // t(reg_num-5)
                reg_name[0] = 't';
                reg_name[1] = reg_num - 5 + '0';
        }
}

int get_immediate_signed(unsigned int imm_raw, unsigned int bit_num)
{
        int immediate = 0;
        if (imm_raw >> (bit_num - 1) == 1) // Negative number
                return -(((~imm_raw + 1) << (32 - bit_num)) >> (32 - bit_num));
        else // Positive number
                return imm_raw;
}

void read_one_line(unsigned int line_num)
{
        // Get current line
        unsigned int current_line = machine_codes[line_num];

        // Read opcode
        unsigned int opcode = current_line & 0x0000007f;

        // Print instruction name
        if (opcode == 0x33)
        {
                // R type: add/or/slt/sltu/sll
                unsigned int rd_raw = (current_line >> 7) & 0x1f;
                unsigned int func3 = (current_line >> 12) & 0x07;
                unsigned int rs1_raw = (current_line >> 15) & 0x1f;
                unsigned int rs2_raw = (current_line >> 20) & 0x1f;

                // Print instruction name
                if (func3 == 0x0)
                {
                        // add
                        printf("add");
                }
                else if (func3 == 0x6)
                {
                        // or
                        printf("or");
                }
                else if (func3 == 0x2)
                {
                        // slt
                        printf("slt");
                }
                else if (func3 == 0x3)
                {
                        // sltu
                        printf("sltu");
                }
                else if (func3 == 0x1)
                {
                        // sll
                        printf("sll");
                }
                printf(" ");

                // Print rd
                get_reg_name(rd_raw);
                printf("%s", reg_name);
                printf(",");

                // Print rs1
                get_reg_name(rs1_raw);
                printf("%s", reg_name);
                printf(",");

                // Print rs2
                get_reg_name(rs2_raw);
                printf("%s", reg_name);
                printf("\n");
        }
        else if (opcode == 0x67)
        {
                // I type: jalr
                unsigned int rd_raw = (current_line >> 7) & 0x1f;
                unsigned int func3 = (current_line >> 12) & 0x07;
                unsigned int rs1_raw = (current_line >> 15) & 0x1f;
                unsigned int imm_0_11_raw = (current_line >> 20);

                // Generate immediate
                int imm_0_11 = get_immediate_signed(imm_0_11_raw, 12);

                // Print instruction name
                printf("jalr");
                printf(" ");

                // Print rd
                get_reg_name(rd_raw);
                printf("%s", reg_name);
                printf(",");

                // Print rs1
                get_reg_name(rs1_raw);
                printf("%s", reg_name);
                printf(",");

                // Print imm[11,0]
                printf("%d", imm_0_11);
                printf("\n");
        }
        else if (opcode == 0x13)
        {
                // I type: addi/ori
                unsigned int rd_raw = (current_line >> 7) & 0x1f;
                unsigned int func3 = (current_line >> 12) & 0x07;
                unsigned int rs1_raw = (current_line >> 15) & 0x1f;
                unsigned int imm_0_11_raw = (current_line >> 20);

                // Generate immediate
                int imm_0_11 = get_immediate_signed(imm_0_11_raw, 12);

                // Print instruction name
                if (func3 == 0x0)
                {
                        // addi
                        printf("addi");
                }
                else if (func3 == 0x6)
                {
                        // ori
                        printf("ori");
                }
                printf(" ");

                // Print rd
                get_reg_name(rd_raw);
                printf("%s", reg_name);
                printf(",");

                // Print rs1
                get_reg_name(rs1_raw);
                printf("%s", reg_name);
                printf(",");

                // Print imm[11,0]
                printf("%d", imm_0_11);
                printf("\n");
        }
        else if (opcode == 0x03)
        {
                // I type: lb/lbu/lw
                unsigned int rd_raw = (current_line >> 7) & 0x1f;
                unsigned int func3 = (current_line >> 12) & 0x07;
                unsigned int rs1_raw = (current_line >> 15) & 0x1f;
                unsigned int imm_0_11_raw = (current_line >> 20);

                // Generate immediate
                int imm_0_11 = get_immediate_signed(imm_0_11_raw, 12);

                // Print instruction name
                if (func3 == 0x0)
                {
                        // lb
                        printf("lb");
                }
                else if (func3 == 0x4)
                {
                        // lbu
                        printf("lbu");
                }
                else if (func3 == 0x2)
                {
                        // lw
                        printf("lw");
                }
                printf(" ");

                // Print rd
                get_reg_name(rd_raw);
                printf("%s", reg_name);
                printf(",");

                // Print imm[11,0]
                printf("%d", imm_0_11);
                printf("(");

                // Print rs1
                get_reg_name(rs1_raw);
                printf("%s", reg_name);
                printf(")");
                printf("\n");


        }
        else if (opcode == 0x23)
        {
                // S type: sb/sw
                unsigned int imm_0_11_raw = (current_line >> 7) & 0x1f;
                unsigned int func3 = (current_line >> 12) & 0x07;
                unsigned int rs1_raw = (current_line >> 15) & 0x1f;
                unsigned int rs2_raw = (current_line >> 20) & 0x1f;
                imm_0_11_raw += (current_line >> 25) << 5;

                // Generate immediate
                int imm_0_11 = get_immediate_signed(imm_0_11_raw, 12);

                // Print instruction name
                if (func3 == 0x0)
                {
                        // sb
                        printf("sb");
                }
                else if (func3 == 0x2)
                {
                        // sw
                        printf("sw");
                }
                printf(" ");

                // Print rs2
                get_reg_name(rs2_raw);
                printf("%s", reg_name);
                printf(",");

                // Print imm[11,0] (offset)
                printf("%d", imm_0_11);
                printf("(");

                // Print rs1
                get_reg_name(rs1_raw);
                printf("%s", reg_name);
                printf(")\n");
        }
        else if (opcode == 0x63)
        {
                // SB type: beq/bne/blt/bge
                unsigned int imm_0_12_raw = ((current_line >> 7) & 0x01) << 11;
                imm_0_12_raw += ((current_line >> 8) & 0x0f) << 1;
                unsigned int func3 = (current_line >> 12) & 0x07;
                unsigned int rs1_raw = (current_line >> 15) & 0x1f;
                unsigned int rs2_raw = (current_line >> 20) & 0x1f;
                imm_0_12_raw += ((current_line >> 25) & 0x3f) << 5;
                imm_0_12_raw += ((current_line >> 31) & 0x01) << 12;

                // Generate immediate
                int imm_0_12 = get_immediate_signed(imm_0_12_raw, 13);

                // Print instruction name
                if (func3 == 0x0)
                {
                        // beq
                        printf("beq");
                }
                else if (func3 == 0x1)
                {
                        // bne
                        printf("bne");
                }
                else if (func3 == 0x4)
                {
                        // blt
                        printf("blt");
                }
                else if (func3 == 0x5)
                {
                        // bge
                        printf("bge");
                }
                printf(" ");

                // Print rs1
                get_reg_name(rs1_raw);
                printf("%s", reg_name);
                printf(",");

                // Print rs2
                get_reg_name(rs2_raw);
                printf("%s", reg_name);
                printf(",");

                // Print immediate
                printf("%d", imm_0_12);
                printf("\n");
        }
        else if (opcode == 0x6f)
        {
                // UJ type: jal
                unsigned int rd_raw = (current_line >> 7) & 0x1f;
                unsigned int imm_0_20_raw = ((current_line >> 12) & 0xff) << 12;
                imm_0_20_raw += ((current_line >> 20) & 0x01) << 11;
                imm_0_20_raw += ((current_line >> 21) & 0x3ff) << 1;
                imm_0_20_raw += ((current_line >> 31) & 0x01) << 20;

                // Generate immediate
                int imm_0_20 = get_immediate_signed(imm_0_20_raw, 21);

                // Print instruction name
                printf("jal");
                printf(" ");

                // Print rd
                get_reg_name(rd_raw);
                printf("%s", reg_name);
                printf(",");

                // Print immediate
                printf("%d", imm_0_20);
                printf("\n");
        }
        else if (opcode == 0x37)
        {
                // U type: lui
                unsigned int rd_raw = (current_line >> 7) & 0x1f;
                unsigned int imm_12_31 = (current_line >> 12);

                // Print instruction name
                printf("lui");
                printf(" ");

                // Print rd
                get_reg_name(rd_raw);
                printf("%s", reg_name);
                printf(",");

                // Print immediate
                printf("%u", imm_12_31);
                printf("\n");
        }
}

void read_line()
{
        unsigned int line_num = 0;

        // Loop to read lines
        while (line_num != lines_of_machine_codes)
        {
                read_one_line(line_num);
                line_num++;
        }
}

int main()
{
        read_line();
        return 0;
}