#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int
main()
{
	char filename[maxlen +1];
	int stdinID, stdoutID;
	int status;

	stdinID = Open("stdin", 1);
	stdoutID = Open("stdout", 0);

	Write("Filename: ", maxlen, 1);
	Read(filename, maxlen, stdinID);

	if (Delete(filename) == 0) {
		Write("Success \n", maxlen, 1);
	}
	else  {
		Write("Fail \n", maxlen, 1);
	}
	
	Close(stdinID);
	Close(stdoutID);
	Halt();
}
