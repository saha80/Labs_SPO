#if defined(__linux__)

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include "getch.h"

using namespace std;

pthread_mutex_t mutex;

void *print_string(void *args) {
    auto s = *(string *) args;
    while (true) {
        pthread_mutex_lock(&mutex);
        for (auto c : s) {
            putchar(c);
            usleep(10000);
        }
        putchar('\n');
        pthread_mutex_unlock(&mutex);
        usleep(200000);
    }
}

int main() {
    if (pthread_mutex_init(&mutex, nullptr) != 0) {
        cerr << "pthread_mutex_init failed" << endl;
        exit(EXIT_FAILURE);
    }
    vector<pthread_t> v;
    cout << "'+' add new thread" << endl << "'-' remove last thread" << endl << "'q' quit" << endl;
    int c;
    while (true) {
        if (kbhit()) {
            c = getch();
            if (c == '+') {
                pthread_t t;
                auto s = "thread " + to_string(v.size());
                if (pthread_create(&t, nullptr, print_string, (void *) (&s)) != 0) {
                    cerr << "pthread_create failed" << endl;
                    exit(EXIT_FAILURE);
                }
                v.push_back(t);
            }
            if (c == '-') {
                if (!v.empty()) {
                    pthread_cancel(v.back());
                    v.pop_back();
                } else {
                    cout << "can't close last thread" << endl;
                }
            }
            if (c == 'q') {
                while (!v.empty()) {
                    pthread_cancel(v.back());
                    v.pop_back();
                }
                break;
            }
        }
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}

#endif