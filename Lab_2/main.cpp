#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <conio.h>
#include <string>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::to_string;

typedef std::pair<HANDLE, HANDLE> handle_pair;

HANDLE new_process(const size_t s, const string& parent_path) {
	STARTUPINFO si{};
	PROCESS_INFORMATION pi{};
	si.cb = sizeof(si);
	const auto args = /*"Process " + */' ' + to_string(s);
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
			for (size_t i = 0; i < strlen(argv[argc - 1]); i++) {
				putchar(argv[argc - 1][i]);
				Sleep(50);
			}
			putchar('\n');
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
				const auto event_name = string("_write_event_") + std::to_string(processes.size());
				auto write_event = CreateEventA(nullptr, true, false, event_name.c_str());
				auto process = new_process(processes.size(), argv[0]);
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

//#define _CRT_SECURE_NO_WARNINGS
//#include <iostream>
//#include <windows.h>
//#include <conio.h>
//#include <string>
//#include <vector>
//#include <map>
//
//using std::cout;
//using std::cerr;
//using std::endl;
//using std::vector;
//using std::string;
//using std::to_string;
//
//HANDLE newProc(int procNumber, const string& parentPath)
//{
//	STARTUPINFO si;
//	PROCESS_INFORMATION pi;
//	ZeroMemory(&si, sizeof(si));
//	ZeroMemory(&pi, sizeof(pi));
//	si.cb = sizeof(si);
//
//	const auto path = parentPath.c_str();
//	char args[20];
//	sprintf(args + 1, "%d", procNumber);
//	args[0] = ' ';
//	//const auto args = (LPSTR)(' ' + std::to_string(procNumber).c_str());
//
//	if (!CreateProcessA(path, args, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi)) {
//		std::cerr << "Failed creating process" << endl << "Error status " << GetLastError() << endl;
//		//throw std::system_error(GetLastError(), std::system_category());
//		exit(1);
//	}
//	return pi.hProcess;
//}
//
//int main(int argc, char** argv)
//{
//	setlocale(LC_ALL, "RUSSIAN");
//	if (argc > 1) {//Значит мы внутри ребёнка
//		const auto write_event = OpenEventA(EVENT_ALL_ACCESS, false, (string("WriteEvent_") + argv[1]).c_str());
//		while (true) {
//			WaitForSingleObject(write_event, INFINITE);
//			const auto p = string("Process ") + argv[1];
//			for (const auto i : p) {
//				putchar(i);
//				Sleep(100);
//			}
//			//for (size_t i = 0; i < strlen(p); ++i) {
//			//	putchar(p[i]);
//			//	Sleep(100);
//			//}
//			//for (size_t i = 0; i < strlen(argv[argc - 1]); i++) {
//			//	putchar(argv[argc - 1][i]);
//			//	Sleep(100);
//			//}
//			putchar('\n');
//			ResetEvent(write_event);
//			while (WaitForSingleObject(write_event, 0) == WAIT_OBJECT_0);
//		}
//	}
//
//	std::vector<std::pair<HANDLE, HANDLE>> children;
//	children.reserve(9);
//	unsigned currentWriting = 0;
//	auto alreadyAdded = false, alreadyDeleted = false;
//
//	cout << "Press '+' to create new process" << endl;
//	cout << "Press '-' to kill last process" << endl;
//	cout << "Press 'q' to exit the program" << endl;
//
//	while (true) {
//		if (GetKeyState(0x51)) { //Если нажата кнопка Q
//			while (!children.empty()) {
//				TerminateProcess(children.back().first, EXIT_SUCCESS);
//				ResetEvent(children.back().second);
//				CloseHandle(children.back().second);
//				CloseHandle(children.back().first);
//				children.pop_back();
//			}
//			break;
//		}
//		if (GetKeyState(VK_OEM_PLUS) < 0 && !alreadyAdded) { //Если нажат + и при этом мы ещё не обрабатывали это нажатие
//			alreadyAdded = true;
//			if (children.size() < 9) {
//				const auto event_name = string("WriteEvent_") + std::to_string(children.size() + 1);
//				auto WriteEvent = CreateEventA(nullptr, TRUE, FALSE, event_name.c_str());
//				auto process = newProc(children.size() + 1, argv[0]);
//				children.emplace_back(process, WriteEvent);
//			}
//		}
//		if (GetKeyState(VK_OEM_PLUS) > 0) {//Если кнопка + отжата
//			alreadyAdded = false;
//		}
//		if (GetKeyState(VK_OEM_MINUS) < 0 && !alreadyDeleted) { //Если нажат - и при этом мы ещё не обрабатывали это нажатие
//			alreadyDeleted = true;
//			if (!children.empty()) {
//				const auto child = children.back();
//				TerminateProcess(child.first, EXIT_SUCCESS);
//				ResetEvent(child.second);
//				CloseHandle(child.second);
//				CloseHandle(child.first);
//				children.pop_back();
//				if (children.empty()) {
//					cout << "\nNo more processes running. To add new one press \"+\" or \"q\" to exit" << endl;
//				}
//			}
//			else {
//				cout << "\nNo processes running. To add new one press \"+\" or \"q\" to exit)" << endl;
//			}
//		}
//		if (GetKeyState(VK_OEM_MINUS) > 0) {//Если кнопка - отжата
//			alreadyDeleted = false;
//		}
//		if (!children.empty()) {
//			if (++currentWriting >= children.size()) {
//				currentWriting = 0;
//			}
//			SetEvent(children[currentWriting].second);
//			while (WaitForSingleObject(children[currentWriting].second, 0) == WAIT_OBJECT_0);
//		}
//	}
//}