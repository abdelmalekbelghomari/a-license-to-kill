#include "memory.h"
#ifndef MAILBOX_HH
#define MAILBOX_HH

#define CRUCIAL 10
#define STRONG 9
#define MEDIUM 6
#define LOW 3
#define VERY LOW 2
#define SHIFT 5


void cesar_encryption(char str[], int shift);
void cesar_decryption(char str[], int shift);

void send_message(char* msg,int priority,memory_t* mem);

#endif