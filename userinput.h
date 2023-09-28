/**
 * @brief Get a single line of user input.
 *
 * @return char* Pointer to this line of input which is already allocated in memory.
 * Must be freed by the caller.
 */
char *getLine();

/**
 * @brief Get a single line of userinput where the number of characters
 * in the string is bound.
 *
 * @param limit Upper limit for number of characters in string.
 * @return char* Pointer to this line of input which is already allocated in memory.
 * Must be freed by the caller.
 */
char *getLimitedLine(int limit);

/**
 * @brief Get user input for a positive integer.
 *
 * @return int
 */
int getPositiveInt();