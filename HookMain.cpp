#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <windows.h>

#define DEF_DLL_NAME "KeyHook.dll"
#define DEF_HOOKSTART "Install_Hook"
#define DEF_HOOKSTOP "UnInstall_Hook"
#define DEF_PROCESS_NAME "notepad.exe"

typedef void(*PFN_HOOKSTART)();
typedef void(*PFN_HOOKSTOP)();

int main(int argc, char** argv) {

	HMODULE hDll = NULL;
	PFN_HOOKSTART HookStart = NULL;
	PFN_HOOKSTOP HookStop = NULL;

	hDll = LoadLibraryA(DEF_DLL_NAME);
	if (hDll == NULL)
	{
		printf("[ERROR] %s Loading Failed..\n", DEF_DLL_NAME);
		return 0;
	}

	HookStart = (PFN_HOOKSTART)GetProcAddress(hDll, DEF_HOOKSTART);
	if (HookStart == NULL)
	{
		printf("[ERROR] %s Function Loding Failed..\n", DEF_HOOKSTART);
		return 0;
	}

	HookStop = (PFN_HOOKSTOP)GetProcAddress(hDll, DEF_HOOKSTOP);
	if (HookStop == NULL)
	{
		printf("[ERROR] %s Function Loding Failed..\n", DEF_HOOKSTOP);
		return 0;
	}

	HookStart();
	
	if (FindWindow(NULL, TEXT("KeyHooking")) == NULL)
	{
		printf("[ERROR] KeyHooking.exe Not RUN\n");
		goto EXIT;
	}
	
	printf("'q'==quit\n");
	while (_getch() != 'q')
	{	
		if (FindWindow(NULL, TEXT("KeyHooking")) == NULL)
		{
			printf("[ERROR] KeyHooking.exe Not RUN\n");
			goto EXIT;
		}
	}

	EXIT:

	HookStop();
	
	FreeLibrary(hDll);

	return 0;
}

