#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int
main()
{
	char buffer[maxlen +1];
	int stdinID, stdoutID;
	int bytesRead, bytesWrite;
	stdinID = Open("stdin", 1);
	stdoutID = Open("stdout", 0);
	Write("Input: ", maxlen, 1);
	bytesRead = Read(buffer, maxlen, stdinID);
	Write("Output: ", maxlen, 1);
	bytesWrite = Write(buffer, maxlen, stdoutID);
	Close(stdinID);
	Close(stdoutID);
	Halt();
}
