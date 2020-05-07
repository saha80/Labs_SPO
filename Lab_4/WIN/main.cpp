#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <Windows.h>

CRITICAL_SECTION critical_section;

using namespace std;

using handle_heap = std::pair<HANDLE, size_t*>;

DWORD WINAPI PrintFunction(LPVOID param)
{
	const auto str = "thread " + to_string(*static_cast<size_t*>(param));
	while (true)
	{
		EnterCriticalSection(&critical_section);
		for (auto i : str) {
			cout << i;
		}
		cout << endl;
		LeaveCriticalSection(&critical_section);
		Sleep(200);
	}
}

handle_heap new_thread(const int n)
{
	auto heap = static_cast<size_t*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(size_t)));
	if (!heap) {
		return {};
	}
	*heap = n;
	DWORD thread_id;
	return  { CreateThread(nullptr, 0, PrintFunction, heap, 0, &thread_id), heap };
}

void close_thread(handle_heap &t)
{
	TerminateThread(t.first, EXIT_SUCCESS);
	CloseHandle(t.first);
	HeapFree(GetProcessHeap(), 0, t.second);
	t.second = nullptr;
}

void close_all_threads(vector<handle_heap> &v)
{
	while (!v.empty()) {
		close_thread(v.back());
		v.pop_back();
	}
}

int main()
{
	InitializeCriticalSection(&critical_section);
	vector<handle_heap> threads;
	while (true)
	{
		auto c = 0;
		if (_kbhit()) {
			c = tolower(_getch());
		}
		if (c == '+') {
			const auto thread = new_thread(threads.size());
			if (!thread.first || !thread.second) {
				cerr << "error code " << GetLastError() << endl;
				close_all_threads(threads);
			}
			threads.push_back(thread);
		}
		if (c == '-') {
			if (!threads.empty()) {
				close_thread(threads.back());
				threads.pop_back();
			}
			else {
				cout << "can't close last thread" << endl;
			}
		}
		if (c == 'q') {
			close_all_threads(threads);
			break;
		}
	}
	DeleteCriticalSection(&critical_section);
}