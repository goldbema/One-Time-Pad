/*******************************************************************************
*      Filename: keygen.c
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: Writes random characters from the uppercase letters and space 
*                into stdout.
*******************************************************************************/

#include "keygen.h"

int validateKeyLength(char *);
char generateKeyChar();

/*******************************************************************************
*      Function: main()
*   Description: The main keygen function.
*    Parameters: int argc - The argument count.
*                char **argv - The argument list.
* Preconditions: None.
*       Returns: 0 on success, 1 on failure.
*******************************************************************************/

int main(int argc, char **argv) {
    int i, val; 
    /* Seed the random number generator */
    srand(time(NULL));

    /* keygen takes only 1 argument representing the number of chars to 
     * be generated */
    if (argc != KEYGEN_ARGS) {
        fprintf(stderr, "Usage: keygen keylength\n");
        exit(1);
    }    
    /* Validate the number of chars to be generated*/
    val = validateKeyLength(argv[1]);
    if (val < 0) {
        exit(1);
    }
    /* Generate each char and output it to stdout */
    for (i = 0; i < val; i++) {
        printf("%c", generateKeyChar());
    } 
    printf("\n");
    fflush(stdin);
    return 0;
}

/*******************************************************************************
*      Function: validateKeyLength()
*   Description: Validates the key length argument.
*    Parameters: char *keylen - The key length as a string.
* Preconditions: None.
*       Returns: -1 on error, the key length otherwise.
*******************************************************************************/

int validateKeyLength(char *keylen) {
    char *endptr;
    int val;

    /* Reset the global errno */
    errno = 0;
    /* Convert the string to integer */
    val = strtol(keylen, &endptr, 10);
    /* Check for underflow and overflow, as well as unsupported base */
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
           || (errno != 0 && val == 0)) {
        perror("strtol");
        return -1;
    }
    /* If the value is negative, return an error */
    if (val <= 0) {
        fprintf(stderr, "invalid keylength\n");
        return -1;
    }

    return val;
}

/*******************************************************************************
*      Function: generateKeyChar()
*   Description: Generates a single character.
*    Parameters: None.
* Preconditions: None.
*       Returns: A random character in the uppercase letters or space.
*******************************************************************************/

char generateKeyChar() {
    /* Note that this will distort the probability distribution away from the
     * normal distribution due to the fact that KEYGEN_NUM_CHARS does not 
     * (or is unlikely to) divide evenly into RAND_MAX. Also note that the
     * problem goes deeper, however, in that rand() itself is not guaranteed
     * to produce a uniform distribution. For simplicity's sake, we use
     * this simple method of random generation. If we decide that a uniform
     * distribution is necessary later on, we only need to alter code in
     * this function. */
    int random = rand() % KEYGEN_NUM_CHARS;

    if (random == KEYGEN_NUM_CHARS - 1) {
        return ' ';
    }

    return 'A' + random;
}
