#ifndef _RECEIVEANDDISPLAY_H_
#define _RECEIVEANDDISPLAY_H_

// Start connection with other terminal
void Open_Receiver(char*);
// Start background threads
void Receiver_init(void);
void Display_init(void);

// Stop background threads and cleanup
void Receiver_shutdown(void);
void Display_shutdown(void);

#endif
