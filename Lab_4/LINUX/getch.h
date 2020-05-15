//
// Created by alex on 5/13/20.
//
#if defined(__linux__)

#ifndef LAB_3_GETCH_H
#define LAB_3_GETCH_H

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cstdio>

int kbhit() {
    struct termios oldt{}, newt{};
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;

}

static struct termios old, current;

void initTermios(const bool echo) {
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    current = old; /* make new settings same as old settings */
    current.c_lflag &= ~ICANON; /* disable buffered i/o */
    if (echo) {
        current.c_lflag |= ECHO; /* set echo mode */
    } else {
        current.c_lflag &= ~ECHO; /* set no echo mode */
    }
    tcsetattr(0, TCSANOW, &current); /* use these new terminal i/o settings now */
}

void resetTermios() {
    tcsetattr(0, TCSANOW, &old);
}

int getch_(const bool echo) {
    int ch;
    initTermios(echo);
    ch = getchar();
    resetTermios();
    return ch;
}

int getch() {
    return getch_(true);
}

#endif //LAB_3_GETCH_H

#endif //defined(__linux__)