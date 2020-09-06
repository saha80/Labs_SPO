#if defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

using handle_pair = std::pair<HANDLE, HANDLE>;

using namespace std;

void print_by_char(const int argc, char const* const* const argv, const DWORD delay_milliseconds)
{
	for (auto i = 0; i < argc; ++i) {
		for (size_t k = 0; k < strlen(argv[i]); ++k) {
			putchar(argv[i][k]);
			Sleep(delay_milliseconds);
		}
		putchar(' ');
		Sleep(delay_milliseconds);
	}
	putchar('\n');
	Sleep(delay_milliseconds);
}

void child(const int argc, char const* const* const argv)
{
	if (argc != 2) {
		cerr << "incorrect args" << endl;
		exit(EXIT_FAILURE);
	}
	const auto semaphore = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, (string("_Lab_2_") + argv[1]).c_str());
	if (!semaphore) {
		cerr << "error code: " << GetLastError() << endl;
		exit(EXIT_FAILURE);
	}
	while (true)
	{
		WaitForSingleObject(semaphore, INFINITE);
		print_by_char(argc, argv, 50);
		if (!ReleaseSemaphore(semaphore, 1, nullptr)) {
			exit(EXIT_FAILURE);
		}
	}
}

handle_pair new_handle_pair(char*const path, const size_t num)
{
	STARTUPINFO si{};
	PROCESS_INFORMATION pi{};
	si.cb = sizeof(si);
	if (!CreateProcessA(path, const_cast<LPSTR>(("Process " + to_string(num)).c_str()),
		nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
	{
		return { pi.hProcess, nullptr };
	}
	return { pi.hProcess, CreateSemaphoreA(nullptr, 0, 1, ("_Lab_2_" + to_string(num)).c_str()) };
}

void close_handle_pair(handle_pair &p)
{
	TerminateProcess(p.first, EXIT_SUCCESS);
	CloseHandle(p.first);
	CloseHandle(p.second);
}

void close_all_handle_pairs(vector<handle_pair> &v)
{
	while (!v.empty()) {
		close_handle_pair(v.back());
		v.pop_back();
	}
}

int main(int argc, char **argv) {
	if (argc > 1) {
		child(argc, argv);
	}
	vector<handle_pair> processes;
	size_t current = 0;
	auto is_running = true;
	cout << "'+' - add process" << endl << "'-' - remove last process" << endl << "'q' - exit" << endl;
	while (is_running) {
		if (_kbhit()) {
			const auto ch = tolower(_getch());
			if (ch == '+') {
				const auto pair = new_handle_pair(*argv, processes.size());
				if (!pair.first || !pair.second) {
					cerr << "error code: " << GetLastError() << endl;
					close_all_handle_pairs(processes);
					exit(EXIT_FAILURE);
				}
				processes.push_back(pair);
			}
			if (ch == '-') {
				if (!processes.empty()) {
					close_handle_pair(processes.back());
					processes.pop_back();
				}
				else {
					cout << "Can't close last process" << endl;
				}
			}
			if (ch == 'q') {
				is_running = false;
				close_all_handle_pairs(processes);
			}
		}
		if (!processes.empty()) {
			if (++current >= processes.size()) { current = 0; }
			if (!ReleaseSemaphore(processes[current].second, 1, nullptr)) {
				cerr << "error code: " << GetLastError() << endl;
				close_all_handle_pairs(processes);
				exit(EXIT_FAILURE);
			}
			WaitForSingleObject(processes[current].second, INFINITE);
		}
	}
}
#endif
