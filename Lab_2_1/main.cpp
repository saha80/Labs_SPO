#include <conio.h>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

using namespace std;

HANDLE semaphore;

void child(const int argc, char *const *const argv)
{
	auto is_running = true;
	while (is_running)
	{
		const auto stat = WaitForSingleObject(semaphore, 0);
		if (stat == WAIT_OBJECT_0)
		{
			const auto millis = 50;
			for (auto i = 0; i < argc; ++i)
			{
				for (size_t k = 0; k < strlen(argv[k]); ++k)
				{
					putchar(argv[k][i]);
					Sleep(millis);
				}
				putchar(' ');
				Sleep(millis);
			}
			putchar('\n');
			Sleep(millis);
			if (!ReleaseSemaphore(semaphore, 1, nullptr))
			{
				cerr << "error code " << GetLastError() << endl;
				exit(EXIT_FAILURE);
			}
			is_running = false;
		}
		if (stat == WAIT_TIMEOUT)
		{
			is_running = false;
		}
	}
	//while (WaitForSingleObject(semaphore, 0) == WAIT_OBJECT_0)
	//{
	//	const auto millis = 50;
	//	for (auto i = 0; i < argc; ++i)
	//	{
	//		for (size_t k = 0; k < strlen(argv[k]); ++k)
	//		{
	//			putchar(argv[k][i]);
	//			Sleep(millis);
	//		}
	//		putchar(' ');
	//		Sleep(millis);
	//	}
	//	putchar('\n');
	//	Sleep(millis);
	//	if (!ReleaseSemaphore(semaphore, 1, nullptr))
	//	{
	//		cerr << "error code " << GetLastError() << endl;
	//		exit(EXIT_FAILURE);
	//	}
	//}
}

HANDLE new_process(char*const path, const size_t num)
{
	STARTUPINFO si{};
	PROCESS_INFORMATION pi{};
	si.cb = sizeof(si);
	if (!CreateProcessA(path, (LPSTR)(" Process " + to_string(num)).c_str(), nullptr, nullptr, 0, 0, nullptr, nullptr, &si, &pi))
	{
		cerr << "Last error code: " << GetLastError() << endl;
		exit(EXIT_FAILURE);
	}
	return pi.hProcess;
}

int main(int argc, char **argv) {
	if (argc > 1) {
		child(argc, argv);
	}
	vector<HANDLE> processes;
	size_t current = 0;
	auto is_running = true;
	semaphore = CreateSemaphoreA(nullptr, 0, 1, nullptr);
	if (!semaphore) {
		cerr << "Last error code: " << GetLastError() << endl;
		exit(EXIT_FAILURE);
	}
	cout << "'+' - add process" << endl
		<< "'-' - remove last process" << endl
		<< "'q' - exit" << endl;
	while (is_running) {
		if (_kbhit()) {
			const auto ch = _getch();
			if (ch == '+') {
				processes.push_back(new_process(*argv, current));
			}
			if (ch == '-') {
				if (processes.empty()) {
					CloseHandle(processes.back());
					processes.pop_back();
				}
				else {
					cout << "Can't close last process" << endl;
				}
			}
			if (ch == 'q') {
				is_running = false;
				while (!processes.empty()) {
					CloseHandle(processes.back());
					processes.pop_back();
				}
			}
			if (!processes.empty()) {
				if (++current >= processes.size()) { current = 0; }
				while (WaitForSingleObject(processes[current], 0) == WAIT_OBJECT_0);
			}
		}
	}
	CloseHandle(semaphore);
	OpenSemaphoreA(0, 0, 0);
	//ReleaseSemaphore();
	//WaitForSingleObject();
}
//#include <windows.h>
//#include <cstdio>
//
//#define MAX_SEM_COUNT 10
//#define THREADCOUNT 12
//
//HANDLE ghSemaphore;
//
//DWORD WINAPI ThreadProc(LPVOID);
//
//int main()
//{
//	HANDLE aThread[THREADCOUNT];
//	DWORD ThreadID;
//	ghSemaphore = CreateSemaphore(nullptr, MAX_SEM_COUNT, MAX_SEM_COUNT, nullptr);
//	if (ghSemaphore == nullptr)
//	{
//		printf("CreateSemaphore error: %lu\n", GetLastError());
//		return 1;
//	}
//	for (auto& i : aThread)
//	{
//		if (!(i = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, &ThreadID)))
//		{
//			printf("CreateThread error: %lu\n", GetLastError());
//			return 1;
//		}
//	}
//	WaitForMultipleObjects(THREADCOUNT, aThread, TRUE, INFINITE);
//	for (auto& i : aThread)
//	{
//		CloseHandle(i);
//	}
//	CloseHandle(ghSemaphore);
//	return 0;
//}

//DWORD WINAPI ThreadProc(LPVOID lpParam)
//{
//	UNREFERENCED_PARAMETER(lpParam);
//	BOOL bContinue = TRUE;
//	while (bContinue)
//	{
//		switch (WaitForSingleObject(ghSemaphore, 0))
//		{
//		case WAIT_OBJECT_0:
//			
//			// TODO: Perform task
//			printf("Thread %lu: wait succeeded\n", GetCurrentThreadId());
//			bContinue = FALSE;
//			//Sleep(5);
//			if (!ReleaseSemaphore(ghSemaphore, 1, nullptr))
//			{
//				printf("ReleaseSemaphore error: %lu\n", GetLastError());
//			}
//			break;
//		case WAIT_TIMEOUT:
//			printf("Thread %lu: wait timed out\n", GetCurrentThreadId());
//			break;
//		}
//	}
//	return TRUE;
//}