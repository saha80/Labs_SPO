#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "pass at least one arg" << endl;
        return 0;
    }
    if (argc >= 2) { // child
        cout << "time" << endl;
        return 0;
    }
    pid_t pid = fork();
    switch (pid) {
        case -1:
            cerr << "fork failed" << endl;
            break;
        case 0: // child
            execl(argv[0], "some_arg", nullptr);
            break;
        default: // parent
            cout << "time" << endl;
            waitpid(pid, nullptr, 0/*wait any child*/);
            break;
    }
}
