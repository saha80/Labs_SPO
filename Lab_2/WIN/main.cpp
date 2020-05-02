#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <conio.h>
#include <string>
#include <vector>

using namespace std;

typedef std::pair<HANDLE, HANDLE> handle_pair;

HANDLE new_process(const size_t s, const string& parent_path) {
	STARTUPINFO si{};
	PROCESS_INFORMATION pi{};
	si.cb = sizeof(si);
	const auto args = " Process " + to_string(s);
	if (!CreateProcessA(parent_path.c_str(), (LPSTR)args.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi)) {
		cerr << "Failed creating process" << endl << "Error status " << GetLastError() << endl;
		exit(1);
	}
	return pi.hProcess;
}

void close_process(handle_pair& child) {
	TerminateProcess(child.first, EXIT_SUCCESS);
	ResetEvent(child.second);
	CloseHandle(child.first);
	CloseHandle(child.second);
}

void close_all_processes(std::vector<handle_pair> &p) {
	while (!p.empty()) {
		close_process(p.back());
		p.pop_back();
	}
}

int main(const int argc, char *argv[]) {
	if (argc > 1) { //child process
		const auto str = string("_write_event_") + argv[argc - 1];
		const auto write_event = OpenEventA(EVENT_ALL_ACCESS, false, str.c_str());
		while (true) {
			WaitForSingleObject(write_event, INFINITE);
			for (auto k = 1; k < argc; ++k) {
				for (size_t i = 0; i < strlen(argv[k]); i++) {
					putchar(argv[k][i]);
					Sleep(50);
				}
				putchar(' ');
				Sleep(50);
			}
			putchar('\n');
			Sleep(50);
			ResetEvent(write_event);
			while (WaitForSingleObject(write_event, 0) == WAIT_OBJECT_0);
		}
	}
	// parent process
	auto is_running = true;
	auto current_writing = 0u;
	vector<handle_pair> processes;

	cout << "'+' for creating new process" << endl;
	cout << "'-' for killing last process" << endl;
	cout << "'q' for exiting the program" << endl;

	while (is_running) {
		if (_kbhit()) {
			const auto ch = _getch();
			if (ch == '+') {
				const auto event_name = string("_write_event_") + std::to_string(processes.size() + 1);
				auto write_event = CreateEventA(nullptr, true, false, event_name.c_str());
				auto process = new_process(processes.size() + 1, argv[0]);
				if (!write_event) {
					close_all_processes(processes);
					return 1;
				}
				processes.emplace_back(process, write_event);
			}
			if (ch == '-') {
				if (!processes.empty()) {
					close_process(processes.back());
					processes.pop_back();
				}
				else {
					cout << "Can't close last process." << endl;
				}
			}
			if (ch == 'q') {
				is_running = false;
				close_all_processes(processes);
			}
		}
		if (!processes.empty()) {
			++current_writing;
			if (current_writing >= processes.size()) {
				current_writing = 0;
			}
			SetEvent(processes[current_writing].second);
			while (WaitForSingleObject(processes[current_writing].second, 0) == WAIT_OBJECT_0);
		}
	}
	return 0;
}