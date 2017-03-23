/*******************************************************************************
*      Filename: file_utils.h
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: The header file for file_utils.c. Please see file_utils.c for 
*                more details.
*******************************************************************************/

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int validateFiles(FILE *, FILE *, int *, int *);

#endif
