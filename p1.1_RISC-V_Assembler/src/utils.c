/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021.
*/

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

/*******************************
 * Do Not Modify Code Below 
 *******************************/

static const char *output_file = NULL;

/* Check if log file is set */
int is_log_file_set()
{
    return output_file != NULL;
}

/* Set log file */
void set_log_file(const char *filename)
{
    /* Check if out file exist */
    if (filename)
    {
        output_file = filename;

        /* Unlink the file */
        unlink(filename);
    }
    else
    {
        /* No output file */
        output_file = NULL;
    }
}

/* Write errors to log file */
void write_to_log(char *fmt, ...)
{
    /* arguments */
    va_list args;

    /* Check if output file exists */
    if (output_file)
    {
        /* Open output file */
        FILE *f = fopen(output_file, "a");

        /* Return error if no output file */
        if (!f)
        {
            return;
        }

        /* Print into log file */
        va_start(args, fmt);
        vfprintf(f, fmt, args);

        /* Ends outputing */
        va_end(args);

        /* Close file */
        fclose(f);
    }
    else
    {
        /* Output file doesn't exist */
        va_start(args, fmt);

        /* Print error */
        vfprintf(stderr, fmt, args);
        va_end(args);
    }
}

/* Log instruction */
void log_inst(const char *name, char **args, int num_args)
{
    /* Temp var */
    int i;

    /* Check output file exist */
    if (output_file)
    {
        /* Open output file */
        FILE *f = fopen(output_file, "a");

        /* Return error on failing */
        if (!f)
        {
            return;
        }

        /* Print the instruction name */
        fprintf(f, "%s", name);

        /* Print valuables */
        for (i = 0; i < num_args; i++)
        {
            fprintf(f, " %s", args[i]);
        }

        /* New line */
        fprintf(f, "\n");

        /* Close file */
        fclose(f);
    }
    else
    {
        /* Print the instruction name */
        fprintf(stderr, "%s", name);

        /* Print valuables */
        for (i = 0; i < num_args; i++)
        {
            fprintf(stderr, " %s", args[i]);
        }

        /* New line */
        fprintf(stderr, "\n");
    }
}
