// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void IncreaseProgramCounter() 
{
	machine->registers[PrevPCReg] = machine->registers[PCReg]; 
	machine->registers[PCReg] = machine->registers[NextPCReg];
	machine->registers[NextPCReg] += 4;
	
}


char* User2System(int virtAddr,int limit)
{
	int i;// index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit +1];//need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf,0,limit+1);
	//printf("\n Filename u2s:");
	for (i = 0 ; i < limit ;i++)
	{
		machine->ReadMem(virtAddr+i,1,&oneChar);
		kernelBuf[i] = (char)oneChar;
		//printf("%c",kernelBuf[i]);
		if (oneChar == 0)
		break;
	}
	return kernelBuf;
}

int System2User(int virtAddr,int len,char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0 ;
	do{
		oneChar= (int) buffer[i];
		machine->WriteMem(virtAddr+i,1,oneChar);
		i ++;
	}while(i < len && oneChar != 0);
	return i;
}

char* intToChar(int num) {
	int temp = num;
	char* charNum = NULL;
	int digits = 0;
	bool isNegative = false;
	int index = 0;
	int i;

	if (num == 0) {
		charNum = new char[2];
		charNum[0] = '0';
		charNum[1] = '\0';
		return charNum;
	}

	if (num < 0) {
		num *= -1;
		temp *= -1;
		isNegative = true;
		index++;
	}

	while (temp > 0) {
		digits++;
		temp/= 10;
	} 

	charNum = new char[digits + isNegative + 1];

	charNum[digits + isNegative] = '\0';	

	for (i = digits + index - 1; i >= index; i--) {
		charNum[i] = num % 10 + '0';
		num /= 10;
	}

	if (isNegative) charNum[0] = '-';

	return charNum;

}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
		int MaxFileLength = 32;

    switch (which) {
			case NoException:
				return;

			case PageFaultException:
				DEBUG('a', "\n No valid translation found");
				printf("\n\n No valid translation found");
				interrupt->Halt();
				return;
			case ReadOnlyException:
				DEBUG('a', "\n Write attempted to page marked read-only");
				printf("\n\n Write attempted to page marked read-only");
				interrupt->Halt();
				return;
			case BusErrorException:
				DEBUG('a', "\n Translation resulted invalid physical address");
				printf("\n\n Translation resulted invalid physical address");
				interrupt->Halt();
				return;
			case AddressErrorException:
				DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
				printf("\n\n Unaligned reference or one that was beyond the end of the address space");
				interrupt->Halt();
				return;
			case OverflowException:
				DEBUG('a', "\nInteger overflow in add or sub.");
				printf("\n\n Integer overflow in add or sub.");
				interrupt->Halt();
				return;
			case IllegalInstrException:
				DEBUG('a', "\n Unimplemented or reserved instr.");
				printf("\n\n Unimplemented or reserved instr.");
				interrupt->Halt();
				return;
			case NumExceptionTypes:
				DEBUG('a', "\n Number exception types");
				printf("\n\n Number exception types");
				interrupt->Halt();
				return;
	
			case SyscallException: {
				//printf("\n");
				switch (type) {
					case SC_Halt: {
						DEBUG('a', "\n Shutdown, initiated by user program.");
						printf ("\n\n Shutdown, initiated by user program.");
						interrupt->Halt();
						break;
					}
					case SC_CreateFile: {
						//Input name
						//Output: 
						/* 
							-1: Error
							0: File is already exist
							1: Success 	
						*/
						int virtAddr;
						char* filename;
						DEBUG('a',"\n SC_Createfile call ...");
						DEBUG('a',"\n Reading virtual address of filename");
						// Lấy tham số tên tập tin từ thanh ghi r4
						virtAddr = machine->ReadRegister(4);
						DEBUG ('a',"\n Reading filename.");
						// MaxFileLength là = 32
						filename = User2System(virtAddr,MaxFileLength+1);
						if (filename == NULL)
						{
							printf("\n Not enough memory in system");
							DEBUG('a',"\n Not enough memory in system");
							IncreaseProgramCounter();
							machine->WriteRegister(2,-1); // trả về lỗi cho chương
							// trình người dùng
							delete filename;
							return;
						}

						if (filename[0] == '\0') {
							printf("File name is not valid \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2,-1); // trả về lỗi cho chương
							// trình người dùng
							delete filename;
							return;
						}
						DEBUG('a',"\n Finish reading filename.");
						//DEBUG(‘a’,"\n File name : '"<<filename<<"'");
						// Create file with size = 0
						// Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
						// việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
						// hành Linux, chúng ta không quản ly trực tiếp các block trên
						// đĩa cứng cấp phát cho file, việc quản ly các block của file
						// trên ổ đĩa là một đồ án khác
						
						switch(fileSystem->Create(filename, 0)) {
							case -1: 
								//printf("Create file %s fail \n", filename);
								machine->WriteRegister(2, -1);
								break;
							case 0: 
								//printf("File is already exist \n");
								machine->WriteRegister(2, 0);
								break;
							case 1:
								//printf("Create %s successfully \n", filename);
								machine->WriteRegister(2, 1);
								break;
						}
						IncreaseProgramCounter();
						delete filename;
						return;
					 }
					case SC_Open: {
						//Input: name, type
						//Output: OpenFileID cua file do neu mo thanh cong;  -1 neu that bai
						int virtAddr, openType, freeSlot;
						char* filename;
						DEBUG('a',"\n SC_Open call ...");
						DEBUG('a',"\n Reading virtual address of filename");
						// Lấy tham số tên tập tin từ thanh ghi r4
						virtAddr = machine->ReadRegister(4);
						//Lay type tu thanh ghi so 5
						openType = machine->ReadRegister(5);
							
						DEBUG ('a',"\n Reading filename.");
						// MaxFileLength là = 32
						filename = User2System(virtAddr,MaxFileLength+1);

						//Kiem tra type dau vao
						if (openType < 0 || openType > 1) {
							printf("Open type is not valid \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							delete filename;
							return;
						}


						if (filename == NULL)
						{
							printf("\n Not enough memory in system");
							DEBUG('a',"\n Not enough memory in system");	
							IncreaseProgramCounter();
							machine->WriteRegister(2,-1); // trả về lỗi cho chương
							// trình người dùng
							delete filename;
							return;
						}

						if (filename[0] == '\0') {
							printf("File name is not valid \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2,-1); 
							delete filename;
							return;
						}			

						DEBUG('a',"\n Finish reading filename.");
						//DEBUG(‘a’,"\n File name : '"<<filename<<"'");
						// Open file
						// Dùng đối tượng fileSystem của lớp OpenFile để mo file

						//Truong hop mo sdtin va sdtout
						if (strcmp(filename, "stdin") == 0) {
							if (openType == 0) {
								printf("sdtin type must be 1 \n");
								IncreaseProgramCounter();
								machine->WriteRegister(2, -1);
								return;
							}
							//Neu file va type hop le, tien hanh ghi file vao bang dac ta
							fileSystem->files[0] = fileSystem->Open(filename, openType);
							//printf("Open %s successfully \n", filename);
							IncreaseProgramCounter();
							machine->WriteRegister(2, 0);	
							delete filename;	
							return;		
						}
						else if (strcmp(filename, "stdout") == 0) {
							if (openType == 1) {
								printf("sdtin type must be 0 \n");
								IncreaseProgramCounter();
								machine->WriteRegister(2, -1);
								return;
							}
							//Neu file va type hop le, tien hanh ghi file vao bang dac ta
							fileSystem->files[1] = fileSystem->Open(filename, openType);
							//printf("Open %s successfully \n", filename);
							IncreaseProgramCounter();
							machine->WriteRegister(2, 1);
							delete filename;
							return;
						}

						//Truong hop mo file
						//Tim slot con trong o bang dac ta files
						freeSlot = fileSystem->findFreeSlot();
						//Neu bang dac ta da day 
						IncreaseProgramCounter();
						if (freeSlot == -1) {
							printf("Maximum opening files \n");
							machine->WriteRegister(2, -1);
						}
						else {
							//Cap nhat file vao bang dac ta
							fileSystem->files[freeSlot] = fileSystem->Open(filename, openType);
							//Neu file khong ton tai tra ve loi
							if (fileSystem->files[freeSlot] == NULL) {
								printf("File %s not found \n", filename);
								machine->WriteRegister(2, -1);
							}	
							else
							//Neu thanh cong tra ve vi tri trong bang dac ta
								//printf("Open %s successfully \n", filename);
								machine->WriteRegister(2, freeSlot);
						}
					
						delete filename;
						return;
					}
					
					case SC_Close: {
					//Input: OpenFileID cua file
					//Output: 0 neu thanh cong; 1 neu that bai

						//Kiem tra tham so dau vao

						//Lay openFileID tu thanh ghi so 4
						int openFileID = machine->ReadRegister(4);
						//Kiem tra tinh hop le cua openFileID
						if (openFileID < 0 || openFileID >= 10) {
							printf("ID is not valid \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							return;
						}

						IncreaseProgramCounter();
						//Truong hop file chua duoc mo
						if (fileSystem->files[openFileID] == NULL) {
							printf("File is not opening \n");
							machine->WriteRegister(2, -1);
						}
						else {
							//File dang duoc mo
							//Xoa thong tin file khoi bang dac ta
							//printf("Close successfully \n");
							delete fileSystem->files[openFileID];
							fileSystem->files[openFileID] = NULL;					
							machine->WriteRegister(2, openFileID);
						}
						return;
					} 
					case SC_Read: {

						//Lay cac tham so dau vao
						int buffer = machine->ReadRegister(4);
						int charCount = machine->ReadRegister(5);
						int id = machine->ReadRegister(6);
						char* temp;

						//Kiem tra dau vao
						if (id < 0 || id >= 10) {
								printf("Id out of range \n");
								IncreaseProgramCounter();
								machine->WriteRegister(2, -1);
								return;
						}

						//Kiem tra file dang mo
						if (fileSystem->files[id] == NULL) {
							printf("File isn't opened \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							return;
						}

						IncreaseProgramCounter();
						//Console IO

						//Truong hop doc stdout
						if (id == 1) {
							printf("Cannot read stdout \n");
							machine->WriteRegister(2, -1);
							return;
						}

						temp = User2System(buffer, charCount);
						//Truong hop doc stdin
						if (id == 0) {
							//Doc stdin
							int bytes = gSynchConsole->Read(temp, charCount);
							//Lay du lieu tu kernel space
							System2User(buffer, bytes, temp);
							machine->WriteRegister(2, bytes);
							delete temp;
							return;
						}

						//File
						else {
							//Doc file
							int bytes = fileSystem->files[id]->Read(temp, charCount);
							//printf("Read file successfully \n");
							//Chua o cuoi file
							if (bytes > 0) {
								//Lay du lieu tu kernel space
								System2User(buffer, bytes, temp);
								machine->WriteRegister(2, bytes);
							}
							//Cuoi file
							else {
								machine->WriteRegister(2, -2);
							}
						}
						delete temp;
						return;
					}

					case SC_Write: {
						//Lay cac tham so dau vao
						int buffer = machine->ReadRegister(4);
						int charCount = machine->ReadRegister(5);
						int id = machine->ReadRegister(6);
						char* temp;

						//Kiem tra dau vao
						if (id < 0 || id >= 10) {
								printf("Id out of range \n");
								IncreaseProgramCounter();
								machine->WriteRegister(2, -1);
								return;
						}

						//Kiem tra file dang mo
						if (fileSystem->files[id] == NULL) {
							printf("File is not opened \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							return;
						}

						//Kiem tra stdin
						if (id == 0) {
							printf("Cannot write to stdin \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							return;
						}

						//Kiem tra type cua file
						if (fileSystem->files[id]->openType == 1) {
							printf("File is read-only \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							return;
						}

						temp = User2System(buffer, charCount);

						IncreaseProgramCounter();

						//Stdout
						if (id == 1) {
							int bytes = gSynchConsole->Write(temp, charCount);
							machine->WriteRegister(2, bytes);
						}
						//File
						else {
							int bytes = fileSystem->files[id]->Write(temp, charCount);
							//printf("Write file successfully \n");
							machine->WriteRegister(2, bytes);					
						}

						delete temp;
						return;
					}

					case SC_Seek: {
						int pos = machine->ReadRegister(4);
						int id = machine->ReadRegister(5);

						if (id < 0 || id >= 10) {
							printf("ID out of range \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							return;
						}

						if (id == 0 || id == 1) {
							printf("Cannot seek on stdin, stdout \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							return;
						}

						if (fileSystem->files[id] == NULL) {
							printf("File is not opened \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							return;
						}

						if ((pos < 0 && pos != -1) || pos > fileSystem->files[id]->Length()) {
							printf("Pos is not valid \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							return;
						}

						if (pos == -1) {
							fileSystem->files[id]->Seek(fileSystem->files[id]->Length());
						}
						else {
							fileSystem->files[id]->Seek(pos);
						}

						IncreaseProgramCounter();
						machine->WriteRegister(2, fileSystem->files[id]->getCurrentOffSet());
						return;

					}

					case SC_Delete: {
						int virtAddr = machine->ReadRegister(4);
						char* filename = NULL;
						int i, fileID;

						filename = User2System(virtAddr,MaxFileLength+1);
						if (filename == NULL || filename[0] == '\0') {
							printf("File name is not valid \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							delete filename;
							return;
						}

						if (strcmp(filename, "stdout") == 0 || strcmp(filename, "stdin") == 0) {
							printf("Cannot delete stdin, stdout \n");
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							delete filename;
							return;
						}

						for (i = 2; i < 10; i++) {
							if (fileSystem->files[i] != NULL && strcmp(filename, fileSystem->files[i]->name) == 0) {
								printf("File %s is opening \n", filename);
								IncreaseProgramCounter();
								machine->WriteRegister(2, -1);
								delete filename;
								return;
							}
						}

						fileID = OpenForReadWrite(filename, false);
						
						if (fileID == -1) {
							printf("File %s is not exist \n", filename);
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							delete filename;
							return;
						}

						Close(fileID);

						if (fileSystem->Remove(filename)) {
							//printf("Delete file %s successfully \n", filename);
							IncreaseProgramCounter();
							machine->WriteRegister(2, 0);
							delete filename;
							return;
						}
						else {
							//printf("Delete file %s failed \n", filename);
							IncreaseProgramCounter();
							machine->WriteRegister(2, -1);
							delete filename;
							return;
						}
						
					}
					 default: {
						printf("\n Unexpected user mode exception (%d %d)", which,
						type);
						interrupt->Halt();
					}
					
				}
			}
		}
}
