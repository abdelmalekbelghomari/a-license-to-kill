#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mailbox.h"
void cesar_encryption(char str[], int shift) {
  int i = 0;

  while (str[i] != '\0') {

    if(isupper(str[i])){
        char c = str[i] - 'A';
        c += shift;
        c = c % 26;
        str[i] = c + 'A';
    }else{
      char c = str[i] - 'a';
        c += shift;
        c = c % 26;
        str[i] = c + 'a';
    }
    i++;
  }

}
void cesar_decryption(char str[], int shift) {
  int i = 0;

  while (str[i] != '\0') {
    if(isupper(str[i])){
        char c = str[i] - 'A';
        c -= shift;
        c = c % 26;
        str[i] = c + 'A';
    }else{
       char c = str[i] - 'a';
        c -= shift;
        c = c % 26;
        str[i] = c + 'a';
    }
    i++;
  }

}

void send_message(char* msg,int priority,memory_t* mem){
    mqd_t mq;
    mq = mq_open("/mq", O_WRONLY);
    char msgEncrypted[MAX_LENGTH_OF_MESSAGE];
    strcpy(msgEncrypted,msg);
    cesar_encryption(msgEncrypted,SHIFT);
    mq_send(mq,msgEncrypted,strlen(msgEncrypted)+1,priority);
    mem->memory_has_changed = 1;
    mq_close(mq);
}
