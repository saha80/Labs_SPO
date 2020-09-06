#include "FileThreads.h"

int main()
{
	HANDLE readAndWriteThreads[2];  
	HANDLE handleEvent = create_events();
	fileInfo.Overlapped.Offset = 0;
	fileInfo.Overlapped.OffsetHigh = 0;
	fileInfo.Overlapped.hEvent = handleEvent;
	fileInfo.positionOutFile = 0;
	fileInfo.numberOfBytes = sizeof(fileInfo.buffer);

	dynamicLibrary = LoadLibraryA("aio.dll");
	
	cout << "Start parsing!\n";
	readAndWriteThreads[0] = CreateThread(nullptr, 0, threadWriter, (LPVOID)("output.txt"), 0, nullptr);
	readAndWriteThreads[1] = CreateThread(nullptr, 0, threadReader, 
		(LPVOID)(R"(C:\Users\Alex\source\repos\Labs_SPO\Lab_5\WIN\Lab_5\)"), 0, nullptr);

	WaitForMultipleObjects(2, readAndWriteThreads, TRUE, INFINITE);

	CloseHandle(readAndWriteThreads[0]);
	CloseHandle(readAndWriteThreads[1]);
	close_events(handleEvent);
	FreeLibrary(dynamicLibrary);
	cout << "Operation complete!";
	return 0;
}