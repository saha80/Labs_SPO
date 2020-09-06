#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void show_time() {
    auto t = time(nullptr);
    cout << ctime(&t) << endl;
}

int main(int argc, char **argv) {
    if (argc == 2) { // child
        show_time();
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
            show_time();
            waitpid(pid, nullptr, 0/*wait any child*/);
            break;
    }
}
