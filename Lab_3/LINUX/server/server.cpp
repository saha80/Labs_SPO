#if defined(__linux__)
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <semaphore.h>
#include <string.h>
#include <math.h>
#include <sstream>

using namespace std;

constexpr auto BUFSIZE = 512;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} semunion;

int WaitSemaphore(int textSemId, unsigned short int num, int waitTime);

int ReleaseSemaphore(int textSemId, unsigned short int num);

void parseMessage(string, char *&, char *&);

int main(int argc, char *argv[]) {
    char *amountOfMoney = new char[256];
    char *currencyOfMoney = new char[256];

    srand(time(nullptr));
    key_t shmkey = getpid() * (rand() % 100);
    int shmId = shmget(shmkey, BUFSIZE, IPC_CREAT);
    if (shmId < 0) {
        printf("shmget error\n");
        return 1;
    }

    srand(time(nullptr));
    key_t textSemKey = ftok("/client", 'A') * (rand() % 100);
    int textSemId = semget(textSemKey, 4, IPC_CREAT);

    semctl(textSemId, 0, SETALL, 0);
    if (textSemId < 0) {
        printf("Text Semaphores is not created.");
        return 1;
    }

    int pid = fork();
    switch (pid) {
        case -1:
            printf("New process is not created\n");
            return 1;

        case 0: {
            stringstream strs;
            strs << (int) shmkey;
            string temp_str = strs.str();
            char *char_type = (char *) temp_str.c_str();
            stringstream strs1;
            strs1 << (int) textSemKey;
            string temp_str1 = strs1.str();
            char *char_type1 = (char *) temp_str1.c_str();
            if (execlp("./server", "server", char_type, char_type1, NULL) == -1)
                printf("Error.");
            break;
        }
        default: {
            char *shm, *s;
            shm = (char *) shmat(shmId, nullptr, 0);
            if (shm == (char *) -1) {
                perror("shmat");
                return 1;
            }
            while (true) {
                WaitSemaphore(textSemId, 2, -1);
                int p = 0;
                char tempMessage[BUFSIZE];
                for (s = shm; *s != NULL; s++, p++) {
                    tempMessage[p] = *s;
                }
                ReleaseSemaphore(textSemId, 1);
                string message(tempMessage);
                if (message == "quit") break;

                parseMessage(message, amountOfMoney, currencyOfMoney);

                ReleaseSemaphore(textSemId, 2);
                s = shm;
                strncpy(s, message.c_str(), sizeof(message));
                WaitSemaphore(textSemId, 1, -1);
            }
            semctl(textSemId, 0, IPC_RMID, semunion);
            shmdt(shm);
        }
            break;
    }
    return 0;
}

int WaitSemaphore(int textSemId, unsigned short int num, int tempTime) {
    long long int waitTime = tempTime * 100000;
    const struct timespec time{0, waitTime};
    sembuf buf{num, -1, SEM_UNDO};
    if (waitTime < 0) {
        return semop(textSemId, &buf, 1);
    } else {
        return semtimedop(textSemId, &buf, 1, &time);
    }
}

int ReleaseSemaphore(int textSemId, unsigned short int num) {
    sembuf buf{num, 1, SEM_UNDO};
    return semtimedop(textSemId, &buf, 1, nullptr);
}

void parseMessage(string message, char *&amountOfMoney, char *&currencyOfMoney) {
    char buffer[256];
    strcpy(buffer, message.c_str());

    int i = 0;
    while (buffer[i] != ' ') {
        amountOfMoney[i] = buffer[i];
        i++;
    }
    amountOfMoney[i] = '\0';

    while (buffer[i] != ' ')
        i++;

    i++;

    currencyOfMoney[0] = buffer[i];
    i++;
    currencyOfMoney[1] = buffer[i];
    i++;
    currencyOfMoney[2] = buffer[i];

    currencyOfMoney[3] = '\0';
}
#endif