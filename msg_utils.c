/*******************************************************************************
*      Filename: msg_utils.c
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: Provides utility functions for forming and extracting packets.
*******************************************************************************/

#include "cipher_utils.h"
#include "msg_utils.h"

/*******************************************************************************
*      Function: min()
*   Description: Returns the minimum value of two integers.
*    Parameters: int a - The first integer.
*                int b - The second integer.
* Preconditions: None.
*       Returns: The minimum value.
*******************************************************************************/

int min(int a, int b) {
    return a < b ? a : b;
}

/*******************************************************************************
*      Function: segmentToPacketLen()
*   Description: Converts a segment length to the length of a packet.
*    Parameters: int segmentLen - The segment length.
* Preconditions: None.
*       Returns: The packet payload length.
*******************************************************************************/

int segmentToPacketLen(int segmentLen) {
    return (segmentLen * 2) + OTP_DELIMITER_BYTES + OTP_HEADER_BYTES;
}

/*******************************************************************************
*      Function: formPacket()
*   Description: Forms a packet on the client side.
*    Parameters: FILE *ptextPtr - The text file pointer.
*                FILE *keyPtr - The key file pointer.
*                int ptextRem - The amount of text in bytes remaining to be 
*                               processed.
*                char *packetBuffer - The packet string buffer.
*                int packetBufferLen - The packet buffer length.
*                int mode - Encipher or decipher mode.
* Preconditions: Both files have been validated. The packet buffer length is
*                accurate. The mode is set to a valid state.
*       Returns: -1 on error. The length of the text segment processed, 
*                otherwise.
*******************************************************************************/

int formPacket(FILE *ptextPtr, FILE *keyPtr, int ptextRem, char *packetBuffer, 
               int packetBufferLen, int mode) {
    char finalChar = OTP_END_DELIM;
    /* Determine the maximum length of the text segment. */
    int maxSegmentLen = (packetBufferLen - OTP_DELIMITER_BYTES - 
                         OTP_HEADER_BYTES) / 2;
    int i, segmentLen;
    int offset = 0;

    /* Throw an error if a segment can't be formed or there isn't any text
     * remaining to be processed */
    if (maxSegmentLen <= 0 || ptextRem <= 0) {
        fprintf(stderr, "formPacket: Error in arguments\n");
        return -1;
    }

    /* determine the number of text bytes to write  */
    segmentLen = min(maxSegmentLen, ptextRem); 

    /* Clear the packet buffer */
    memset(packetBuffer, '\0', sizeof(packetBuffer));

    /* Place the header */
    if (mode == OTP_ENCIPHER) {
        packetBuffer[offset++] = 'e';
    } else {
        packetBuffer[offset++] = 'd';
    }

    /* Place the text segment */
    for (i = 0; i < segmentLen; i++) {
        packetBuffer[offset++] = (unsigned char) fgetc(ptextPtr);
    }
   
    /* Place the midline delimiter  */
    packetBuffer[offset++] = OTP_DELIMITER;

    /* Place the ciphertext segment */
    for (i = 0; i < segmentLen; i++) {
        packetBuffer[offset++] = (unsigned char) fgetc(keyPtr);
    }
  
    /* Place the ending delimiter */
    if (ptextRem > segmentLen) {
        finalChar = OTP_CONT_DELIM;
    }
    sprintf(&packetBuffer[offset], "%c%c", OTP_DELIMITER, finalChar);

    return segmentLen;
}

/*******************************************************************************
*      Function: extractPacket()
*   Description: Extracts the text and key segments from a packet.
*    Parameters: char *packet - The packet string.
*                int packetLen - The packet string length.
*                char *plain - The text string buffer.
*                int plainLen - The text string length.
*                char *key - The key string buffer.
*                int keyLen - The key string length.
*                int expectedMode - The expected packet mode.
* Preconditions: All lengths are correct for their respective buffers. The 
*                expected mode (encipher or decipher) is correct.
*       Returns: 1 if the packet is a continuation packet, 0 if the packet is
*                an end transmission packet, -1 otherwise.
*******************************************************************************/

int extractPacket(char *packet, int packetLen, char *plain, int plainLen,
                  char *key, int keyLen, int expectedMode) {
    char *midpointDelim, *endDelim;
    int i = 0;
    int curr = 0;
    int mode;
  
    /* Check buffer lengths */
    if (keyLen <= 0 || plainLen <= 0 || packetLen <= 0) {
        fprintf(stderr, "extractPacket: Invalid buffer length\n");
        return -1;
    }

    /* Verify that packet mode matches expected mode */
    mode = packet[0] == 'e' ? OTP_ENCIPHER : OTP_DECIPHER; 
    if (mode != expectedMode) {
        return -1;
    }

    /* Verify that key and text buffer lengths are the same. */
    if (plainLen != keyLen) {
        fprintf(stderr, "extractPacket: Unequal key and text buffer lengths\n");
        return -1;
    }

    /* Verify that the key and text buffers combined are less long than the
     * packet length. */
    if ((plainLen + keyLen) < packetLen) {
        fprintf(stderr, "extractPacket: key and text buffers too small\n");
        return -1;
    }
    /* Process the packet text segment into the text buffer */ 
    midpointDelim = strchr(packet, OTP_DELIMITER);
    if (!midpointDelim) {
        fprintf(stderr, "extractPacket: No mid delimiter found\n");
        return -1;
    }
    memcpy(plain, &packet[1], midpointDelim - &packet[1]);  

    /* Process the packet key segment into the key buffer */
    endDelim = strchr(midpointDelim+1, OTP_DELIMITER); 
    if (!endDelim) {
        fprintf(stderr, "extractPacket: No end delimiter found\n");
        return -1; 
    }
    memcpy(key, midpointDelim+1, endDelim - (midpointDelim+1));

    /* Verify that the key and text strings are identical in length */
    if (strlen(plain) != strlen(key)) {
        fprintf(stderr, "extractPacket: key and text of unequal length\n");
        return -1;
    }
 
    /* Determine whether the packet is a continuation or end transmission 
     * packet */ 
    if (strchr(packet, OTP_CONT_DELIM) == NULL) {
        return 0;
    }
    return 1; 
 
}

/*******************************************************************************
*      Function: processMessage()
*   Description: Processes text and key buffers into the text buffer according
*                to the encipher/decipher mode.
*    Parameters: char *text - The text buffer.
*                char *key - The key buffer.
*                int mode - The encipher/decipher mode. 
* Preconditions: extractMessage() has been called to inform the text and key
*                buffers. The mode is correct.
*       Returns: 0 on success, -1 on error.
*******************************************************************************/

int processMessage(char *text, char *key, int mode) {
    unsigned char c;
    int textLen = strlen(text);
    int keyLen = strlen(key);
    int i, status;

    /* Validate text and key buffer length */
    if (textLen != keyLen || textLen == 0 || keyLen == 0) {
        fprintf(stderr, "processMessage: Invalid argument(s)\n");
        return -1;
    }

    /* Perform cipher operations */
    for (i = 0; i < textLen; i++) {
        if (mode == OTP_ENCIPHER) {
            c = encipher(text[i], key[i]); 
        } else {
            c = decipher(text[i], key[i]);
        }
        text[i] = c;
    }

    /* Append delimiters to the response message */ 
    status = sprintf(&text[textLen], "%c%c", OTP_DELIMITER, OTP_END_DELIM);
    if (status < 0) {
        perror("sprintf");
        return -1;
    }
    return 0;
}

/*******************************************************************************
*      Function: processResponse()
*   Description: Processes the server response into a string suitable for output
*    Parameters: char *text - The response buffer.
* Preconditions: The text buffer contains a server response.
*       Returns: -1 on error, 0 on success.
*******************************************************************************/

int processResponse(char *text) {
    char *delim;

    /* Validate the string length of the text */
    if (strlen(text) <= 0) {
        fprintf(stderr, "processResponse: Empty string passed as arg\n");
        return -1;
    }

    /* Find the midpoint delimiter (at the end of the ciphered text segment) */
    delim = strchr(text, OTP_DELIMITER);
    if (!delim) {
        fprintf(stderr, "processResponse: No delimiter in argument\n");
        return -1;
    }
    /* Place a null terminator at the delimiter index */
    *delim = '\0';

    return 0;
}
