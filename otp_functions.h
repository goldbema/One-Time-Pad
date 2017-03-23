/*******************************************************************************
*    Filename: otp_functions.h
*      Author: Maxwell Goldberg
*        Date: 03.17.17
* Description: The header file for otp_functions.c. Please see otp_functions.c
*              for more details.
*******************************************************************************/

#ifndef OTP_FUNCTIONS_H
#define OTP_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define OTP_ARGS     4  /* The number of client arguments */
#define OTP_D_ARGS   2  /* The number of server arguments */

int otp_enc_client(const char *, const char *, const char *, int);
int otp_enc_server(const char *, int);

#endif
