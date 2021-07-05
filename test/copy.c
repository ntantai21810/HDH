#include "syscall.h"
#include "copyright.h"
#define maxlen 32

int
main()
{
	char fileSrc[maxlen +1];
	char fileDest[maxlen + 1];
	char* content;
	int stdinID, stdoutID, fileSrcID, fileDestID;
	int fileSrcLen;

	stdinID = Open("stdin", 1);
	stdoutID = Open("stdout", 0);

	Write("Enter src: ", maxlen, stdoutID);
	Read(fileSrc, maxlen, stdinID);

	Write("Enter dest: ", maxlen, stdoutID);
	Read(fileDest, maxlen, stdinID);


	fileSrcID = Open(fileSrc, 1);
	
	if (fileSrcID != -1) {
		fileSrcLen = Seek(-1, fileSrcID);

		Seek(0, fileSrcID);

		Read(content, fileSrcLen, fileSrcID);

		if (CreateFile(fileDest) != -1) {
			fileDestID = Open(fileDest, 0);
			Write(content, fileSrcLen, fileDestID);
			Write("Success \n", maxlen, stdoutID);
			Close(fileDestID);
		}
		Close(fileSrcID);
	}

	Close(stdinID);
	Close(stdoutID);
	
	

	Halt();
}
