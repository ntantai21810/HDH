#include "syscall.h"
#define maxlen 256

void main() {
	int pingID, pongID;
	Open("stdout", 0);
	Write("Ping-Pong test starting ... \n\n", maxlen, 1);
	pingID = Exec("./test/Ping");
	pongID = Exec("./test/Pong");
	while(1) {};
}
