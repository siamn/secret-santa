#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// responsibility of caller to free line after usage for this function.
char *getLine()
{
    char *line = NULL;
    size_t len = 0;
    ssize_t linelen;
    int choice;
    char *end;

    linelen = getline(&line, &len, stdin);
    if (linelen < 0)
    {
        free(line);
        printf("Could not read user input. Try again.");
        return NULL;
    }
    int lastCharacter = strlen(line) - 1;
    if (line[lastCharacter] == '\n')
    {
        line[lastCharacter] = '\0';
    }

    return line;
}

char *getLimitedLine(int limit)
{
    char *line = NULL;
    int length = 0;
    while (length <= 0 || length >= limit || line == NULL)
    {
        line = getLine();
        length = strlen(line);
        if (length >= limit)
        {
            free(line);
            printf("The input was too long. Please try again:\n");
        }
    }
    return line;
}

// function should handle freeing memory itself as a parsed integer is returned rather than the string.
int getPositiveInt()
{
    char *line = getLine();
    int choice;
    char *end;

    choice = strtol(line, &end, 10);
    if (end == line || choice < 0)
    {
        if (choice == 0)
        {
            if (errno == ERANGE)
            {
                printf("Input out of range\n");
            }
            if (errno == EINVAL)
            {
                printf("Could not parse input.\n");
            }
        }
        printf("Please try again with a valid number.\n");
        return -1;
    }
    free(line); // have to free line as no longer being used after this function exits.
    return choice;
}
