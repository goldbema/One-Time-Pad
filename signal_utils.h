/*******************************************************************************
*      Filename: signal_utils.h
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: The header file for signal_utils.c. Please see signal_utils.c for
*                more details.
*******************************************************************************/

#ifndef SIGNAL_UTILS_H
#define SIGNAL_UTILS_H

#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void handlerRegister();

#endif
