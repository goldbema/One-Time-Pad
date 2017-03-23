/*******************************************************************************
*      Filename: otp_dec_d.c
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: The one-time pad decryption server.
*******************************************************************************/

#include "cipher_utils.h"
#include "otp_functions.h"

/*******************************************************************************
*      Function: main()
*   Description: The main otp decryption server function.
*    Parameters: int argc - The argument count.
*                char **argv - The argument list.
* Preconditions: None.
*       Returns: 0 on success, 1 on error.
*******************************************************************************/

int main(int argc, char **argv) {
    /* Validate the argument count. */
    if (argc != OTP_D_ARGS) {
        fprintf(stderr, "Usage: otp_enc_d listening_port\n");
        exit(1);
    }

    /* Execute the server in decipher mode */
    return otp_server(argv[1], OTP_DECIPHER);
}
