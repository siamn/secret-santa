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

// function should handle freeing memory itself (as only 1, 0 or -1 is returned (-1 for error))
int getYesNoResponse(char *msg)
{
    char *input;
    int response = 2;
    while (response > 1)
    {
        printf("%s", msg);
        printf("Please type 'yes'/'y'  or 'no'/'n' below:\n");
        input = getLine();

        if (strcmp(input, "yes") == 0 || strcmp(input, "y") == 0)
        {
            free(input); // free input as no longer needed.
            return 1;
        }
        else if (strcmp(input, "no") == 0 || strcmp(input, "n") == 0)
        {
            free(input); // free input as no longer needed.
            return 0;
        }
        else
        {
            printf("Please try again.\n");
        }
    }
    free(input); // free input as no longer needed.
    return -1;
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

float getFloat()
{
    char *line = getLine();
    char *end;
    float num;
    num = strtof(line, &end);
    if (end == line)
    {

        if (errno == ERANGE)
        {
            printf("Input out of range\n");
        }

        printf("Please try again with a valid number.\n");
        free(line); // have to free line as no longer being used after this function exits.
        return -1.0;
    }
    free(line);

    // printf("Float: %.2f\n", num);
    return num;
}

float getBoundPositiveFloat(int lowerBound, int upperBound)
{
    float num = -1.0;
    while (num == -1.0 || (num < lowerBound || num > upperBound))
    {
        num = getFloat();
        if (num == -1.0)
        {
            printf("Try again: \n");
        }
        else if (num < lowerBound)
        {
            printf("Input is too low. Try again:\n");
        }
        else if (num > upperBound)
        {
            printf("Input is too high. Try again:\n");
        }
    }
    return num;
}
