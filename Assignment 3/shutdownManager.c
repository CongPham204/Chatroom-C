//This module handles the shutdown for the threads
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "shutdownManager.h"

static bool Shutdown_init;
//Busy wait until the trigger shutdown function is called for
void ShutdownManager_waitingForShutdown(void)
{
    Shutdown_init = false;
    while(Shutdown_init == false);

}
void ShutdownManager_triggerShutdown(void)
{
    Shutdown_init = true;
}