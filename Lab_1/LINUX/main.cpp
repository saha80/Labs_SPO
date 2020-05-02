#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char **argv) {
    pid_t pid = fork();
    cout << "lab1" << endl;
//    switch (pid) {
//        case -1:
//            break;
//        case 0:
//            //todo: execl()
//            break;
//        default:
//            //todo: close child
//            break;
//    }
}
