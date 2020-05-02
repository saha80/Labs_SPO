#include <iostream>
#include <vector>
#include <conio.h>
#include <Windows.h>

using namespace std;

LPCRITICAL_SECTION mutex;

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	return {};
}

int main()
{
	//CreateThread();
	//PMYDATA pDataArray[MAX_THREADS];
	vector<HANDLE> threads;

	for (auto &thread : threads)
	{
		DWORD thread_id;
		thread = CreateThread(nullptr, 0, MyThreadFunction, HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(1)), 0, &thread_id);
	}

	auto is_running = true;
	while (is_running)
	{
		if (_kbhit())
		{
			const auto c = _getch();
			if (c == '+')
			{
				//todo: add thread
			}
			if (c == '-')
			{
				//todo: remove thread
			}
			if (c == 'q')
			{
				is_running = false;
				//todo: close threads
			}
			//todo:
		}
	}
}
