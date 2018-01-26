/*
* Zeus.h : Common header file for Zeus Injector
* Written By : @Ice3man
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
*  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
*  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
*  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <Windows.h>
#include <stdio.h>
#include "ntdll.h"

/*
* Used for differentiating between debug
* and release builds. 
* Debug build offers verbose debug information
*/
#define _DEBUG_ 

/*
* Prototypes for NTCreateThreadEx : Thanks to InjectAllTheThings
* project.
*/

struct NtCreateThreadExBuffer {
	ULONG Size;
	ULONG Unknown1;
	ULONG Unknown2;
	PULONG Unknown3;
	ULONG Unknown4;
	ULONG Unknown5;
	ULONG Unknown6;
	PULONG Unknown7;
	ULONG Unknown8;
};

typedef NTSTATUS(WINAPI *LPFUN_NtCreateThreadEx) (
	PHANDLE hThread,
	ACCESS_MASK DesiredAccess,
	LPVOID ObjectAttributes,
	HANDLE ProcessHandle,
	LPTHREAD_START_ROUTINE lpStartAddress,
	LPVOID lpParameter,
	BOOL CreateSuspended,
	ULONG StackZeroBits,
	ULONG SizeOfStackCommit,
	ULONG SizeOfStackReserve,
	LPVOID lpBytesBuffer
	);

/*
* Acquires a handle to the process id specified
*/
HANDLE ZeusOpenProcess(int ProcID);
/*
* Writes the Dll Location String to target process memory
*/
LPVOID ZeusWriteDllStrMemory(char *szDllLoc, HANDLE hProcess);
/*
* Enables SeDebugPrivileges for playing with system processes
*/
int ZeusSetDebugPrivileges(void); //Requires Admin Rights
/*
* Lists all the processes running on the system
*/
int ZeusListProcess();
/*
* Get's process's ID From Process Name
*/
unsigned long ZeusGetProcessIdFromName(char *procName);
/*
* Prints the Current OS And Other Details Like username and machine name
*/
void ZeusPrintInfo();

/*
* Creates a Remote Thread Using CreateRemoteThread (Most Primitive)
*/
int ZeusCreateRemoteThread(int ProcID, char *szDllLoc);

/*
* Creates A Remote Thread Using RtlCreateUserThread
*/
int ZeusRtlCreateUserThread(int ProcID, char *szDllLoc);

/*
* Creates A Remote Thread using NtCreateThreadEx
*/
int ZeusNtCreateThreadEx(int ProcID, char *szDllLoc);

/*
* Gets a thread ID From A PID
*/
DWORD ZeusGetThreadID(DWORD pid);

/*
* Injects Dll Using SetWindowsHookEx
*/
int ZeusSetWindowsHookEx(int ProcID, char *szDllLoc);

/*
* Injects a Dll Using QueueUserAPC
*/
int ZeusQueueUserAPC(int ProcID, char *szDllLoc);
