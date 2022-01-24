/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "translate_utils.h"

/* Write instruction from a string */
void write_inst_string(FILE *output, const char *name, char **args, int num_args)
{
    /* Temp var */
    int i;

    /* Output the name */
    fprintf(output, "%s", name);

    /* Output arguments */
    for (i = 0; i < num_args; i++)
    {
        fprintf(output, " %s", args[i]);
    }

    /* New line */
    fprintf(output, "\n");
}

/* Write instruction from hex num */
void write_inst_hex(FILE *output, uint32_t instruction)
{
    fprintf(output, "%08x\n", instruction);
}

/* Check if label is valid */
int is_valid_label(const char *str)
{
    int first = 1;

    /* Check if NULL */
    if (!str)
    {
        return 0;
    }

    /* Use while loop to check every char */
    while (*str)
    {
        /* Check first char */
        if (first)
        {
            /* Invalid first char */
            if (!isalpha((int)*str) && *str != '_')
            {
                /* does not start with letter or underscore */
                return 0;
            }
            else
            {
                /* Not first char */
                first = 0;
            }
        }
        else if (!isalnum((int)*str) && *str != '_')
        {
            /* subsequent characters not alphanumeric */
            return 0;
        }

        /* Move to next one */
        str++;
    }

    /* empty string is invalid  */
    return first ? 0 : 1;
}

/* Translate the input string into a signed number. The number is then
   checked to be within the correct range (note bounds are INCLUSIVE)
   ie. NUM is valid if LOWER_BOUND <= NUM <= UPPER_BOUND.

   The input may be in either positive or negative, and be in either
   decimal or hexadecimal format. It is also possible that the input is not
   a valid number. Fortunately, the library function strtol() can take
   care of all that (with a little bit of work from your side of course).
   Please read the documentation for strtol() carefully. Do not use strtoul()
   or any other variants.

   You should store the result into the location that OUTPUT points to. The
   function returns 0 if the conversion proceeded without errors, or -1 if an
   error occurred.
 */
int translate_num(long int *output, const char *str, long int lower_bound, long int upper_bound)
{
    /* Variables for temp use */
    int number;

    /* Record the first non-num character */
    char *endptr;

    /* Return -1 when str is null or output file is null */
    if (!str || !output)
        return -1;

    /* Check if the number is HEX
    HEX: treat as unsigned number
    DEC: treat ad signed number */
    if ((str[0] == '0' && str[1] == 'x') || (str[0] == '0' && str[1] == 'X'))
        number = strtol(str, &endptr, 16);
    else
        number = strtol(str, &endptr, 10);

    /* Detect if the number is valid */
    if (*endptr)
        return -1;

    /* Detect upper and lower bound */
    if (number < lower_bound || number > upper_bound)
        return -1;

    /* Put the converted value into output */
    *output = number;

    /* Return 0 on successful */
    return 0;
}
/* Translates the register name to the corresponding register number. Please
   see the RISC-V Green Sheet for information about register numbers.

   Returns the register number of STR or -1 if the register name is invalid.
 */
int translate_reg(const char *str)
{
    /* Check register */
    if (strcmp(str, "zero") == 0 || strcmp(str, "x0") == 0)
        return 0;
    /* Check register */
    else if (strcmp(str, "ra") == 0 || strcmp(str, "x1") == 0)
        return 1;
    /* Check register */
    else if (strcmp(str, "sp") == 0 || strcmp(str, "x2") == 0)
        return 2;
    /* Check register */
    else if (strcmp(str, "gp") == 0 || strcmp(str, "x3") == 0)
        return 3;
    /* Check register */
    else if (strcmp(str, "tp") == 0 || strcmp(str, "x4") == 0)
        return 4;
    /* Check register */
    else if (strcmp(str, "t0") == 0 || strcmp(str, "x5") == 0)
        return 5;
    /* Check register */
    else if (strcmp(str, "t1") == 0 || strcmp(str, "x6") == 0)
        return 6;
    /* Check register */
    else if (strcmp(str, "t2") == 0 || strcmp(str, "x7") == 0)
        return 7;
    /* Check register */
    else if (strcmp(str, "s0") == 0 || strcmp(str, "x8") == 0 || strcmp(str, "fp") == 0)
        return 8;
    /* Check register */
    else if (strcmp(str, "s1") == 0 || strcmp(str, "x9") == 0)
        return 9;
    /* Check register */
    else if (strcmp(str, "a0") == 0 || strcmp(str, "x10") == 0)
        return 10;
    /* Check register */
    else if (strcmp(str, "a1") == 0 || strcmp(str, "x11") == 0)
        return 11;
    /* Check register */
    else if (strcmp(str, "a2") == 0 || strcmp(str, "x12") == 0)
        return 12;
    /* Check register */
    else if (strcmp(str, "a3") == 0 || strcmp(str, "x13") == 0)
        return 13;
    /* Check register */
    else if (strcmp(str, "a4") == 0 || strcmp(str, "x14") == 0)
        return 14;
    /* Check register */
    else if (strcmp(str, "a5") == 0 || strcmp(str, "x15") == 0)
        return 15;
    /* Check register */
    else if (strcmp(str, "a6") == 0 || strcmp(str, "x16") == 0)
        return 16;
    /* Check register */
    else if (strcmp(str, "a7") == 0 || strcmp(str, "x17") == 0)
        return 17;
    /* Check register */
    else if (strcmp(str, "s2") == 0 || strcmp(str, "x18") == 0)
        return 18;
    /* Check register */
    else if (strcmp(str, "s3") == 0 || strcmp(str, "x19") == 0)
        return 19;
    /* Check register */
    else if (strcmp(str, "s4") == 0 || strcmp(str, "x20") == 0)
        return 20;
    /* Check register */
    else if (strcmp(str, "s5") == 0 || strcmp(str, "x21") == 0)
        return 21;
    /* Check register */
    else if (strcmp(str, "s6") == 0 || strcmp(str, "x22") == 0)
        return 22;
    /* Check register */
    else if (strcmp(str, "s7") == 0 || strcmp(str, "x23") == 0)
        return 23;
    /* Check register */
    else if (strcmp(str, "s8") == 0 || strcmp(str, "x24") == 0)
        return 24;
    /* Check register */
    else if (strcmp(str, "s9") == 0 || strcmp(str, "x25") == 0)
        return 25;
    /* Check register */
    else if (strcmp(str, "s10") == 0 || strcmp(str, "x26") == 0)
        return 26;
    /* Check register */
    else if (strcmp(str, "s11") == 0 || strcmp(str, "x27") == 0)
        return 27;
    /* Check register */
    else if (strcmp(str, "t3") == 0 || strcmp(str, "x28") == 0)
        return 28;
    /* Check register */
    else if (strcmp(str, "t4") == 0 || strcmp(str, "x29") == 0)
        return 29;
    /* Check register */
    else if (strcmp(str, "t5") == 0 || strcmp(str, "x30") == 0)
        return 30;
    /* Check register */
    else if (strcmp(str, "t6") == 0 || strcmp(str, "x31") == 0)
        return 31;
    /* No match register */
    else
        return -1;
}
