#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int
main()
{
	char filename[maxlen +1];
	int stdinID, stdoutID;
	int bytes, status;

	stdinID = Open("stdin", 1);
	stdoutID = Open("stdout", 0);

	Write("Filename: ", maxlen, 1);
	bytes = Read(filename, maxlen, stdinID);
	
	status = CreateFile(filename);

	if (status == 1) {
		Write("Success \n", maxlen, 1);
	}
	else if (status == 0) {
		Write("File already exist \n", maxlen, 1);
	}
	else  {
		Write("Fail \n", maxlen, 1);
	}
	
	Close(stdinID);
	Close(stdoutID);
	Halt();
}
