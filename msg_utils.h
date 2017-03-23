/*******************************************************************************
*      Filename: msg_utils.h
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: The header file for msg_utils.c. Please see msg_utils.c for more
*                details.
*******************************************************************************/

#ifndef MSG_UTILS_H
#define MSG_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OTP_CONT_DELIM '!'     /* The continuation message final delimiter */
#define OTP_END_DELIM '$'      /* The end transmission final delimiter */
#define OTP_DELIMITER '@'      /* The intermediate message delimiter */ 
#define OTP_DELIMITER_BYTES 3  /* The total number of delimiters in a message */
#define OTP_HEADER_BYTES 1     /* The total number of header bytes */
#define OTP_PAYLOAD_MAX 1460   /* The maximum message payload size */

int segmentToPacketLen(int);
int formPacket(FILE *, FILE *, int, char *, int, int);
int extractPacket(char *, int, char *, int, char *, int, int);

#endif
