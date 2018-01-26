/* 
* ZeusSetWindowsHookEx.cpp : Holds the mechanism
* required for dll injection using SetWindowsHook
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

/* 
* ZeusSetWindowsHookEx : Injects A Dll using
* SetWindowsHook API.
*
* Requires : A Dll with an exported function named 'Proc'
*
* @Arguments : 
*	@ProcID : Process ID to inject
*	@szDllLoc : Location of the Dll TO Inject
*
* @Return :
*	@Void
*/
int ZeusSetWindowsHookEx(int ProcID, char *szDllLoc)
{
	DWORD ThreadID;

	ThreadID = ZeusGetThreadID(ProcID);

	printf("\n[+] Thread ID In Use : %d", ThreadID);

	HMODULE hModule = LoadLibraryEx(szDllLoc, NULL, DONT_RESOLVE_DLL_REFERENCES);
	if (hModule == NULL)
	{
#ifdef _DEBUG_
		printf("\n[!] Could Not Load Library File Specified !");
#endif
		ExitProcess(1);
	}

	HOOKPROC Addr = (HOOKPROC) GetProcAddress(hModule, "Proc");
	if (Addr == NULL)
	{
#ifdef _DEBUG_
		printf("\n[!] Could Not Get Exported Proc Function !");
#endif
		ExitProcess(1);
	}

	HHOOK Handle = SetWindowsHookEx(WH_KEYBOARD, Addr, hModule, ThreadID);
	if (Handle == NULL)
	{
#ifdef _DEBUG_
		printf("\n[!] Could Not Set Hook Using SetWindowsHookEx !");
#endif
		ExitProcess(1);
	}
	printf("\n[+] Successfully Injected Using SetWindowsHookEx !");
	printf("\n[-] Press Any Enter Any Key To Remove The Hook !");
	getchar();
	UnhookWindowsHookEx(Handle);
	ExitProcess(0);
}
