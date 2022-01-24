/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/utils.h"
#include "src/tables.h"
#include "src/translate_utils.h"
#include "src/translate.h"

#include "assembler.h"

#define MAX_ARGS 3
#define BUF_SIZE 1024
#define MAX_LINE_LENGTH 100

/* Chars to ignore */
const char *IGNORE_CHARS = " \f\n\r\t\v,()";

/*******************************
 * Helper Functions
 *******************************/

/* you should not be calling this function yourself. */
static void raise_label_error(uint32_t input_line, const char *label)
{
    write_to_log("Error - invalid label at line %d: %s\n", input_line, label);
}

/* call this function if more than MAX_ARGS arguments are found while parsing
   arguments.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.

   EXTRA_ARG should contain the first extra argument encountered.
 */
static void raise_extra_argument_error(uint32_t input_line, const char *extra_arg)
{
    write_to_log("Error - extra argument at line %d: %s\n", input_line, extra_arg);
}

/* You should call this function if write_pass_one() or translate_inst()
   returns 0.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.
 */
static void raise_instruction_error(uint32_t input_line, const char *name, char **args,
                                    int num_args)
{
    /* Write instruction error into file */
    write_to_log("Error - invalid instruction at line %d: ", input_line);
    log_inst(name, args, num_args);
}

/* Truncates the string at the first occurrence of the '#' character. */
static void skip_comments(char *str)
{
    /* Get comment start simbol */
    char *comment_start = strchr(str, '#');
    if (comment_start)
    {
        /* Replace comment */
        *comment_start = '\0';
    }
}

/* Reads STR and determines whether it is a label (ends in ':'), and if so,
   whether it is a valid label, and then tries to add it to the symbol table.

   INPUT_LINE is which line of the input file we are currently processing. Note
   that the first line is line 1 and that empty lines are included in this count.

   BYTE_OFFSET is the offset of the NEXT instruction (should it exist).

   Four scenarios can happen:
    1. STR is not a label (does not end in ':'). Returns 0.
    2. STR ends in ':', but is not a valid label. Returns -1.
    3a. STR ends in ':' and is a valid label. Addition to symbol table fails.
        Returns -1.
    3b. STR ends in ':' and is a valid label. Addition to symbol table succeeds.
        Returns 1.
 */
static int add_if_label(uint32_t input_line, char *str, uint32_t byte_offset,
                        SymbolTable *symtbl)
{
    /* Get the length of str */
    size_t len = strlen(str);
    if (str[len - 1] == ':')
    {
        /* Check ':' */
        str[len - 1] = '\0';
        if (is_valid_label(str))
        {
            /* The label is valid */
            if (add_to_table(symtbl, str, byte_offset) == 0)
            {
                /* Return 1 on valid */
                return 1;
            }
            else
            {

                /* Return -1 on invalid */
                return -1;
            }
        }
        else
        {
            /* Raise label error on input_line */
            raise_label_error(input_line, str);

            /* Return -1 on error */
            return -1;
        }
    }
    else
    {
        /* Return 0 on not label */
        return 0;
    }
}

/*******************************
 * Implement the Following
 *******************************/

/* First pass of the assembler. You should implement pass_two() first.

   This function should read each line, strip all comments, scan for labels,
   and pass instructions to write_pass_one(). The symbol table should also
   been built and written to specified file. The input file may or may not
   be valid. Here are some guidelines:

    1. Only one label may be present per line. It must be the first token present.
        Once you see a label, regardless of whether it is a valid label or invalid
        label, treat the NEXT token as the beginning of an instruction.
    2. If the first token is not a label, treat it as the name of an instruction.
        DO NOT try to check it is a valid instruction in this pass.
    3. Everything after the instruction name should be treated as arguments to
        that instruction. If there are more than MAX_ARGS arguments, call
        raise_extra_argument_error() and pass in the first extra argument. Do not
        write that instruction to the output file (eg. don't call write_pass_one())
    4. Only one instruction should be present per line. You do not need to do
        anything extra to detect this - it should be handled by guideline 3.
    5. A line containing only a label is valid. The address of the label should
        be the byte offset of the next instruction, regardless of whether there
        is a next instruction or not.
    6. If an instruction contains an immediate, you should output it AS IS.
    7. Comments should always be skipped before any further process.

   Just like in pass_two(), if the function encounters an error it should NOT
   exit, but process the entire file and return -1. If no errors were encountered,
   it should return 0.
 */
int pass_one(FILE *input, FILE *inter, FILE *symtbl)
{
    /* A buffer for line parsing. */
    char buf[BUF_SIZE];

    /* Variables for argument parsing. */
    char *args[MAX_ARGS];
    size_t num_args = 0;

    /* Buffers */
    char *token_buf = NULL;
    char *inst_name_buf = NULL;

    /* Record current line and byte offset */
    uint32_t current_line = 0;
    uint32_t byte_offset = 0;

    /* Flag for error */
    int return_value = 0;

    /* Symbol Tabel */
    SymbolTable *symbol_table = NULL;

    /* Temp value for add return */
    int add_label_result = 0;

    /* Extra argument */
    char *extra_arg = NULL;

    /* Added line number */
    unsigned added_line_num = 0;

    /* For each line, there are some hints of what you should do:
        1. Skip all comments
        2. Use `strtok()` to read next token
        3. Deal with labels
        4. Parse the instruction
     */

    symbol_table = create_table(SYMBOLTBL_UNIQUE_NAME);

    /* Use for loop to parse all lines */
    while (fgets(buf, MAX_LINE_LENGTH, input))
    {
        /* Add line num by 1 */
        current_line++;

        /* Skip comments */
        skip_comments(buf);

        /* If only comment in a line, skip to next line */
        if (!(*buf))
            continue;

        /* Read token */
        token_buf = strtok(buf, IGNORE_CHARS);

        /* Skip empty lines */
        if (!token_buf)
            continue;

        /* Deal with labels */
        add_label_result = add_if_label(current_line, token_buf, byte_offset, symbol_table);

        /* Check result of add if label */
        if (add_label_result == 0)
        {
            /* STR is not a label (does not end in ':') */
            inst_name_buf = token_buf;
        }
        else if (add_label_result == 1)
        {
            /* STR ends in ':' and is valid label. Add succeeds */
            /* No instructions this line, skip to next line */
            if (!(inst_name_buf = strtok(NULL, IGNORE_CHARS)))
                continue;
        }
        else if (add_label_result == -1)
        {
            /* STR ends in ':' and add fails */
            return_value = -1;

            /* Skip the line */
            continue;
        }

        /* Parse instruction arguments */
        for (num_args = 0; num_args < MAX_ARGS; num_args++)
        {
            /* Put arguments into args */
            if (!(args[num_args] = strtok(NULL, IGNORE_CHARS)))
                break;
        }

        /* Check if too many arguments */
        if ((extra_arg = strtok(NULL, IGNORE_CHARS)) != NULL)
        {
            /* Too many arguments */
            raise_extra_argument_error(current_line, extra_arg);
            return_value = -1;

            /* Skip this line */
            continue;
        }

        /* Write instruction to .int file, add byte offset */
        if (!(added_line_num = write_pass_one(inter, inst_name_buf, args, num_args)))
        {
            /* Instruction error */
            raise_instruction_error(current_line, inst_name_buf, args, num_args);
            return_value = -1;
        }

        /* Add byte offset */
        byte_offset += 4 * added_line_num;
    }

    /* Write symbol table into file */
    write_table(symbol_table, symtbl);

    /* Return return value */
    return return_value;
}

/* Second pass of the assembler.

   This function should read an intermediate file and the corresponding symbol table
   file, translates it into machine code. You may assume:
    1. The input file contains no comments
    2. The input file contains no labels
    3. The input file contains at maximum one instruction per line
    4. All instructions have at maximum MAX_ARGS arguments
    5. The symbol table file is well formatted
    6. The symbol table file contains all the symbol required for translation
   If an error is reached, DO NOT EXIT the function. Keep translating the rest of
   the document, and at the end, return -1. Return 0 if no errors were encountered. */
int pass_two(FILE *inter, FILE *symtbl, FILE *output)
{
    /* A buffer for line parsing. */
    char buf[BUF_SIZE];

    /* Variables for argument parsing. */
    char *args[MAX_ARGS];
    size_t num_args = 0;

    /* Buffers for .text operation */
    char *inst_name_buf = NULL;

    /* Symbol table for output */
    SymbolTable *symbol_table = NULL;

    /* Flag for error */
    int return_value = 0;

    /* Record current line and byte offset */
    uint32_t current_line = 0;
    uint32_t byte_offset = 0;

    /* Write text segment. */
    fprintf(output, ".text\n");

    /* Create symbol table here by reading the symbol table file. */
    symbol_table = create_table_from_file(SYMBOLTBL_UNIQUE_NAME, symtbl);

    /* For each line, there are some hints of what you should do:
        1. Get instruction name.
        2. Parse instruction arguments; Extra arguments should be filtered out in
         `pass_one()`, so you don't need to worry about that here.
        3. Use `translate_inst()` to translate the instruction and write to the
         output file;
        4. Or if an error occurs, call `raise_instruction_error()` instead of write
         the instruction.
     */

    /* Use for loop to parse all lines */
    while (fgets(buf, MAX_LINE_LENGTH, inter))
    {
        /* Add line num by 1 */
        current_line++;

        /* Get instruction name */
        inst_name_buf = strtok(buf, IGNORE_CHARS);

        /* Parse instruction arguments */
        for (num_args = 0; num_args < MAX_ARGS; num_args++)
        {
            /* parse arguments */
            if (!(args[num_args] = strtok(NULL, IGNORE_CHARS)))
                break;
        }

        /* Translate instruction and write to output file */
        if (translate_inst(output, inst_name_buf, args, num_args, byte_offset, symbol_table) == -1)
        {
            /* Raise instruction error */
            raise_instruction_error(current_line, inst_name_buf, args, num_args);
            return_value = -1;
        }

        /* add current address by 1 */
        byte_offset += 4;
    }

    /* Write symbol segment. */
    fprintf(output, "\n.symbol\n");

    /* Write symbols here by calling `write_table()` */
    write_table(symbol_table, output);

    return return_value;
}

/*******************************
 * Do Not Modify Code Below
 *******************************/

static int open_files_pass_one(FILE **input, FILE **inter, FILE **symtbl,
                               const char *input_name, const char *inter_name, const char *symtbl_name)
{
    /* Open file for input */
    *input = fopen(input_name, "r");
    if (!*input)
    {
        /* Return error if cannot open */
        write_to_log("Error: unable to open input file: %s\n", input_name);
        return -1;
    }

    /* Open .int file */
    *inter = fopen(inter_name, "w");
    if (!*inter)
    {
        /* On error */
        write_to_log("Error: unable to open intermediate file: %s\n", inter_name);
        fclose(*input);
        return -1;
    }

    /* Open .symtbl file */
    *symtbl = fopen(symtbl_name, "w");
    if (!*symtbl)
    {
        /* On error */
        write_to_log("Error: unable to open symbol table file: %s\n", symtbl_name);

        /* Close files */
        fclose(*input);
        fclose(*inter);

        /* Return -1 on error */
        return -1;
    }

    /* Return 0 on success */
    return 0;
}

/* Open files needed for p2 */
static int open_files_pass_two(FILE **inter, FILE **symtbl, FILE **output,
                               const char *inter_name, const char *symtbl_name, const char *output_name)
{
    /* Open .int file */
    *inter = fopen(inter_name, "r");
    if (!*inter)
    {
        /* On error */
        write_to_log("Error: unable to open intermediate file: %s\n", inter_name);

        /* Return error -1 */
        return -1;
    }

    /* Open .symtbl file */
    *symtbl = fopen(symtbl_name, "r");
    if (!*symtbl)
    {
        /* On error */
        write_to_log("Error: unable to open symbol table file: %s\n", inter_name);
        fclose(*inter);

        /* Return error -1 */
        return -1;
    }

    /* Open .out file */
    *output = fopen(output_name, "w");
    if (!*output)
    {
        /* On error */
        write_to_log("Error: unable to open output file: %s\n", output_name);

        /* Close files */
        fclose(*inter);
        fclose(*symtbl);

        /* Return error -1 */
        return -1;
    }

    /* Return 0 on success */
    return 0;
}

/* Close all files */
static void close_files(FILE *file1, FILE *file2, FILE *file3)
{
    /* Close files */
    fclose(file1);
    fclose(file2);
    fclose(file3);
}

/* Runs the two-pass assembler. Most of the actual work is done in pass_one()
   and pass_two().
 */
int assemble(const char *in, const char *out, const char *int_, const char *sym)
{
    /* Needed files */
    FILE *input, *inter, *symtbl, *output;

    /* Error code */
    int err = 0;

    /* Pass 1 */
    if (in)
    {
        printf("Running pass one: %s -> %s, %s\n", in, int_, sym);

        /* Open files needed */
        if (open_files_pass_one(&input, &inter, &symtbl, in, int_, sym) != 0)
        {
            exit(1);
        }

        /* Check if error happens */
        if (pass_one(input, inter, symtbl) != 0)
        {
            err = 1;
        }

        /* Close files */
        close_files(input, inter, symtbl);
    }

    /* Pass 2 */
    if (out)
    {
        printf("Running pass two: %s, %s -> %s\n", int_, sym, out);

        /* Open files needed */
        if (open_files_pass_two(&inter, &symtbl, &output, int_, sym, out) != 0)
        {
            exit(1);
        }

        /* Check if error happens */
        if (pass_two(inter, symtbl, output) != 0)
        {
            err = 1;
        }

        /* Close files */
        close_files(inter, symtbl, output);
    }

    return err;
}

/* Print usage and exit */
static void print_usage_and_exit()
{
    printf("Usage:\n");

    /* Print usages */
    printf("  Runs both passes: assembler <input file> <intermediate file> <symbol table file> <output file>\n");
    printf("  Run pass #1:      assembler -p1 <input file> <intermediate file> <symbol table file>\n");
    printf("  Run pass #2:      assembler -p2 <intermediate file> <symbol table> <output file>\n");
    printf("Append -log <file name> after any option to save log files to a text file.\n");

    /* Exit */
    exit(0);
}

/* Main function */
int main(int argc, char **argv)
{
    /* Run pass1 or pass2 */
    int mode = 0;

    /* Needed files */
    char *input, *inter, *output, *symtbl;

    /* Record error */
    int err;

    /* If number of arguments is not correct ,exit */
    if (argc != 5 && argc != 7)
    {
        print_usage_and_exit();
    }

    /* Decide which pass to run */
    if (strcmp(argv[1], "-p1") == 0)
    {
        /* Set mode */
        mode = 1;
    }
    else if (strcmp(argv[1], "-p2") == 0)
    {
        /* Set mode */
        mode = 2;
    }

    /* Assign arguments */
    if (mode == 1)
    {
        /* Assign files needed */
        input = argv[2];
        inter = argv[3];
        symtbl = argv[4];

        /* No .out file needed */
        output = NULL;
    }
    else if (mode == 2)
    {
        /* Assign files needed */
        input = NULL;
        inter = argv[2];
        symtbl = argv[3];

        /* Output to .out file */
        output = argv[4];
    }
    else
    {
        /* Run all passes */
        input = argv[1];
        inter = argv[2];
        symtbl = argv[3];

        /* Output to .out file */
        output = argv[4];
    }

    /* Log error messages */
    if (argc == 7)
    {
        if (strcmp(argv[5], "-log") == 0)
        {
            /* Set log file according to .out file */
            set_log_file(argv[6]);
        }
        else
        {
            /* Print usage and exit */
            print_usage_and_exit();
        }
    }

    /* Record error message */
    err = assemble(input, output, inter, symtbl);

    /* On error */
    if (err)
    {
        /* Error exists */
        write_to_log("One or more errors encountered during assembly operation.\n");
    }
    else
    {
        /* Successful */
        write_to_log("Assembly operation completed successfully!\n");
    }

    /* If log file is set */
    if (is_log_file_set())
    {
        printf("Results saved to %s\n", argv[5]);
    }

    /* Return error */
    return err;
}
