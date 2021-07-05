
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "receiveAndDisplay.h"
#include "sendAndInput.h"
#include "shutdownManager.h"
int main(int argc, char **argv)
{
	if (argc != 4) {
		fprintf(stderr,"usage: ./s-talk <myPort> <theirMachineName> <theirPort>\n");
		exit(1);
	}
	Open_Receiver(argv[1]);
	Open_Sender(argv[2],argv[3]);
	Receiver_init();
	Sender_init();
	Input_init();
	Display_init();
	printf("Enter ! to end the session \n");
	ShutdownManager_waitingForShutdown();
	Receiver_shutdown();
    Sender_shutdown();
    Display_shutdown();
    Input_shutdown();
	return 0;
}
