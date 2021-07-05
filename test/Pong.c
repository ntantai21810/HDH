#include "syscall.h"

void main() {
	int i;
	//Open("stdout", 0);
	for (i = 0; i < 1000; i++) {
		Write("B", 1, 1);
	}
}
