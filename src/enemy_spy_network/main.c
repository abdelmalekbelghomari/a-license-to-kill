#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "memory.h" 

#define MAX_MESSAGE_SIZE 128
#define SHIFT 3

void caesar_cipher(char *message) {
    for (int i = 0; message[i] != '\0'; ++i) {
        char ch = message[i];
        if (ch >= 'a' && ch <= 'z') {
            ch += SHIFT;
            if (ch > 'z') ch -= 26;
            message[i] = ch;
        } else if (ch >= 'A' && ch <= 'Z') {
            ch += SHIFT;
            if (ch > 'Z') ch -= 26;
            message[i] = ch;
        }
    }
}

int main() {
    int shm_fd;
    memory_t *shared_memory;
    SpyMessage messages[6];

    // Ouvrir la mémoire partagée existante
    shm_fd = shm_open("/SharedMemory", O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    shared_memory = mmap(0, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Initialiser les messages
    strcpy(messages[0].content, "Very Low");
    messages[0].priority = 2;

    strcpy(messages[1].content, "Low");
    messages[1].priority = 3;

    strcpy(messages[2].content, "Medium");
    messages[2].priority = 6;

    strcpy(messages[3].content, "Strong");
    messages[3].priority = 9;

    strcpy(messages[4].content, "Crucial");
    messages[4].priority = 10;

    strcpy(messages[5].content, "Deceptive");
    messages[5].priority = 1;

    // Chiffrer tous les messages
    for (int i = 0; i < 6; ++i) {
        caesar_cipher(messages[i].content);
    }

    // Envoyer les messages à la queue de messages de la mémoire partagée
    for (int i = 0; i < 6; ++i) {
        if (mq_send(shared_memory->mqInfo.mq, (char *)&messages[i], sizeof(SpyMessage), messages[i].priority) == -1) {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }
    }

    // Fermer et désallouer les ressources
    munmap(shared_memory, sizeof(memory_t));
    close(shm_fd);

    return 0;
}
