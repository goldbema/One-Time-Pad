/*******************************************************************************
*      Filename: signal_utils.c
*        Author: Maxwell Goldberg
* Last Modified: 03.17.17
*   Description: Contains the SIGCHLD signal handler register.
*******************************************************************************/

#include "signal_utils.h"

/*******************************************************************************
*      Function: handlerRegister()
*   Description: Registers the SIGCHLD handler.
*    Parameters: None.
* Preconditions: None.
*       Returns: None.
*******************************************************************************/

void handlerRegister() {
    struct sigaction ignore_action = {0};

    ignore_action.sa_handler = SIG_IGN;

    /* By setting SIGCHLD to SIG_IGN, we cause zombie child processes to be 
     * reaped automatically. */
    sigaction(SIGCHLD, &ignore_action, NULL);
}
