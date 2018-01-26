/* 
* ZeusQueueUserAPC.cpp : Holds the mechanism
* required for dll injection using QueueUserAPC
* Method.
*
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


#include "Zeus.h"
#include <TlHelp32.h>


/* 
* ZeusQueueUserAPC: Injects A Dll using
* QueueUserAPC API. One of the best techniques
* And also not very well known.
*
* @Arguments : 
*	@ProcID : Process ID to inject
*	@szDllLoc : Location of the Dll TO Inject
*
* @Return :
*	@Void
*/
int ZeusQueueUserAPC(int ProcID, char *szDllLoc)
{
	DWORD ThreadID = 0, Result = 0;

	HANDLE hProcess = ZeusOpenProcess(ProcID);
	LPVOID arg = ZeusWriteDllStrMemory(szDllLoc, hProcess);

	LPVOID addr = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	if (addr == NULL)
	{
#ifdef _DEBUG_
		printf("\n[!] Error : Could not get Library Address !");
#endif
		ExitProcess(1);
	}
	printf("\n[+] Got Address Of LoadLibraryA Successfully !");

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		printf("\n[!] Error : Could Not Get Thread Information !");
		ExitProcess(1);
	}

	THREADENTRY32 threadEntry;
	threadEntry.dwSize = sizeof(THREADENTRY32);

	DWORD bResult = Thread32First(hSnapshot, &threadEntry);
	while (bResult)
	{
		bResult = Thread32Next(hSnapshot, &threadEntry);
		if (bResult)
		{
			if (threadEntry.th32OwnerProcessID == ProcID)
			{
					ThreadID = threadEntry.th32ThreadID;

					/*
					* Some APC May Not Trigger Immediately and some may not trigger at all
					* So, we try all threads and randomly fire at them.
					* Keep trying till you succeed
					*/
					printf("\n[+] Using thread: %i\n", ThreadID);
					HANDLE hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, ThreadID);
					if (hThread == NULL)
					{
						printf("\n[!] Error : Could Not Open Thread ! Trying other ...");
					}
					else
					{
						// Queue the APC
						DWORD dwResult = QueueUserAPC((PAPCFUNC)addr, hThread, (ULONG_PTR) arg);
						if (!dwResult)
						{
							printf("\n[!] Error : Could Not Queue User APC ! Trying other Threads...");
						}
						else 
						{
							printf("\n[+] Successfully Queued User APC !");
						}
						CloseHandle(hThread);
					}
			
			}
		}
	}
	ExitProcess(0);
}
