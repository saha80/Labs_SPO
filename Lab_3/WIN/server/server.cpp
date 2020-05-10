#if defined(_WIN32) || defined(_WIN64)
#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>

constexpr auto BUFSIZE = 512;

DWORD WINAPI InstanceThread(LPVOID);
VOID GetAnswerToRequest(LPTSTR, LPTSTR, LPDWORD);

int _tmain(VOID)
{
    BOOL   fConnected = FALSE;
    DWORD  dwThreadId = 0;
    HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = nullptr;
    LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\pipe_name");
    while (true)
    {
        _tprintf(TEXT("\nPipe Server: Main thread awaiting client connection on %s\n"), lpszPipename);
        hPipe = CreateNamedPipe(
            lpszPipename,             // pipe name
            PIPE_ACCESS_DUPLEX,       // read/write access
            PIPE_TYPE_MESSAGE |       // message type pipe
            PIPE_READMODE_MESSAGE |   // message-read mode
            PIPE_WAIT,                // blocking mode
            PIPE_UNLIMITED_INSTANCES, // max. instances
            BUFSIZE,                  // output buffer size
            BUFSIZE,                  // input buffer size
            0,                        // client time-out
            nullptr);                    // default security attribute
        if (hPipe == INVALID_HANDLE_VALUE)
        {
            _tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError());
            return -1;
        }
        fConnected = ConnectNamedPipe(hPipe, nullptr) ? TRUE : GetLastError() == ERROR_PIPE_CONNECTED;
        if (fConnected) {
            printf("Client connected, creating a processing thread.\n");
            hThread = CreateThread(
                nullptr,              // no security attribute
                0,                 // default stack size
                InstanceThread,    // thread proc
                (LPVOID)hPipe,    // thread parameter
                0,                 // not suspended
                &dwThreadId);      // returns thread ID
            if (hThread == nullptr)
            {
                _tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError());
                return -1;
            }
            CloseHandle(hThread);
        }
        else
            CloseHandle(hPipe);
    }

    return 0;
}

DWORD WINAPI InstanceThread(LPVOID lpvParam)
{
    auto hHeap = GetProcessHeap();
    auto pchRequest = (char*)HeapAlloc(hHeap, 0, BUFSIZE);
    auto pchReply = (char*)HeapAlloc(hHeap, 0, BUFSIZE);
    DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
    BOOL fSuccess = FALSE;
    HANDLE hPipe = nullptr;
    if (lpvParam == nullptr) {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   InstanceThread got an unexpected NULL value in lpvParam.\n");
        printf("   InstanceThread exiting.\n");
        if (pchReply != nullptr) HeapFree(hHeap, 0, pchReply);
        if (pchRequest != nullptr) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }
    if (pchRequest == nullptr) {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   InstanceThread got an unexpected NULL heap allocation.\n");
        printf("   InstanceThread exitting.\n");
        if (pchReply != nullptr) HeapFree(hHeap, 0, pchReply);
        return (DWORD)-1;
    }
    if (pchReply == nullptr)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   InstanceThread got an unexpected NULL heap allocation.\n");
        printf("   InstanceThread exitting.\n");
        if (pchRequest != nullptr) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }
    printf("InstanceThread created, receiving and processing messages.\n");
    hPipe = (HANDLE)lpvParam;
    while (true)
    {
        fSuccess = ReadFile(
            hPipe,        // handle to pipe
            pchRequest,    // buffer to receive data
            BUFSIZE, // size of buffer
            &cbBytesRead, // number of bytes read
            nullptr);        // not overlapped I/O
        if (!fSuccess || cbBytesRead == 0)
        {
            if (GetLastError() == ERROR_BROKEN_PIPE)
            {
                _tprintf(TEXT("InstanceThread: client disconnected.\n"));
            }
            else
            {
                _tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError());
            }
            break;
        }
        GetAnswerToRequest(pchRequest, pchReply, &cbReplyBytes);
        fSuccess = WriteFile(
            hPipe,        // handle to pipe
            pchReply,     // buffer to write from
            cbReplyBytes, // number of bytes to write
            &cbWritten,   // number of bytes written
            nullptr);        // not overlapped I/O
        if (!fSuccess || cbReplyBytes != cbWritten)
        {
            _tprintf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError());
            break;
        }
    }
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    HeapFree(hHeap, 0, pchRequest);
    HeapFree(hHeap, 0, pchReply);
    printf("InstanceThread exiting.\n");
    return 1;
}

void GetAnswerToRequest(LPTSTR pchRequest, LPTSTR pchReply, LPDWORD pchBytes)
{
    _tprintf(TEXT("Client Request String:\"%s\"\n"), pchRequest);
    if (FAILED(StringCchCopy(pchReply, BUFSIZE, TEXT(pchRequest))))
    {
        *pchBytes = 0;
        pchReply[0] = 0;
        printf("StringCchCopy failed, no outgoing message.\n");
        return;
    }
    *pchBytes = lstrlen(pchReply) + 1;
}
#endif