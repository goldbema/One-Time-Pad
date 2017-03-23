/*******************************************************************************
*      Filename: cipher_utils.c
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: Provides utilities for enciphering and deciphering one-time pad
*                characters.
*******************************************************************************/

#include "cipher_utils.h"

/*******************************************************************************
*     Functions: charToInt()
*   Description: Converts a character to its corresponding 0 - OTP_NUM_CHARS-1
*                value.
*    Parameters: char c - The character to be converted.
* Preconditions: The character is in A-Z or is a space.
*       Returns: The corresponding integer.
*******************************************************************************/

int charToInt(char c) {
    return c == ' ' ? (OTP_NUM_CHARS - 1) : (c - 'A');
}

/*******************************************************************************
*     Functions: intToChar()
*   Description: Converts an integer in 0 - OTP_NUM_CHARS-1 to its corresponding
*                character value.
*    Parameters: int n - The integer to be converted.
* Preconditions: The integer is in 0 - OTP_NUM_CHARS-1.
*       Returns: The corresponding character.
*******************************************************************************/

char intToChar(int n) {
    return n == (OTP_NUM_CHARS - 1) ? ' ' : (n + 'A');
}

/*******************************************************************************
*     Functions: decipher()
*   Description: Process a single one-time pad ciphertext character using a
*                key value.
*    Parameters: char cipherChar - The ciphertext character.
*                char keyChar - The key character.
* Preconditions: The cipher and key characters are valid.
*       Returns: The deciphered character value.
*******************************************************************************/

char decipher(char cipherChar, char keyChar) {
    /* Convert the characters to integer values */
    int cipherCharVal = charToInt(cipherChar);
    int keyCharVal = charToInt(keyChar);
    int val;

    /* Perform modular arithmetic */
    val = cipherCharVal - keyCharVal;
    if (val < 0) {
         val += OTP_NUM_CHARS;
    }

    /* Return the character value */
    return intToChar(val);
}

/*******************************************************************************
*     Functions: encipher()
*   Description: Process a single one-time pad plaintext character using a key
*                value.
*    Parameters: char plainChar - The plaintext character.
*                char keyChar - The key character.
* Preconditions: The plain and key characters are valid.
*       Returns: The enciphered character value.
*******************************************************************************/

char encipher(char plainChar, char keyChar) {
    /* Convert the characters to integer values */
    int plainCharVal = charToInt(plainChar);
    int keyCharVal = charToInt(keyChar);
    int val;

    /* Perform modular arithmetic */
    val = plainCharVal + keyCharVal;
    if (val >= OTP_NUM_CHARS) {
        val -= OTP_NUM_CHARS;
    }
   
    /* Return the character value */
    return intToChar(val);
}
