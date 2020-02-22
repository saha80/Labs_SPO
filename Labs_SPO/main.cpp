#include <windows.h>
#include <iostream>
#include <chrono>
#include <iomanip>

#include <tchar.h>

int _tmain(const int argc, TCHAR *argv[])
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	tm tm{};
	auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	localtime_s(&tm, &time);
	std::cout << std::put_time(&tm, "%c") << std::endl;

	if (argc < 2)
	{
		return 0;
	}
	if (!CreateProcess(
		nullptr,
		argv[1],
		nullptr,
		nullptr,
		false,
		0,
		nullptr,
		nullptr,
		&si,
		&pi)
		)
	{
		std::cout << "CreateProcess failed (" << GetLastError() << ")" << std::endl;
		return EXIT_FAILURE;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}