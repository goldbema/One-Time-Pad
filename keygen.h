/*******************************************************************************
*      Filename: keygen.h
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: The header file for keygen.c. Please see keygen.c for more 
*                details.
*******************************************************************************/

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define KEYGEN_ARGS       2  /* The number of arguments to keygen */
#define KEYGEN_NUM_CHARS 27  /* The number of characters in the keygen set */
