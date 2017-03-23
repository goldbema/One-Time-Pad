/*******************************************************************************
*      Filename: otp_enc.c
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: The one-time pad encryption client.
*******************************************************************************/

#include "cipher_utils.h"
#include "otp_functions.h"

/*******************************************************************************
*      Function: main()
*   Description: The main otp encryption client function.
*    Parameters: int argc - The argument count.
*                char **argv - The argument list.
* Preconditions: None.
*       Returns: 0 on success, 1 on file validation error, 2 on connection error
*******************************************************************************/

int main(int argc, char **argv) {
    /* Validate the argument count */
    if (argc != OTP_ARGS) {
        fprintf(stderr, "Usage: otp_enc plaintext key port\n");
        exit(1);
    }
    /* Execute the one-time pad client in encipher mode */
    return otp_client(argv[1], argv[2], argv[3], OTP_ENCIPHER);
}
