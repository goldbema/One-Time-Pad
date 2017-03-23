/*******************************************************************************
*      Filename: cipher_utils.h
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: The header file for cipher_utils.c. Please see cipher_utils.c for
*                more details.
*******************************************************************************/

#ifndef CIPHER_UTILS_H
#define CIPHER_UTILS_H

#define OTP_ENCIPHER   0    /* Encipher mode constant */
#define OTP_DECIPHER   1    /* Decipher mode constant */
#define OTP_NUM_CHARS 27    /* The number of chars in the cipher */

char decipher(char, char);
char encipher(char, char);

#endif
