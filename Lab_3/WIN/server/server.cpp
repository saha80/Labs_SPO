#include <string>
#if defined(_WIN32) || defined(_WIN64)
#include <iostream>
#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>

using namespace std;

int _tmain(VOID)
{
	auto pipe_name = R"(\\.\pipe\my_pipe)";
	auto server_can_enter = CreateSemaphoreA(nullptr, 1, 1, "_pipe_server_can_enter_event_");
	auto client_can_print = CreateSemaphoreA(nullptr, 0, 1, "_pipe_client_can_print_event_");
	auto pipe = CreateNamedPipeA(pipe_name, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE, 2, BUFSIZ, BUFSIZ, 20000000, nullptr);
	if (pipe == INVALID_HANDLE_VALUE) {
		cerr << "CreateNamedPipe failed, GetLastError = " << GetLastError() << endl;
		return EXIT_FAILURE;
	}
	string user_input;
	auto is_running = true;
	DWORD written;
	while (is_running)
	{
		WaitForSingleObject(server_can_enter, INFINITE);
		getline(cin, user_input);
		if (user_input == "\\disconnect/") {
			is_running = false;
		}
		if (!WriteFile(pipe, user_input.c_str(), user_input.size(), &written, nullptr)) {
			cerr << "WriteFile failed, GetLastError = " << GetLastError() << endl;
		}
		ReleaseSemaphore(client_can_print, 1, nullptr);
	}
	CloseHandle(pipe);
	return 0;
}
#endif