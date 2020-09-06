#pragma once
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

struct FileInfo
{
	HANDLE fileHeader;
	DWORD numberOfBytes;
	CHAR  buffer[100];
	DWORD  positionInFile;
	DWORD  positionOutFile;
	OVERLAPPED Overlapped;
} fileInfo;

HINSTANCE dynamicLibrary;

HANDLE readerCompleted;
HANDLE readerStop;
HANDLE writerCompleted;

DWORD WINAPI threadWriter(PVOID);
DWORD WINAPI threadReader(PVOID);

inline HANDLE create_events()
{
	writerCompleted = CreateEvent(nullptr, FALSE, TRUE, nullptr);
	readerCompleted = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	readerStop = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	return CreateEvent(nullptr, FALSE, FALSE, TEXT("_main_event_"));
}

inline void close_events(HANDLE eventHandler)
{
	CloseHandle(writerCompleted);
	CloseHandle(readerCompleted);
	CloseHandle(readerStop);
	CloseHandle(eventHandler);
}

DWORD WINAPI threadReader(PVOID path)
{
	WIN32_FIND_DATA findFileData;
	HANDLE readFileHandle = nullptr;
	auto readResult = FALSE;
	const string folder(((const char*)path));
	const auto fileMask = folder + "*.txt";
	char readFilePath[MAX_PATH];
	auto findHandle = FindFirstFile(fileMask.c_str(), &findFileData);
	auto Read = (BOOL(*)(FileInfo*))GetProcAddress(dynamicLibrary, "readFromFile");
	if (findHandle == INVALID_HANDLE_VALUE) {
		return EXIT_FAILURE;
	}
	while (true)
	{
		WaitForSingleObject(writerCompleted, INFINITE);
		if (readResult == FALSE)
		{
			fileInfo.positionInFile = 0;
			strcpy_s(readFilePath, folder.c_str());
			strcat_s(readFilePath, findFileData.cFileName);
			readFileHandle =
				CreateFile(readFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
		}
		fileInfo.fileHeader = readFileHandle;
		if (fileInfo.fileHeader == INVALID_HANDLE_VALUE) {
			FindClose(findHandle);
			CloseHandle(readFileHandle);
			SetEvent(readerStop);
			return EXIT_FAILURE;
		}
		readResult = (Read)(&fileInfo);
		if (!readResult && GetLastError() == ERROR_HANDLE_EOF)
		{
			if (FindNextFile(findHandle, &findFileData))
			{
				CloseHandle(readFileHandle);
				SetEvent(writerCompleted);
				continue;
			}
			break;
		}
		SetEvent(readerCompleted);
	}
	FindClose(findHandle);
	CloseHandle(readFileHandle);
	SetEvent(readerStop);
	return EXIT_SUCCESS;
}

DWORD WINAPI threadWriter(PVOID path)
{
	HANDLE outFileHandle = CreateFile((const char*)path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, nullptr);
	auto Write = (BOOL(*)(FileInfo*))GetProcAddress(dynamicLibrary, "writeToFile");
	HANDLE eventsOfReadAndStopRead[2] = { readerCompleted, readerStop };
	const auto stopEvent = 1;
	while (true) {
		const auto currentEvent = WaitForMultipleObjects(2, eventsOfReadAndStopRead, FALSE, INFINITE) - WAIT_OBJECT_0;
		if (currentEvent == stopEvent) {
			break;
		}
		fileInfo.fileHeader = outFileHandle;
		(Write)(&fileInfo);
		SetEvent(writerCompleted);
	}
	CloseHandle(outFileHandle);
	return EXIT_SUCCESS;
}