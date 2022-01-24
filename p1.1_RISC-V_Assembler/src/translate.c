/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tables.h"
#include "translate_utils.h"
#include "translate.h"

/* Writes instructions during the assembler's first pass to OUTPUT. The case
   for general instructions has already been completed, but you need to write
   code to translate the li, bge and mv pseudoinstructions. Your pseudoinstruction
   expansions should not have any side effects.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in pass two. However, for
   pseudoinstructions, you must make sure that ARGS contains the correct number
   of arguments. You do NOT need to check whether the registers / label are
   valid, since that will be checked in part two.

   Also for li:
    - make sure that the number is representable by 32 bits. (Hint: the number
        can be both signed or unsigned).
    - if the immediate can fit in the imm field of an addiu instruction, then
        expand li into a single addi instruction. Otherwise, expand it into
        a lui-addi pair.

   And for bge and move:
    - your expansion should use the fewest number of instructions possible.

   venus has slightly different translation rules for li, and it allows numbers
   larger than the largest 32 bit number to be loaded with li. You should follow
   the above rules if venus behaves differently.

   Use fprintf() to write. If writing multiple instructions, make sure that
   each instruction is on a different line.

   Returns the number of instructions written (so 0 if there were any errors  fix! ：p).
 */
unsigned write_pass_one(FILE *output, const char *name, char **args, int num_args)
{
    /* Temp variables for li */
    long int li_immediate = 0;
    long int li_upper = 0;
    long int li_lower = 0;

    /* Temp string for li */
    char *t_str = NULL;

    /* Check if arguments is empty */
    if (!output || !name || !args)
        return 0;

    /* Deal with pseudo-instructions. */
    if (strcmp(name, "li") == 0)
    {
        /* Check argument num and get immediate */
        if (num_args != 2)
            return 0;

        /* Get immediate from arg[1] */
        if (translate_num(&li_immediate, args[1], -2147483648, 2147483647) == -1)
            return 0;

        printf("Imediate(HEX):%lx\n", li_immediate);

        /* Detect if imm is oversize */
        if (li_immediate <= 2047 && li_immediate >= -2048)
        {
            /* li rd, immediate -> addi rd, x0, immediate */
            args[2] = args[1];
            args[1] = "x0";

            /* Write a line into .int file */
            write_inst_string(output, "addi", args, 3);

            /* Write 1 line */
            return 1;
        }
        else
        {
            /* Set immediates */
            li_lower = li_immediate & 0xFFF;

            printf("li_lower(HEX):%lx\n", li_lower);

            li_upper = (li_immediate & 0xFFFFFFFF) >> 12;

            printf("li_upper(HEX):%lx\n", li_upper);

            /* Calculate lui value */
            if (li_lower >> 11 == 1)
            {
                /* The MSB of li_lower is 1 */
                li_upper = li_upper + 1;
                li_lower += ~(0xFFF);
                printf("li_lower(HEX):%lx\n", li_lower);
                printf("li_upper(HEX):%lx\n", li_upper);
            }

            /* Assign space for t_str */
            t_str = malloc(sizeof(char) * 100);

            /* lui rd, immediate */
            sprintf(t_str, "%ld", li_upper);
            args[1] = t_str;

            /* Write a line into .int file */
            write_inst_string(output, "lui", args, 2);

            /* Set arguments */
            args[1] = args[0];
            sprintf(t_str, "%ld", li_lower);
            args[2] = t_str;

            /* Write a line into .int file */
            write_inst_string(output, "addi", args, 3);

            /* Free temp valuable */
            free(t_str);

            /* Write 2 lines */
            return 2;
        }

        /* Input as HEX */
        if (li_immediate < 4096 && li_immediate > 2047)
        {
            /* li rd, immediate -> addi rd, x0, immediate */

            args[1] = "x0";

            /* Assign space for t_str */
            t_str = malloc(sizeof(char) * 100);

            sprintf(t_str, "0x%lx", li_immediate);

            args[2] = t_str;

            /* Write a line into .int file */
            write_inst_string(output, "addi", args, 3);
        }

        /* Detect if imm is oversize */
        else if (li_immediate < 2048 && li_immediate > -2049)
        {
            /* li rd, immediate -> addi rd, x0, immediate */
            args[2] = args[1];
            args[1] = "x0";

            /* Write a line into .int file */
            write_inst_string(output, "addi", args, 3);

            /* Write 1 line */
            return 1;
        }
    }
    else if (strcmp(name, "beqz") == 0)
    {
        /* beqz rs1, offset -> beq rs, x0, offset */
        if (num_args != 2)
            return 0;

        /* Reassign arguments */
        args[2] = args[1];
        args[1] = "x0";

        /* Write a line into .int file */
        write_inst_string(output, "beq", args, 3);

        /* Write 1 line */
        return 1;
    }
    else if (strcmp(name, "mv") == 0)
    {
        /* mv rd, rs1 -> addi rd, rs, 0 */
        if (num_args != 2)
            return 0;

        /* Reassign arguments */
        args[2] = "0";

        /* Write a line into .int file */
        write_inst_string(output, "addi", args, 3);

        /* Write 1 line */
        return 1;
    }
    else if (strcmp(name, "j") == 0)
    {
        /* j offset -> jal x0, offset */
        if (num_args != 1)
            return 0;

        /* Reassign arguments */
        args[1] = args[0];
        args[0] = "zero";

        /* Write a line into .int file */
        write_inst_string(output, "jal", args, 2);

        /* Write 1 line */
        return 1;
    }
    else if (strcmp(name, "jr") == 0)
    {
        /* jr rs1 -> jalr zero, rs1, 0 */
        if (num_args != 1)
            return 0;

        /* Reassign arguments */
        args[1] = args[0];
        args[0] = "zero";
        args[2] = "0";

        /* Write a line into .int file */
        write_inst_string(output, "jalr", args, 3);

        /* Write 1 line */
        return 1;
    }
    else if (strcmp(name, "jal") == 0 && num_args == 1)
    {
        /* jal offset -> jal x1 offset */
        args[1] = args[0];
        args[0] = "ra";

        /* Write a line into .int file */
        write_inst_string(output, "jal", args, 2);

        /* Write 1 line */
        return 1;
    }
    else if (strcmp(name, "jalr") == 0 && num_args == 1)
    {
        /* jalr rs -> jal x1 rs 0 */
        args[1] = args[0];
        args[0] = "ra";
        args[2] = "0";

        /* Write a line into .int file */
        write_inst_string(output, "jalr", args, 3);

        /* Write 1 line */
        return 1;
    }
    else
    {
        /* Write the instruction directly */
        write_inst_string(output, name, args, num_args);
        return 1;
    }

    /* Return 0 on error */
    return 0;
}

/* Writes the instruction in hexadecimal format to OUTPUT during pass #2.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step.

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write
   anything to OUTPUT but simply return -1. venus may be a useful resource for
   this step.

   Note the use of helper functions. Consider writing your own! If the function
   definition comes afterwards, you must declare it first (see translate.h).

   Returns 0 on success and -1 on error.
 */
int translate_inst(FILE *output, const char *name, char **args, size_t num_args, uint32_t addr, SymbolTable *symtbl)
{
    /* R type instructions */
    /* add */
    if (strcmp(name, "add") == 0)
        return write_rtype(0x33, 0x00, 0x00, output, args, num_args);
    /* or */
    else if (strcmp(name, "or") == 0)
        return write_rtype(0x33, 0x06, 0x00, output, args, num_args);
    /* slt */
    else if (strcmp(name, "slt") == 0)
        return write_rtype(0x33, 0x02, 0x00, output, args, num_args);
    /* sltu */
    else if (strcmp(name, "sltu") == 0)
        return write_rtype(0x33, 0x03, 0x00, output, args, num_args);
    /* sll */
    else if (strcmp(name, "sll") == 0)
        return write_rtype(0x33, 0x01, 0x00, output, args, num_args);

    /* I type instructions */
    /* addi */
    else if (strcmp(name, "addi") == 0)
        return write_itype(0x13, 0x00, output, args, num_args);
    /* ori */
    else if (strcmp(name, "ori") == 0)
        return write_itype(0x13, 0x06, output, args, num_args);
    /* lb */
    else if (strcmp(name, "lb") == 0)
        return write_itype(0x03, 0x00, output, args, num_args);
    /* lbu */
    else if (strcmp(name, "lbu") == 0)
        return write_itype(0x03, 0x04, output, args, num_args);
    /* lw */
    else if (strcmp(name, "lw") == 0)
        return write_itype(0x03, 0x02, output, args, num_args);
    /* jalr */
    else if (strcmp(name, "jalr") == 0)
        return write_itype(0x67, 0x00, output, args, num_args);

    /* S type instructions */
    /* sb */
    else if (strcmp(name, "sb") == 0)
        return write_stype(0x23, 0x00, output, args, num_args);
    /* sw */
    else if (strcmp(name, "sw") == 0)
        return write_stype(0x23, 0x02, output, args, num_args);

    /* SB type instructions */
    /* beq */
    else if (strcmp(name, "beq") == 0)
        return write_sbtype(0x63, 0x00, output, args, num_args, addr, symtbl);
    /* bne */
    else if (strcmp(name, "bne") == 0)
        return write_sbtype(0x63, 0x01, output, args, num_args, addr, symtbl);
    /* blt */
    else if (strcmp(name, "blt") == 0)
        return write_sbtype(0x63, 0x04, output, args, num_args, addr, symtbl);
    /* bge */
    else if (strcmp(name, "bge") == 0)
        return write_sbtype(0x63, 0x05, output, args, num_args, addr, symtbl);

    /* U type instructions */
    /* lui */
    else if (strcmp(name, "lui") == 0)
        return write_utype(0x37, output, args, num_args);

    /* J type instructions */
    /* jal */
    else if (strcmp(name, "jal") == 0)
        return write_ujtype(0x6f, output, args, num_args, addr, symtbl);

    /* No match instructions */
    else
        return -1;
}

/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_hex() to write to
   OUTPUT. Both are defined in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_rtype(uint8_t opcode, uint8_t funct3, uint8_t funct7, FILE *output, char **args, size_t num_args)
{
    /* Variables for temp use */
    int rd, rs1, rs2;
    uint32_t instruction = 0;

    /* Check arguments number */
    if (num_args != 3)
        return -1;

    /* Ser registers */
    rd = translate_reg(args[0]);
    rs1 = translate_reg(args[1]);
    rs2 = translate_reg(args[2]);

    /* Check if error occured */
    if (rd == -1 || rs1 == -1 || rs2 == -1)
        return -1;

    /* R type instruction: funct7 rs2 rs1 funct3 rd opcode */
    instruction = opcode + (rd << 7) + (funct3 << 12) + (rs1 << 15) + (rs2 << 20) + (funct7 << 25);

    /* Write the instruction */
    write_inst_hex(output, instruction);

    /* return 0 if no error */
    return 0;
}

int write_itype(uint8_t opcode, uint8_t funct3, FILE *output, char **args, size_t num_args)
{
    /* Variables for temporary use */
    int rs1, rd;
    long int immediate = 0;
    uint32_t instruction = 0;

    /* 
    Upper and lower bound for the immediate (12 bits) 
    Treat number as unsigned if in HEX, range: 0~4096
    Treat number as signed if in DEC, range: -2048~2047
     */
    long int upper_bound = 4096;
    long int lower_bound = -2048;

    /* Check arguments number */
    if (num_args != 3)
        return -1;

    /* Destination register */
    rd = translate_reg(args[0]);

    /* Detect opcode */
    if (opcode == 0x03)
    {
        /* lb lbu lw opcode: 0x03 */
        rs1 = translate_reg(args[2]);

        /* Translate the string into number */
        if (translate_num(&immediate, args[1], lower_bound, upper_bound) == -1)
            return -1;
    }
    else if (opcode == 0x13 || opcode == 0x67)
    {
        /* addi ori opcode 0x13 -- jalr opcode 0x67 */
        rs1 = translate_reg(args[1]);

        /* Translate the string into number */
        if (translate_num(&immediate, args[2], lower_bound, upper_bound) == -1)
            return -1;
    }

    /* Check if error occured */
    if (rs1 == -1 || rd == -1)
        return -1;

    /* I type instruction: imm rs1 funct3 rd opcode */
    instruction = opcode + (rd << 7) + (funct3 << 12) + (rs1 << 15) + ((immediate & 0xFFF) << 20);

    /* Write the instruction into file */
    write_inst_hex(output, instruction);

    /* return 0 if no error */
    return 0;
}

int write_stype(uint8_t opcode, uint8_t funct3, FILE *output, char **args, size_t num_args)
{
    /* Variables for temporary use */
    int rs1, rs2;
    long int offset;
    uint32_t instruction = 0;

    /* Splited immediate */
    uint32_t immLow, immHigh;

    /* Upper and lower bound for the immediate (12 bits) */
    long int upper_bound = 2047;
    long int lower_bound = -2048;

    /* Check arguments number */
    if (num_args != 3)
        return -1;

    /* Get register */
    rs2 = translate_reg(args[0]);
    rs1 = translate_reg(args[2]);

    /* Check if error occured */
    if (rs1 == -1 || rs2 == -1)
        return -1;

    /* Translate the string into number */
    if (translate_num(&offset, args[1], lower_bound, upper_bound) == -1)
        return -1;

    /* Split offset into immLow and immHigh */
    immHigh = offset >> 5;
    immLow = offset & 0x01f;

    /* S type instruction: imm[11:5] rs2 rs1 funct3 imm[4:0] opcode */
    instruction = (immHigh << 25) + (rs2 << 20) + (rs1 << 15) + (funct3 << 12) + (immLow << 7) + opcode;

    /* Write the instruction into file */
    write_inst_hex(output, instruction);

    /* return 0 if no error */
    return 0;
}

/* Hint: the way for branch to calculate relative address. e.g. bne
     bne rs rt label
   assume the byte_offset(addr) of label is L,
   current instruction byte_offset(addr) is A
   the relative address I for label satisfy:
     L = A + I
   so the relative address is
     I = L - A              */
int write_sbtype(uint8_t opcode, uint8_t funct3, FILE *output, char **args, size_t num_args,
                 uint32_t addr, SymbolTable *symtbl)
{
    /* Variables for temporary use */
    int byte_offset, rs1, rs2;
    uint32_t instruction = 0;
    uint32_t immLow, immHigh;

    /* Record relative add */
    int64_t relative_address;

    /* Store label name */
    char *label;

    /* Upper and lower bound for the immediate (13 bits with LSB always 0) */
    long int upper_bound = 4094;
    long int lower_bound = -4096;

    /* Check arguments number */
    if (num_args != 3)
        return -1;

    /* Get arguments */
    rs1 = translate_reg(args[0]);
    rs2 = translate_reg(args[1]);
    label = args[2];

    /* Search for label address with name in symtbl */
    byte_offset = get_addr_for_symbol(symtbl, label);

    /* Check if error occured */
    if (rs1 == -1 || rs2 == -1 || byte_offset == -1)
        return -1;

    /* Calculate relative address */
    relative_address = byte_offset - (int64_t)addr;

    /* Detect upper and lower bound */
    if (relative_address < lower_bound || relative_address > upper_bound)
        return -1;

    /* Split offset into immLow and immHigh -- immHigh: [12|10:5] -- immLow: [4:1|11] */
    immHigh = ((relative_address >> 12) << 6) + ((relative_address >> 5) & 0x3F);
    immLow = (((relative_address >> 1) & 0x0F) << 1) + ((relative_address >> 11) & 0x01);

    /* B type instruction: imm[12|10:5] rs2 rs1 funct3 imm[4:1|11] opcode */
    instruction = (immHigh << 25) + (rs2 << 20) + (rs1 << 15) + (funct3 << 12) + (immLow << 7) + opcode;

    /* Write the instruction into file */
    write_inst_hex(output, instruction);

    /* return 0 if no error */
    return 0;
}

int write_utype(uint8_t opcode, FILE *output, char **args, size_t num_args)
{
    /* Variables for temporary use */
    int rd;
    long int immediate;
    uint32_t instruction = 0;

    /* Upper and lower bound for the immediate (12 bits) */
    long int upper_bound = 2147483647;
    long int lower_bound = -2147483648;

    /* Check arguments number */
    if (num_args != 2)
        return -1;

    /* Set arguments */
    rd = translate_reg(args[0]);

    /* Check if error occured */
    if (rd == -1)
        return -1;

    /* Check error at the same time */
    if (translate_num(&immediate, args[1], lower_bound, upper_bound) == -1)
        return -1;

    /* U type instruction: imm[31:12] rd opcode */
    instruction = (immediate << 12) + (rd << 7) + opcode;

    /* Write the instruction into file */
    write_inst_hex(output, instruction);

    /* return 0 if no error */
    return 0;
}

/* In this project there is no need to relocate labels,
   you may think about the reasons. */
int write_ujtype(uint8_t opcode, FILE *output, char **args, size_t num_args,
                 uint32_t addr, SymbolTable *symtbl)
{
    /* Variables for temporary use */
    int rd;
    long int byte_offset;
    uint32_t instruction = 0;

    /* Record relative address */
    int64_t relative_address;

    /* Upper and lower bound for the immediate (12 bits) */
    long int upper_bound = 1048575;
    long int lower_bound = -1048576;

    /* Store label name */
    char *label;

    /* Check arguments number */
    if (num_args != 2)
        return -1;

    /* Get arguments */
    rd = translate_reg(args[0]);
    label = args[1];

    /* Check if error occured */
    if (rd == -1)
        return -1;

    /* Search for label address with name in symtbl */
    if ((byte_offset = get_addr_for_symbol(symtbl, label)) == -1)
        return -1;

    /* Calculate relative address */
    relative_address = byte_offset - (int64_t)addr;

    /* Detect upper and lower bound */
    if (relative_address < lower_bound || relative_address > upper_bound)
        return -1;

    /* J type instruction: imm[20|10:1|11|19:12] rd opcode */
    instruction = (((relative_address >> 20) & 0x01) << 31) + (((relative_address >> 1) & 0x3FF) << 21) + (((relative_address >> 11) & 0x01) << 20) + (((relative_address >> 12) & 0xFF) << 12) + (rd << 7) + opcode;

    /* Write the instruction into file */
    write_inst_hex(output, instruction);

    /* return 0 if no error */
    return 0;
}
