#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char **argv) {
    pid_t pid = fork();

    switch (pid) {
        case -1:
            break;
        case 0:
            //todo: execl()
            break;
        default:
            //todo: close child
            break;
    }
}
