/*
* ZeusCommon.cpp : Contains routines that are used
*  many times throughout the code.
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
#include <TlHelp32.h> //Required for process Enumeration


/*
* ZeusOpenProcess : Opens a process for operation
* @Arguments : 
*	@ProcID : Process Identifier
* 
* @Return :
*	@HANDLE : Handle to target process
*/
HANDLE ZeusOpenProcess(int ProcID)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcID);
	if (hProcess == NULL)
	{
#ifdef _DEBUG_
		printf("\n[!] Error : Could Not Open A Handle to the process %i !", ProcID);
#endif
		ExitProcess(1);
	}
	/*
	* If correct handle, return it.
	*/
	return hProcess;
}

/*
* ZeusWriteDllStrMemory : Writes the Dll Location String
* to a process memory. It uses the local API's to write a 
* dll location string. 
* 
* @Arguments : 
*	@szDllLoc : Dll Location String
*	@hProcess : HANDLE to the Target Process
*
* @Return :
*	@LPVOID : A Pointer to the dll string location in target process
*/
LPVOID ZeusWriteDllStrMemory(char *szDllLoc, HANDLE hProcess)
{
	/*
	* Allocate some memory for dll location string
	*/
	LPVOID arg = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(szDllLoc), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(arg==NULL)
	{
#ifdef _DEBUG_
		printf("\n[!] Error : Could Not Allocate Memory for Dll !");
#endif
		ExitProcess(1);
	}
	printf("\n[+] Allocated %d Bytes in Target Process !", strlen(szDllLoc));

	/*
	* Write the location to the target process's memory
	*/
	int ret = WriteProcessMemory(hProcess, arg, szDllLoc, strlen(szDllLoc), NULL);
	if(ret == 0)
	{
#ifdef _DEBUG_
		printf("\n[!] Error : No bytes were written to the target process !");
#endif
		ExitProcess(1);
	}
	printf("\n[+] Wrote %d Bytes to the Target Process !", strlen(szDllLoc));

	return arg;
}

/*
* ZeusSetDebugPrivileges : Enables SeDebugPrivilege on
* the program. Requires Administrator Rights
* 
* @Arg : None
: @Ret : None
*/
int ZeusSetDebugPrivileges(void) { 
	TOKEN_PRIVILEGES priv = {0};
	HANDLE hToken = NULL; 

	/*
	* Open Token for adjusting privileges and then 
	* apply the new token using Adjust Token privileges
	* NOTE : Not my own code. Taken from github
	*/
	if( OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) ) {
			priv.PrivilegeCount           = 1;
			priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			if( LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid ) ) {
				if(AdjustTokenPrivileges( hToken, FALSE, &priv, 0, NULL, NULL ) == 0) {
#ifdef _DEBUG_
					printf("\n[!] Error : An error occured Setting Debug Privileges ! [%u]\n",GetLastError());
#endif
					ExitProcess(1);
				} 
			}	
			printf("\n[+] Successfully enabled SeDebugPrivilege !");
			CloseHandle( hToken );
		} 
		return GetLastError();
}

/*
* ZeusListProcess : Lists all the processes running
* on the system.
* It prints all the processes with their process id .
*
* @Arg and @Ret : Void
*/
int ZeusListProcess()
{
   PROCESSENTRY32 pe;
   HANDLE thSnapshot;
   BOOL retval, ProcFound = FALSE;

   thSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

   if(thSnapshot == INVALID_HANDLE_VALUE)
   {
#ifdef _DEBUG_
	   printf("\n[!] Error: unable to create toolhelp snapshot");
#endif
      ExitProcess(1);
   }

   pe.dwSize = sizeof(PROCESSENTRY32);

   retval = Process32First(thSnapshot, &pe);
   printf("\n\n[*] Process List :-\n");
   while(retval)
   {
	  /*
	  * Prints the running process Here. It could
	  * be used in any kind of program
	  */
      printf("\n[+] Process Name : %s", pe.szExeFile);
	  printf("\t Process ID : %d", pe.th32ProcessID);
      retval    = Process32Next(thSnapshot,&pe);
      pe.dwSize = sizeof(PROCESSENTRY32);
   }
} 

/*
* ZeusGetProcessIdFromName : Returns the process ID Of any
* given process given it's name.
* 
* @Arguments :
*	@procName : A String containing name of the process
*
* @Return :
*	@pe.th32ProcessId : Process Id Associated with that process
*/
unsigned long ZeusGetProcessIdFromName(char *procName)
{
   PROCESSENTRY32 pe;
   HANDLE thSnapshot;
   BOOL retval, ProcFound = FALSE;

   thSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

   if(thSnapshot == INVALID_HANDLE_VALUE)
   {
#ifdef _DEBUG_
      printf("\n[!] Error: unable to create toolhelp snapshot");
#endif
      ExitProcess(1);
   }

   pe.dwSize = sizeof(PROCESSENTRY32);

    retval = Process32First(thSnapshot, &pe);

   while(retval)
   {
	   /*
	   * Compare the String given with enumerated 
	   * one.
	   */
      if(strstr(pe.szExeFile, procName) )
      {
		  // Found
         ProcFound = TRUE;
         break;
      }

      retval    = Process32Next(thSnapshot,&pe);
      pe.dwSize = sizeof(PROCESSENTRY32);
   }

   return pe.th32ProcessID;
} 

/*
* ZeusPrintInfo : Prints the current version
* and other information of the operating system.
*
* @Arg And @Ret : VOID
*/
void ZeusPrintInfo()
{
	char szMachine[128];
	char szUser[128];

	DWORD dw;

	OSVERSIONINFO os_version;

	os_version.dwOSVersionInfoSize = sizeof(os_version);

	if (GetVersionEx(&os_version)) 
	{
		if (os_version.dwMajorVersion == 5) 
		{
#ifdef _DEBUG_
			printf("\n[+] OS version: Windows XP");
#endif
		}
		else if (os_version.dwMajorVersion == 6 && os_version.dwMinorVersion == 0) 
		{
#ifdef _DEBUG_
			printf("\n[+] OS version: Windows Vista");
#endif
		}
		else if (os_version.dwMajorVersion == 6 && os_version.dwMinorVersion == 1)
		{
#ifdef _DEBUG_
			printf("\n[+] OS version: Windows 7");
#endif
		}
	}
	else
		printf("\n[-] OS version detect failed.");

	GetComputerName(szMachine, &dw);
	printf("\n[+] Computer Name : %s", szMachine);
	GetUserName(szUser, &dw);
	printf("\n[+] User Name : %s", szUser);

	return;
}

/*
* ZeusGetThreadID : Get's a thread ID from
* a process.
*
* @Args : Process ID
* @Ret : Thread ID
*/
DWORD ZeusGetThreadID(DWORD pid)
{
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h != INVALID_HANDLE_VALUE)
	{
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		if (Thread32First(h, &te))
		{
			do
			{
				if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
				{
					if (te.th32OwnerProcessID == pid)
					{
						HANDLE hThread = OpenThread(READ_CONTROL, FALSE, te.th32ThreadID);
						if (!hThread)
						{
							printf("\n[-] Error: Couldn't get thread handle\n");
							ExitProcess(1);
						}
						else
							return te.th32ThreadID;
					}
				}
			} while (Thread32Next(h, &te));
		}
	}

	CloseHandle(h);
	return (DWORD)0;
}
