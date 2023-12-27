#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../include/memory.h"

#define MAX_MESSAGE_SIZE 128
#define SHIFT 3

void caesar_decipher(char *message) {
    for (int i = 0; message[i] != '\0'; ++i) {
        char ch = message[i];
        if (ch >= 'a' && ch <= 'z') {
            ch = ch - SHIFT;
            if (ch < 'a') ch += 26;
            message[i] = ch;
        } else if (ch >= 'A' && ch <= 'Z') {
            ch = ch - SHIFT;
            if (ch < 'A') ch += 26;
            message[i] = ch;
        }
    }
}

int main() {
    // Open shared memory
    int shm_fd = shm_open("/SharedMemory", O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Map shared memory
    memory_t *memory = mmap(NULL, sizeof(memory_t), PROT_READ, MAP_SHARED, shm_fd, 0);
    if (memory == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    SpyMessage received_message;
    while (1) {
        ssize_t bytes_read = mq_receive(memory->mqInfo.mq, (char *)&received_message, sizeof(SpyMessage), NULL);
        if (bytes_read >= 0) {
            if (received_message.priority != 1) {
                caesar_decipher(received_message.content);
                printf("Received decrypted message: %s (Priority: %d)\n", received_message.content, received_message.priority);
            } else {
                printf("Deceptive message received and ignored.\n");
            }
        } else {
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }
    }

    // Cleanup
    munmap(memory, sizeof(memory_t));
    close(shm_fd);
    return 0;
}

