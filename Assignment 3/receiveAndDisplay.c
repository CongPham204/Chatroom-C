//This module contains the two threads - Receive and Screen - that will make use of the shared ADT that
//will store the messages received from the other terminal

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

#include "receiveAndDisplay.h"
#include "list.h"
#include "shutdownManager.h"
#include "sendAndInput.h"

#define MAX_MESSAGE_LEN 1024
//Network information
static int socketDescriptor;
static struct addrinfo sin, *serverInfo, *addressSearcher;
static struct sockaddr_storage sinRemote;
static socklen_t sin_len;

//Threads PID
static pthread_t displayPID, receivePID;
//Shared List
static List* receiveData;

//Mutexes
static pthread_mutex_t ReceiveListAccess = PTHREAD_MUTEX_INITIALIZER;
//Dynamic Message for receiveThread (since it's not temporary)
static char *message1;

//Thread for receiving messages
void* receiveThread(void* unused)
{
    while(1)
    {
        //The sleep function acts as a cancellation point for pthread_cancel
        sleep(0.001);
        if(List_count(receiveData) < LIST_MAX_NUM_NODES)
        {
            message1 =  malloc(MAX_MESSAGE_LEN*sizeof(char));
            sin_len = sizeof(sinRemote);
            //Receive message from the other client
            int errorNumber = recvfrom(socketDescriptor, message1, MAX_MESSAGE_LEN , 0,(struct sockaddr *)&sinRemote, &sin_len);
            if (errorNumber == -1)
            {
                printf("Receiver error: %d\n",errorNumber);
                exit(1);
            }
            if(message1[0] == '!')
            {
                ShutdownManager_triggerShutdown();
                //Freeing the memory since message1 will not be added into the list to be freed later
                free(message1);
            }
            else
            {
                pthread_mutex_lock(&ReceiveListAccess);
                {
                    List_prepend(receiveData,message1);
                }
                pthread_mutex_unlock(&ReceiveListAccess);
            }
        }
    }
}

//Thread for displaying messages
void* displayThread(void* unused)
{
    while(1)
    {
        //Cancellation Point
        sleep(0.001);
        if(List_count(receiveData) > 0)
        {
            //message2 is temporary, so we can declare them inside and free them at the end
            char* message2;
            pthread_mutex_lock(&ReceiveListAccess);
            {
                message2 = List_trim(receiveData);
            }
            pthread_mutex_unlock(&ReceiveListAccess);
            printf("%s\n",message2);
            free(message2);
            
        }
    }
}

//Establishing sockets and bind them with the specific port
void Open_Receiver(char* localPort)
{

    memset(&sin, 0, sizeof(sin));
    sin.ai_family = AF_UNSPEC; 
    sin.ai_socktype = SOCK_DGRAM;
    sin.ai_flags = AI_PASSIVE; 

    getaddrinfo(NULL, localPort, &sin, &serverInfo);

    for(addressSearcher = serverInfo; addressSearcher != NULL; addressSearcher = addressSearcher->ai_next) 
    {
        socketDescriptor = socket(addressSearcher->ai_family, addressSearcher->ai_socktype, addressSearcher->ai_protocol);
        if (socketDescriptor == -1)
        {
            printf("Socket Error\n");
            continue;
        }
        int errorNumber = bind(socketDescriptor, addressSearcher->ai_addr, addressSearcher->ai_addrlen);
        if(errorNumber == -1)
        {
            printf("Binding Error\n");
            continue;
        }
        break;    
    }

    if(addressSearcher == NULL)
    {
        printf("Failed to bind sockets\n");
        exit(1);
    }
    freeaddrinfo(serverInfo);
    //Initialize the shared ADT list
    receiveData = List_create();
}

//Create the two threads 

void Receiver_init(void)
{
    
    pthread_create(
        &receivePID,         // PID (by pointer)
        NULL,               // Attributes
        receiveThread,      // Function
        NULL);
}


void Display_init(void)
{
    pthread_create(
        &displayPID,
        NULL,
        displayThread,
        NULL);
}

//Shutting down the 2 threads

void Receiver_shutdown(void)
{
    pthread_cancel(receivePID);
    pthread_join(receivePID, NULL);
    //Freeing the memory just to be safe
    free(message1);
}

void Display_shutdown(void)
{
    pthread_cancel(displayPID);
    pthread_join(displayPID, NULL);
}


