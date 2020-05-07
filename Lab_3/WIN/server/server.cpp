#include <iostream>
#include <Windows.h>

using namespace std;

int main()
{
	auto pipe = CreateNamedPipeA("",0,0,1,1,1,1, nullptr);

	ConnectNamedPipe(pipe, nullptr);
}