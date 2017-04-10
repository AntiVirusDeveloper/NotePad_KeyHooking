#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

#define DEF_PROCESS_NAME "notepad.exe"
#define KEY_GET_SUCESS 1

HINSTANCE g_hInstance = NULL;
HHOOK g_hHook = NULL;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = hinstDLL;
		break;

	case DLL_PROCESS_DETACH:
		break;

	default:
		break;
	}

	return TRUE;
}

LRESULT CALLBACK KeyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	char *p = NULL;
	char szPath[MAX_PATH] = { 0, };

	if (nCode >= 0)
	{
		GetModuleFileNameA(NULL, szPath, MAX_PATH);

		p=strrchr(szPath, '\\');

		if (!stricmp(p + 1, DEF_PROCESS_NAME))
		{
			HWND hKey = FindWindow(NULL, TEXT("KeyHooking"));
			
			if (hKey != NULL)
			{
				PostMessage(hKey, WM_USER + 1, wParam, lParam);
			}
		}
	}

	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

#ifdef __cplusplus
extern "C"{
#endif

	__declspec(dllexport) void Install_Hook()
	{
		g_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyProc, g_hInstance, 0);

		if (FindWindow(NULL, TEXT("KeyHooking")) == NULL)
			printf("Not RUN KeyHooking, Please run KeyHooking...\n");

	}

	__declspec(dllexport) void UnInstall_Hook()
	{
		if (g_hHook)
		{
			UnhookWindowsHookEx(g_hHook);
			g_hHook = NULL;
		}
	}

#ifdef __cplusplus
}
#endif
