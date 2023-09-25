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
 * @brief Get user input for a yes / no response after displaying
 * the prompt passed in as argument.
 *
 * @param prompt Message to display to the user.
 * @return int
 */
int getYesNoResponse(char *prompt);

/**
 * @brief Get user input for a positive integer.
 *
 * @return int
 */
int getPositiveInt();

/**
 * @brief Get user input for a float
 *
 * @return float Returns -1.0 if an error occurs
 */
float getFloat();

/**
 * @brief Get user input for a positive float bounded between lowerBound
 * and upperBound. Repeatedly asks for input until this is satisfied.
 *
 * @param lowerBound
 * @param upperBound
 * @return float
 */
float getBoundPositiveFloat(int lowerBound, int upperBound);