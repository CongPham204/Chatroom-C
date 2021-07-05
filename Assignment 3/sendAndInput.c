//This module contains the two threads - Send and Input - that will make use of the shared ADT that
//will store the messages inputed from the keyboard and will be sent to the other terminal

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>         
#include <unistd.h>         
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "sendAndInput.h"
#include "list.h"
#include "shutdownManager.h"
#include "receiveAndDisplay.h"

#define MAX_MESSAGE_LEN 1024
//Network Information
static int socketDescriptor;
static struct addrinfo sin, *serverInfo, *addressSearcher;
//Thread PID
static pthread_t senderPID, inputPID;
//Shared list
static List* sendData;

//Mutexes
static pthread_mutex_t SendListAccess = PTHREAD_MUTEX_INITIALIZER;
//Dynamic Message for receiveThread (since it's not temporary)
static char* message1;

//Thread that waits the input of the keyboard
void* inputThread(void* unused)
{
    printf("Enter your message:\n");
    while(1)
    {
        //Cancellation point
        sleep(0.001);
        if(List_count(sendData) < LIST_MAX_NUM_NODES)
        {
            message1 = malloc(MAX_MESSAGE_LEN*sizeof(char));
            //Reading the message and store it in message1
            fgets(message1,MAX_MESSAGE_LEN,stdin);
            printf("%s\n",message1);
            pthread_mutex_lock(&SendListAccess);
            {
                List_prepend(sendData,message1);
            }
            pthread_mutex_unlock(&SendListAccess);
        }
    }
}

//Thread that sends the message over to the other terminal
void* sendThread(void* unused)
{
    while(1)
    {
        //Cancellation point
        sleep(0.001);
        if(List_count(sendData) > 0)
        {
            char* message2;
            pthread_mutex_lock(&SendListAccess);
            {
                message2 = List_trim(sendData);
            }
            pthread_mutex_unlock(&SendListAccess);
            //Taking the message from the shared list and send it over
            int errorNumber = sendto(socketDescriptor, message2, MAX_MESSAGE_LEN, 0, addressSearcher->ai_addr, addressSearcher->ai_addrlen);
            if(errorNumber == -1)
            {
                printf("Error sending message: %d\n",errorNumber);
                exit(1);
            }
            if(message2[0] == '!')
            {
                ShutdownManager_triggerShutdown();
            }
            free(message2);
        }
    }
}

//Establish connection with the other terminal
void Open_Sender(char* remoteMachineName,char* remotePort)
{
    memset(&sin, 0, sizeof(sin));
    sin.ai_family = AF_UNSPEC;
    sin.ai_socktype = SOCK_DGRAM;
    int errorNumber = getaddrinfo(remoteMachineName, remotePort, &sin, &serverInfo);
    if(errorNumber != 0)
    {
        printf("Error getting address: %d\n",errorNumber);
        exit(1);
    }
    for(addressSearcher = serverInfo; addressSearcher != NULL; addressSearcher = addressSearcher->ai_next) 
    {
        socketDescriptor = socket(addressSearcher->ai_family, addressSearcher->ai_socktype,addressSearcher->ai_protocol);
        if(socketDescriptor == 1)
        {
            printf("Socket Error\n");
            continue;
        }
        break;
    }

    if(addressSearcher == NULL)
    {
        printf("Failed to bind sockets:\n");
        exit(1);
    }
    freeaddrinfo(serverInfo);
    //Initialize the shared list
    sendData = List_create();
}
//Initialize the 2 threads
void Sender_init(void)
{
    pthread_create(
        &senderPID,
        NULL,
        sendThread,
        NULL);
}

void Input_init(void)
{
    pthread_create(
        &inputPID,
        NULL,
        inputThread,
        NULL);
}

//Shutdown the 2 threads
void Sender_shutdown(void)
{
    pthread_cancel(senderPID);
    pthread_join(senderPID, NULL);

}


void Input_shutdown(void)
{
    pthread_cancel(inputPID);
    pthread_join(inputPID, NULL);
    free(message1);
}

