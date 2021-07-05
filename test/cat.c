#include "syscall.h"
#include "copyright.h"
#include "progtest.h"
#define maxlen 32

int
main()
{
	char filename[maxlen +1];
	char* content;
	int stdinID, stdoutID, fileID;
	int fileLen;
	stdinID = Open("stdin", 1);
	stdoutID = Open("stdout", 0);

	Write("Enter filename: ", maxlen, 1);
	Read(filename, maxlen, stdinID);

	fileID = Open(filename, 1);

	if (fileID != -1) {
		fileLen = Seek(-1, fileID);
		Seek(0, fileID);
		Read(content, fileLen, fileID);
		Write("Content: ", maxlen, 1);
		Write(content, fileLen, stdoutID);
		Close(fileID);
	}

	Close(stdinID);
	Close(stdoutID);

	Halt();
}
