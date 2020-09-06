#if defined(__linux__)

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <cstring>

using namespace std;

#define CLIENT_SEM_NUM 0
#define SERVER_SEM_NUM 1

constexpr auto BUFSIZE = 512;

union semun {
    int val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array;  /* Array for GETALL, SETALL */
    struct seminfo *__buf;  /* Buffer for IPC_INFO (Linux specific) */
};

key_t get_key() {
    srand(time(NULL));
    return ftok("/client", 'A') * (rand() % 10);
}

int wait_semaphore(int sem_id, unsigned short int sem_num) {
    sembuf op{sem_num, 0, SEM_UNDO};
    return semop(sem_id, &op, 1);
}

int release_semaphore(int sem_id, unsigned short int sem_num) {
    sembuf op{sem_num, -1, SEM_UNDO};
    return semop(sem_id, &op, 1);
}

int increase_semaphore(int sem_id, unsigned short int sem_num) {
    sembuf op{sem_num, 1, SEM_UNDO};
    return semop(sem_id, &op, 1);
}

int main(int argc, char **argv) {

    int pipe_fd[2];
    pid_t pid;

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int sem_id = semget(get_key(), 2, IPC_CREAT | 0600);
    semun sem_union;
    sem_union.val = 1;
    if (semctl(sem_id, CLIENT_SEM_NUM, SETVAL, sem_union) < 0) {
        cerr << "semctl failed" << endl;
        exit(EXIT_FAILURE);
    }
    sem_union.val = 0;
    if (semctl(sem_id, SERVER_SEM_NUM, SETVAL, sem_union) < 0) {
        cerr << "semctl failed" << endl;
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid == -1) {
        cerr << "fork failed" << endl;
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // child
        char buf[BUFSIZE];
        while (true) {
            memset(buf, BUFSIZE, 0);
            wait_semaphore(sem_id, CLIENT_SEM_NUM); // wait client can print
            increase_semaphore(sem_id, CLIENT_SEM_NUM);
            read(pipe_fd[0], buf, BUFSIZE);
            cout << "from server: " << buf << endl;
            release_semaphore(sem_id, SERVER_SEM_NUM); // release server can enter
        }
        _exit(EXIT_SUCCESS);
    } else { // parent
        string user_input;
        while (user_input != "/dis") {
            wait_semaphore(sem_id, SERVER_SEM_NUM); // wait server can enter
            increase_semaphore(sem_id, SERVER_SEM_NUM);
            getline(cin, user_input);
            write(pipe_fd[1], user_input.c_str(), user_input.size() + 1);
            release_semaphore(sem_id, CLIENT_SEM_NUM); // release client can print
        }
        kill(pid, SIGKILL);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        semctl(sem_id, CLIENT_SEM_NUM, IPC_RMID);
        semctl(sem_id, SERVER_SEM_NUM, IPC_RMID);
        exit(EXIT_SUCCESS);
    }
}

#endif