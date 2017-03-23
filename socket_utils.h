/*******************************************************************************
*      Filename: socket_utils.h
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: The header file for socket_utils.c. Please see socket_utils.c 
*                for more details.
*******************************************************************************/

#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT_MIN            1 /* Minimum port number */
#define PORT_MAX        65535 /* Maximum port number */
#define OTP_CONN_MAX        5 /* Maximum number of queued client conns */

int clientConnect(const char *);
int clientProcessMessage(int, FILE *, FILE *, int, int);

int serverBind(const char *);
int serverProcessMessage(int, int);

int sendPacket(int, char *, int);
int recvPacket(int, char *);  


#endif
