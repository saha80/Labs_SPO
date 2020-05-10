#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <tchar.h>

constexpr auto BUFSIZE = 512;

int main(int argc, char** argv)
{
    HANDLE hPipe;
    auto lpvMessage = "Default message from client.";
    char chBuf[BUFSIZE];
    BOOL   fSuccess = FALSE;
    DWORD  cbRead, cbWritten, dwMode;
    const auto pipe_name = R"(\\.\pipe\pipe_name)";
    if (argc > 1)
        lpvMessage = argv[1];
    while (true)
    {
        hPipe = CreateFileA(
            pipe_name,						// pipe name
            GENERIC_READ | GENERIC_WRITE, // read and write access
            0,								// no sharing
            nullptr,			           // default security attributes
            OPEN_EXISTING,					// opens existing pipe
            0,								 // default attributes
            nullptr);						 // no template file
        if (hPipe != INVALID_HANDLE_VALUE) {
            break;
        }
        if (GetLastError() != ERROR_PIPE_BUSY)
        {
            printf("Could not open pipe. E%lu\n", GetLastError());
            return EXIT_FAILURE;
        }
        if (!WaitNamedPipeA(pipe_name, 20000))
        {
            printf("Could not open pipe: 20 second wait timed out.");
            return EXIT_FAILURE;
        }
    }
    dwMode = PIPE_READMODE_MESSAGE;
    fSuccess = SetNamedPipeHandleState(
        hPipe,    // pipe handle
        &dwMode,  // new pipe mode
        nullptr,     // don't set maximum bytes
        nullptr);    // don't set maximum time
    if (!fSuccess)
    {
        _tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%lu\n"), GetLastError());
        return EXIT_FAILURE;
    }
    std::string user_input;
    while (true)
    {
        std::getline(std::cin, user_input);
        if(user_input.empty())
        {
            break;
        }
        const DWORD cbToWrite = lstrlen(user_input.c_str()) + 1;
        _tprintf(TEXT("Sending %lu byte message: \"%s\"\n"), cbToWrite, user_input.c_str());
        fSuccess = WriteFile(
            hPipe,                  // pipe handle
            user_input.c_str(),     // message
            cbToWrite,              // message length
            &cbWritten,             // bytes written
            nullptr);               // not overlapped
        if (!fSuccess)
        {
            _tprintf(TEXT("WriteFile to pipe failed. GLE=%lu\n"), GetLastError());
            return EXIT_FAILURE;
        }
        printf("\nMessage sent to server, receiving reply as follows:\n");
        do
        {
            fSuccess = ReadFile(
                hPipe,    // pipe handle
                chBuf,    // buffer to receive reply
                BUFSIZE,  // size of buffer
                &cbRead,  // number of bytes read
                nullptr); // not overlapped
            if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
                break;
            _tprintf(TEXT("\"%s\"\n"), chBuf);
        } while (!fSuccess);
        if (!fSuccess)
        {
            _tprintf(TEXT("ReadFile from pipe failed. GLE=%lu\n"), GetLastError());
            return EXIT_FAILURE;
        }
    }
    printf("\n<End of message, press ENTER to terminate connection and exit>");
    _getch();
    CloseHandle(hPipe);
    return 0;
}
#endif