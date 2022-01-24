/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "tables.h"

/* Size of buffer */
#define BUF_SIZE 1024

/* Max size of a line */
#define MAX_LINE_LENGTH 100

/* If labels should be unique */
const int SYMBOLTBL_NON_UNIQUE = 0;
const int SYMBOLTBL_UNIQUE_NAME = 1;

/* define a const int for max capacity of symbol table */
const int SYMBOLTBL_MAX_CAP = 10000;

/*******************************
 * Helper Functions
 *******************************/

/* Allocation failure */
void allocation_failed()
{
    write_to_log("Error: allocation failed\n");

    /* Exit the program */
    exit(1);
}

/* Alignment incorrect */
void addr_alignment_incorrect()
{
    write_to_log("Error: address is not a multiple of 4.\n");
}

/* Name exists */
void name_already_exists(const char *name)
{
    write_to_log("Error: name '%s' already exists in table.\n", name);
}

/* Write to symtbl */
void write_sym(FILE *output, uint32_t addr, const char *name)
{
    fprintf(output, "%u\t%s\n", addr, name);
}

/*******************************
 * Symbol Table Functions
 *******************************/

/* Creates a new SymbolTable containg 0 elements and returns a pointer to that
   table. Multiple SymbolTables may exist at the same time.
   If memory allocation fails, you should call allocation_failed().
   Mode will be either SYMBOLTBL_NON_UNIQUE or SYMBOLTBL_UNIQUE_NAME. You will need
   to store this value for use during add_to_table().
 */
SymbolTable *create_table(int mode)
{
    /* Create space for symboltable */
    SymbolTable *symbolTable = malloc(sizeof(SymbolTable));

    /* call allocation failed if allocation failed */
    if (symbolTable == NULL)
    {
        allocation_failed();
    }

    /* initialize symbol table */
    symbolTable->head = NULL;
    symbolTable->len = 0;
    symbolTable->cap = SYMBOLTBL_MAX_CAP;

    /* Set the mode */
    symbolTable->mode = mode;

    /* return the pointer to the table */
    return symbolTable;
}

/* Frees the given SymbolTable and all associated memory. */
void free_table(SymbolTable *table)
{
    /* temp cursor */
    Symbol *cursor = table->head;
    Symbol *cursor_to_free = NULL;

    /* Free all cursors */
    while (cursor != NULL)
    {
        /* Record cursor to free */
        cursor_to_free = cursor;

        /* Move cursor to next element */
        cursor = cursor->next;

        /* Free cursor */
        free(cursor_to_free->name);
        free(cursor_to_free);
    }

    /* Free the table */
    free(table);
}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE.
   1. ADDR is given as the byte offset from the first instruction.
   2. The SymbolTable must be able to resize itself as more elements are added.

   3. Note that NAME may point to a temporary array, so it is not safe to simply
   store the NAME pointer. You must store a copy of the given string.

   4. If ADDR is not word-aligned, you should call addr_alignment_incorrect()
   and return -1.

   5. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists
   in the table, you should call name_already_exists() and return -1.

   6.If memory allocation fails, you should call allocation_failed().

   Otherwise, you should store the symbol name and address and return 0.
 */
int add_to_table(SymbolTable *table, const char *name, uint32_t addr)
{
    /* temp cursor */
    Symbol *cursor = table->head;

    /* new symbol */
    Symbol *newSymbol = malloc(sizeof(Symbol));

    /* Check if addr is alligned */
    if (addr%4 != 0)
    {
        /* Call error function */
        addr_alignment_incorrect();
        return -1;
    }

    /* unique check */
    if (table->mode == SYMBOLTBL_UNIQUE_NAME)
    {
        /* Do the unique check */
        while (cursor != NULL)
        {
            /* The name already exists */
            if (strcmp(name, cursor->name) == 0)
            {
                /* Call error funcion */
                name_already_exists(cursor->name);

                /* Return error */
                return -1;
            }
            else
            {
                /* Move to next element */
                cursor = cursor->next;
            }
        }
    }

    /* Reset cusor position */
    cursor = table->head;

    /* copy name data into newsymbol's name */
    newSymbol->name = malloc(strlen(name) + 1);

    /* call allocation fail on failure */
    if (newSymbol == NULL || newSymbol->name == NULL)
    {
        allocation_failed();

        /* return -1 on failure */
        return -1;
    }

    /* move the cursor to the tail */
    while (cursor != NULL && cursor->next != NULL)
    {
        cursor = cursor->next;
    }

    /* change the next pointer of last symbol */
    if (cursor == NULL)
    {
        /* cursor is head */
        cursor = newSymbol;
        table->head = newSymbol;
    }
    else
    {
        /* cursor is not head */
        cursor->next = newSymbol;
    }

    /* copy name into new element */
    memcpy(newSymbol->name, name, strlen(name) + 1);

    /* change newsymbol's addr and next pointer */
    newSymbol->addr = addr;
    newSymbol->next = NULL;

    /* resize table */
    table->len++;

    return 0;
}

/* Returns the address (byte offset) of the given symbol. If a symbol with name
   NAME is not present in TABLE, return -1.
 */
int64_t get_addr_for_symbol(SymbolTable *table, const char *name)
{
    /* temp cursor */
    Symbol *cursor = table->head;

    /* Either table or name do not exist or table is empty */
    if (!table || !name || !cursor)
    {
        /* Return error */
        return -1;
    }

    /* Many elements in list */
    while (cursor != NULL)
    {
        if (strcmp(name, cursor->name) == 0)
        {
            /* return address */
            return cursor->addr;
        }
        else
        {
            /* Move to next one */
            cursor = cursor->next;
        }
    }

    /* NAME not present in TABLE */
    return -1;
}

/* Writes the SymbolTable TABLE to OUTPUT. You should use write_sym() to
   perform the write. Do not print any additional whitespace or characters.
 */
void write_table(SymbolTable *table, FILE *output)
{
    /* temp cursor */
    Symbol *cursor = table->head;

    /* use while loop to fprint all elements */
    while (cursor != NULL)
    {
        /* do the print with write_sym */
        write_sym(output, cursor->addr, cursor->name);

        /* move cursor */
        cursor = cursor->next;
    }

    /* Free the table */
    free_table(table);
}

/* Reads the symbol table file written by the `write_table` function, and
   restore the symbol table.
 */
SymbolTable *create_table_from_file(int mode, FILE *file)
{
    /* Symbol table for output */
    SymbolTable *symbolTable = NULL;

    /* A buffer for line parsing. */
    char buf[BUF_SIZE];

    /* Buffers for .symbol operation */
    int64_t labelAddressBuf;
    char *labelNameBuf;

    symbolTable = create_table(mode);

    /* Use for loop to parse all lines */
    while (fgets(buf, MAX_LINE_LENGTH, file))
    {
        /* Get address from line buffer */
        labelAddressBuf = strtoul(strtok(buf, "\t"), 0, 10);
        labelNameBuf = strtok(NULL, "\n\t");

        /* add element into m_symbolTable */
        if (add_to_table(symbolTable, labelNameBuf, labelAddressBuf) == -1)
            return NULL;
    }

    /* Return table address if there is no error */
    return symbolTable;
}