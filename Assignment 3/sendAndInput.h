#ifndef _SENDANDINPUT_H_
#define _SENDANDINPUT_H_

//Start connection with the other terminal
void Open_Sender(char*, char*);

// Start background threads
void Sender_init(void);
void Input_init(void);

// Stop background threads and cleanup
void Sender_shutdown(void);
void Input_shutdown(void);

#endif
