/*******************************************************************************
*      Filename: otp_functions.c
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: The main client and server functions.
*******************************************************************************/

#include "cipher_utils.h"
#include "file_utils.h"
#include "msg_utils.h"
#include "otp_functions.h"
#include "signal_utils.h"
#include "socket_utils.h"

/*******************************************************************************
*      Function: otp_client()
*   Description: The main otp_client procedure.
*    Parameters: const char *ptext - The text filename.
*                const char *key - The key filename.
*                const char *port - The port value string.
*                int mode - The cipher mode.
* Preconditions: The client argument count has been validated.
*       Returns: 0 on success, 1 on file error, 2 on connection error.
*******************************************************************************/

int otp_client(const char *ptext, const char *key, const char *port, 
                   int mode) {
    int sockfd, ptextSize, keySize, status;
    FILE *ptextPtr, *keyPtr;

    /* Attempt to open the text and key files. */
    ptextPtr = fopen(ptext, "r");
    if (!ptextPtr) {
        perror("fopen");
        exit(1);
    }
    keyPtr = fopen(key, "r");
    if (!keyPtr) {
        perror("fopen");
        exit(1);
    }
  
    /* Validate both files */
    status = validateFiles(ptextPtr, keyPtr, &ptextSize, &keySize);
    if (status < 0) {
        exit(1);
    }

    /* Attempt to connect to the port */
    sockfd = clientConnect(port);
    if (sockfd < 0) {
        exit(2);
    }
   
    /* Perform all message sending and receiving operations */ 
    status = clientProcessMessage(sockfd, ptextPtr, keyPtr, ptextSize, mode);
    if (status < 0) {
        fprintf(stderr, "Error: could not contact otp_%s_d on port %s\n", 
                mode == OTP_ENCIPHER ? "enc" : "dec", port);
        exit(2);
    } 

    /* Close both files */
    if (fclose(ptextPtr) == EOF) {
        perror("fclose");
    }
    if (fclose(keyPtr) == EOF) {
        perror("fclose");
    }

    return 0;
}

/*******************************************************************************
*      Function: otp_server()
*   Description: The otp server main function.
*    Parameters: const char *port - The port string.
*                int mode - The cipher mode.
* Preconditions: The server arguments have been validated.
*       Returns: 0 on success, 1 on error.
*******************************************************************************/

int otp_server(const char *port, int mode) {
    struct sockaddr_in clientAddress = {0};
    socklen_t sizeOfClientInfo;
    pid_t spawnpid = -5;
    int listenfd, inboundfd, status;

    sizeOfClientInfo = sizeof(clientAddress);

    /* Register the SIGCHLD handler. This will automatically reap child
     * processes. */
    handlerRegister();
 
    /* Create the listening socket and bind it to the port */
    listenfd = serverBind(port);
    if (listenfd < 0) {
        exit(1);
    }

    /* Set up the listening socket state */
    status = listen(listenfd, OTP_CONN_MAX);
    if (status < 0) {
        perror("listen");
        exit(1);
    }

    while (1) {
        /* Accept an inbound connection */
        inboundfd = accept(listenfd, (struct sockaddr *)&clientAddress, 
                           &sizeOfClientInfo);
        if (inboundfd < 0) {
            perror("accept");
            close(inboundfd);
        } else {
            /* Fork of a process to handle the client */
            spawnpid = fork();
            switch(spawnpid) {
                /* Error condition */
                case -1:
                    perror("fork");
                    exit(1);
                    break;
                /* Child process */
                case 0:
                    /* Receive and process the client message */
                    status = serverProcessMessage(inboundfd, mode);
                    /* Regardless of error, shutdown and close the
                     * connection. Shutting down will prevent the
                     * client from blocking on recv(). */
                    shutdown(inboundfd, SHUT_RDWR);
                    close(inboundfd);
                    return status;
                    break;
                /* Parent process */
                default:
                    break;
            }
        }
    }

    close(listenfd);
    return 0;
}
