/*******************************************************************************
*      Filename: socket_utils.c
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: Provides socket utilities for opening client and server sockets
*                as well as sending and receiving messages.
*******************************************************************************/

#include "msg_utils.h"
#include "socket_utils.h"

/*******************************************************************************
*      Function: convertPort()
*   Description: Attempts to convert a port string to its integer value.
*    Parameters: const char *port - The port string.
* Preconditions: None.
*       Returns: -1 on failure, the port number otherwise.
*******************************************************************************/

int convertPort(const char *port) {
    char *endptr;
    long val;

    /* Reset errno and perform the conversion. Detect out of range and invalid
     * base errors. */
    errno = 0;
    val = strtol(port, &endptr, 10);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
           || (errno != 0 && val == 0)) {
        perror("convertPort: strtol");
        return -1;
    }
    /* Validate the port number */
    if (val < PORT_MIN || val > PORT_MAX) {
        fprintf(stderr, "convertPort: invalid port number\n");
        return -1;
    }

    return val;
}

/*******************************************************************************
*      Function: clientConnect()
*   Description: Attempts to create a socket at the specified port.
*    Parameters: const char *port - The port string.
* Preconditions: None.
*       Returns: -1 on error, the socket file descriptor on success.
*******************************************************************************/

int clientConnect(const char *port) {
    struct sockaddr_in serverAddress = {0};
    struct hostent *he;
    char buffer[HOST_NAME_MAX+1];
    int portNum, sockfd;

    buffer[HOST_NAME_MAX] = '\0';
   
    /* Convert the port string to integer */
    portNum = convertPort(port);
    if (portNum < 0) {
        return -1;
    }

    /* Get the hostname */
    if (gethostname(buffer, sizeof(buffer)-1) != 0) {
        perror("clientConnect: gethostname");
        return -1;
    }

    /* Inform the struct hostent */
    he = gethostbyname(buffer);
    if (!he) {
        herror("clientConnect: gethostbyname");
        return -1;
    } 

    /* Create a TCP socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("clientConnect: socket");
        return -1;
    }

    /* Fill in the server information */
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNum);

    memcpy((char *)&serverAddress.sin_addr.s_addr, (char *)he->h_addr, 
           he->h_length);

    /* Attempt to connect the socket to the server */
    if (connect(sockfd, (struct sockaddr *)&serverAddress, 
                sizeof(serverAddress))) {
        perror("clientConnect: connect");
        close(sockfd);
        return -1;
    }

    return sockfd; 
}

/*******************************************************************************
*      Function: serverBind()
*   Description: Creates a listening socket and binds it to the server at a 
*                system-specified port.
*    Parameters: const char *port - The port string.
* Preconditions: None.
*       Returns: The socket file descriptor on succes, -1 on error.
*******************************************************************************/

int serverBind(const char *port) {
    struct sockaddr_in serverAddress = {0};
    int portNum, sockfd;

    /* Convert the port string to integer */
    portNum = convertPort(port);
    if (portNum < 0) {
        return -1;
    } 
   
    /* Set the address struct */ 
    serverAddress.sin_family = AF_INET;         /* Internet address family */
    serverAddress.sin_port = htons(portNum);    /* The specified port */
    serverAddress.sin_addr.s_addr = INADDR_ANY; /* All machine IPs */

    /* Create a TCP socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("serverBind: socket");
        return -1;
    } 

    /* Bind the socket */
    if (bind(sockfd, (struct sockaddr *)&serverAddress,
        sizeof(serverAddress))  < 0) {
        perror("serverBind: bind");
        return -1;
    }

    return sockfd;
}

/*******************************************************************************
*      Function: sendPacket()
*   Description: Utility for sending an entire packet.
*    Parameters: int sockfd - The socket file descriptor.
*                char *packet - The packet buffer.
*                int packetLen - The packet length.
* Preconditions: The packet length is accurate.
*       Returns: -1 on error, the total number of bytes sent otherwise.
*******************************************************************************/

int sendPacket(int sockfd, char *packet, int packetLen) {
    int totalSent = 0;
    int currSent;

    /* While data remains to be sent, call send() */
    while (totalSent < packetLen) {
        currSent = send(sockfd, &packet[totalSent], packetLen - totalSent, 0);
        if (currSent == -1) {
            perror("sendPacket: send");
            return -1;
        }
        totalSent += currSent;
    }


    return currSent;
}

/*******************************************************************************
*      Function: recvPacket()
*   Description: Receives an entire packet.
*    Parameters: int sockfd - The socket file descriptor.
*                char *packet - The packet buffer.
* Preconditions: The packet must contain either OTP_CONT_DELIM or OTP_END_DELIM.
*       Returns: 0 on remote socket closure, -1 on error, and a positive number
*                otherwise.
*******************************************************************************/

int recvPacket(int sockfd, char *packet) {
    char readBuffer[OTP_PAYLOAD_MAX+1];
    int status;

    memset(packet, '\0', sizeof(packet));

    /* While we haven't received the final delimiter, call recv(). */
    while ((strchr(packet, OTP_CONT_DELIM) == NULL) && 
           (strchr(packet, OTP_END_DELIM) == NULL)) {
        memset(readBuffer, '\0', sizeof(readBuffer));
        status = recv(sockfd, readBuffer, sizeof(readBuffer)-1, 0);
        /* Concatenate the readBuffer contents to the packet buffer */
        strcat(packet, readBuffer);
        if (status == 0) {
            return 0;
        }
        if (status == -1) {
            return -1;
        }
    }
    return status;
}

/*******************************************************************************
*      Function: clientProcessMessage()
*   Description: Sends an entire message to the server, packet by packet.
*    Parameters: int sockfd - The socket file descriptor.
*                FILE *ptextPtr - The text file pointer.
*                FILE *keyPtr - The key file pointer.
*                int ptextLen - The text length.
*                int mode - The cipher mode. 
* Preconditions: The file pointers have been validated, the socket is connected,
*                the mode is accurate, and the text length is accurate.
*       Returns: 0 on success, -1 on error.
*******************************************************************************/

int clientProcessMessage(int sockfd, FILE *ptextPtr, FILE *keyPtr, 
                         int ptextLen, int mode) {
    char packet[OTP_PAYLOAD_MAX+1];
    int totalSent = 0;
    int cur, status;
    /* While text remains to be sent... */
    while (totalSent < ptextLen) {
        /* Form a packet */
        cur = formPacket(ptextPtr, keyPtr, ptextLen - totalSent, packet,
                         OTP_PAYLOAD_MAX, mode); 
        if (cur < 0) {
            return -1;
        }

        /* Send the packet */
        status = sendPacket(sockfd, packet, segmentToPacketLen(cur));
        if (status < 0) {
            return -1;
        }
        totalSent += cur; 

        /* Receive the server response */
        status = recvPacket(sockfd, packet);
        if (status <= 0) {
            return -1;
        }

        /* Process the server response for output */
        status = processResponse(packet);
        if (status < 0) {
            return -1;
        }
        /* Output the response */
        printf("%s", packet);
        fflush(stdout); 
    }   
    printf("\n");
 
    return 0;
}

/*******************************************************************************
*      Function: serverProcessMessage()
*   Description: Processes all client packets for a single message.
*    Parameters: int inboundfd - The socket file descriptor.
*                int mode - The cipher mode.
* Preconditions: The socket is connected and the cipher mode is accurate.
*       Returns: -1 on error, 0 on success.
*******************************************************************************/

int serverProcessMessage(int inboundfd, int mode) {
    char packet[OTP_PAYLOAD_MAX+1];
    char text[(OTP_PAYLOAD_MAX/2)+1];  
    char key[(OTP_PAYLOAD_MAX/2)+1];  
    int status;
    int continuation = 1;

    /* While the packet continuation delimiter is set... */ 
    while (continuation) {
        memset(packet, '\0', sizeof(packet));
        memset(text, '\0', sizeof(text));
        memset(key, '\0', sizeof(key));
   
        /* Receive a packet */
        status = recvPacket(inboundfd, packet);
        if (status <= 0) {
            return -1;
        }

        /* Extract the text and key segments. Determine the continuation
         * state */
        continuation = extractPacket(packet, sizeof(packet), text, sizeof(text), 
                                 key, sizeof(key), mode);
        if (continuation < 0) {
            return -1;
        }

        /* Produce the ciphertext */
        status = processMessage(text, key, mode);
        if (status < 0) {
            return -1;
        }

        /* Send the ciphertext back to the client */
        status = sendPacket(inboundfd, text, strlen(text));
        if (status < 0) {
            return -1;
        } 
    }

    return 0;
}
