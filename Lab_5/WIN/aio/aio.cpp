#include "pch.h"  
#include <windows.h>

#ifdef __cplusplus 
extern "C" {
#endif

	struct FileInfo
	{
		HANDLE fileHeader;
		DWORD numberOfBytes;
		CHAR  buffer[100];
		DWORD  positionInFile;
		DWORD  positionOutFile;
		OVERLAPPED Overlapped;
	};

	__declspec(dllexport) BOOL readFromFile(FileInfo* fileInfo)
	{
		BOOL readResult;
		DWORD numberOfBytesTransferred;

		fileInfo->Overlapped.Offset = fileInfo->positionInFile;
		ReadFile(fileInfo->fileHeader, fileInfo->buffer, fileInfo->numberOfBytes, NULL, &fileInfo->Overlapped);
		readResult = GetOverlappedResult(fileInfo->fileHeader, &fileInfo->Overlapped, &numberOfBytesTransferred, TRUE);
		if (readResult) {
			fileInfo->positionInFile = fileInfo->positionInFile + numberOfBytesTransferred;
		}
		return readResult;
	}
	__declspec(dllexport) BOOL writeToFile(FileInfo* fileInfo)
	{
		BOOL writeResult;
		DWORD numberOfBytesTransferred;

		fileInfo->Overlapped.Offset = fileInfo->positionOutFile;
		WriteFile(fileInfo->fileHeader, fileInfo->buffer, fileInfo->Overlapped.InternalHigh, NULL, &fileInfo->Overlapped);
		writeResult = GetOverlappedResult(fileInfo->fileHeader, &fileInfo->Overlapped, &numberOfBytesTransferred, TRUE);
		if (writeResult) {
			fileInfo->positionOutFile = fileInfo->positionOutFile + numberOfBytesTransferred;
		}
		return writeResult;
	}
#ifdef __cplusplus
}
#endif