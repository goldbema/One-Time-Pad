/*******************************************************************************
*      Filename: otp_dec.c
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: The one-time pad decryption client.
*******************************************************************************/

#include "cipher_utils.h"
#include "otp_functions.h"

/*******************************************************************************
*      Function: main()
*   Description: The main decryption client function.
*    Parameters: int argc - The argument count.
*                char **argv - The argument list.
* Preconditions: None.
*       Returns: 0 on success. 1 on file validation failures. 2 on connection
*                related failures.
*******************************************************************************/

int main(int argc, char **argv) {
    /* Validate arguments */
    if (argc != OTP_ARGS) {
        fprintf(stderr, "Usage: otp_enc plaintext key port\n");
        exit(1);
    }

    /* Run the client in decipher mode */
    return otp_client(argv[1], argv[2], argv[3], OTP_DECIPHER);
}
