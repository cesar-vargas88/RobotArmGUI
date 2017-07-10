// VE_BrazoRobot.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "VE_BrazoRobot.h"
#include "Command.h"

#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <sstream> 

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Robot(HWND, UINT, WPARAM, LPARAM);

//////////////////////////////////////
//				Robot				//
//////////////////////////////////////

int g_nESB = 0;
int g_nPSB = 0;
int g_nES1 = 0;
int g_nPS1 = 0;
int g_nES2 = 0;
int g_nPS2 = 0;
int g_nES3 = 0;
int g_nPS3 = 0;

void RobotStatus(int nESB, int nES1, int nES2, int nES3, int nPSB, int nPS1, int nPS2, int nPS3);
void StartUSBConection();
void StartDebugConsole();

//////////////////////////////////////////
//			USB	Comunication			//
//////////////////////////////////////////

HANDLE g_hUSB;

bool SendPICCommand(HANDLE g_hUSB,PICCOMMAND* pInCmd)
{
	unsigned char Zero=0;
	DWORD dwBytesWritten=0;
	WriteFile(g_hUSB,&Zero,1,&dwBytesWritten,NULL);  //Debes enviar un byte 0x00 antes de enviar un datagrama
	WriteFile(g_hUSB,&pInCmd->m_bCommandCode,1,&dwBytesWritten,NULL);
	WriteFile(g_hUSB,&pInCmd->m_bParamsBufferSize,1,&dwBytesWritten,NULL);
	if(pInCmd->m_bParamsBufferSize)
	{
		WriteFile(
			g_hUSB,
			pInCmd->m_abParamsBuffer,
			pInCmd->m_bParamsBufferSize,
			&dwBytesWritten,NULL);
	}
	return true;

}
bool ReceivePICCommand(HANDLE g_hUSB,PICCOMMAND* pOutCmd)
{
	DWORD dwBytesWritten=0;
	memset(pOutCmd->m_abParamsBuffer,0,16);
	ReadFile(g_hUSB,&pOutCmd->m_bCommandCode,1,&dwBytesWritten,NULL);
	ReadFile(g_hUSB,&pOutCmd->m_bParamsBufferSize,1,&dwBytesWritten,NULL);
	if(pOutCmd->m_bParamsBufferSize)
	{
		ReadFile(g_hUSB,pOutCmd->m_abParamsBuffer,
			pOutCmd->m_bParamsBufferSize,
			&dwBytesWritten,NULL);
	}
	return true;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//////////////////////////////
	//		Start Console		//
	//////////////////////////////
		
	StartDebugConsole();

	//////////////////////////////
	//		USB Conection		//
	//////////////////////////////
	
	StartUSBConection();

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_VE_BRAZOROBOT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VE_BRAZOROBOT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VE_BRAZOROBOT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_VE_BRAZOROBOT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_ROBOT_CONTROL:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ROBOT), hWnd, Robot);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK Robot(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
			HWND hComboESB;
			HWND hComboPSB;
			HWND hComboES1;
			HWND hComboPS1;
			HWND hComboES2;
			HWND hComboPS2;
			HWND hComboES3;
			HWND hComboPS3;

			char *cEnable[] = {"Yes","No"};
			char *cPosition[] = {"  0","  1","  2","  3","  4","  5","  6","  7","  8","  9",
								 " 10"," 11"," 12"," 13"," 14"," 15"," 16"," 17"," 18"," 19"
								 " 20"," 21"," 22"," 23"," 24"," 25"," 26"," 27"," 28"," 29"
								 " 30"," 31"," 32"," 33"," 34"," 35"," 36"," 37"," 38"," 39"
								 " 40"," 41"," 42"," 43"," 44"," 45"," 46"," 47"," 48"," 49"
								 " 50"," 51"," 52"," 53"," 54"," 55"," 56"," 57"," 58"," 59"
								 " 60"," 61"," 62"," 63"," 64"," 65"," 66"," 67"," 68"," 69"	
								 " 70"," 71"," 72"," 73"," 74"," 75"," 76"," 77"," 78"," 79"
								 " 80"," 81"," 82"," 83"," 84"," 85"," 86"," 87"," 88"," 89"
								 " 90"," 91"," 92"," 93"," 94"," 95"," 96"," 97"," 98"," 99"
								 "100","101","102","103","104","105","106","107","108","109"
								 "110","111","112","113","114","115","116","117","118","119"};

			for (int i=0; i<2; i++)
			{
				hComboESB = GetDlgItem(hDlg, IDC_ESB);
				SendMessageA(hComboESB, CB_ADDSTRING, 0, (LPARAM)cEnable[i]);

				hComboES1 = GetDlgItem(hDlg, IDC_ES1);
				SendMessageA(hComboES1, CB_ADDSTRING, 0, (LPARAM)cEnable[i]);	

				hComboES2 = GetDlgItem(hDlg, IDC_ES2);
				SendMessageA(hComboES2, CB_ADDSTRING, 0, (LPARAM)cEnable[i]);	

				hComboES3 = GetDlgItem(hDlg, IDC_ES3);
				SendMessageA(hComboES3, CB_ADDSTRING, 0, (LPARAM)cEnable[i]);	
			}
				
			return (INT_PTR)TRUE;
		}
	case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			else if (LOWORD(wParam) == IDOK)
			{
				char buffer[256];

				//	Guarda estatus en variables boleanas
				g_nESB = SendMessageA(GetDlgItem(hDlg, IDC_ESB), CB_GETCURSEL, 0, 0);
				g_nES1 = SendMessageA(GetDlgItem(hDlg, IDC_ES1), CB_GETCURSEL, 0, 0);
				g_nES2 = SendMessageA(GetDlgItem(hDlg, IDC_ES2), CB_GETCURSEL, 0, 0);
				g_nES3 = SendMessageA(GetDlgItem(hDlg, IDC_ES3), CB_GETCURSEL, 0, 0);

				//	Guarda seleccionado en variables enteras
				GetDlgItemTextA(hDlg,IDC_PSB,buffer,255);
				g_nPSB = atoi(buffer);
				GetDlgItemTextA(hDlg,IDC_PS1,buffer,255);
				g_nPS1 = atoi(buffer);
				GetDlgItemTextA(hDlg,IDC_PS2,buffer,255);
				g_nPS2 = atoi(buffer);
				GetDlgItemTextA(hDlg,IDC_PS3,buffer,255);
				g_nPS3 = atoi(buffer);

				RobotStatus(g_nESB, g_nES1, g_nES2, g_nES3, g_nPSB, g_nPS1, g_nPS2, g_nPS3);
			}
		}
		break;
	}
	return (INT_PTR)FALSE;
}
void RobotStatus(int nESB, int nES1, int nES2, int nES3, int nPSB, int nPS1, int nPS2, int nPS3)
{
	PICCOMMAND _DSM_Status={COMMAND_DSM_STATUS,16};
	PICCOMMAND _DSM_StatusResponse;

	if(nESB)
		_DSM_Status.m_abParamsBuffer[0] = '1';
	else
		_DSM_Status.m_abParamsBuffer[0] = '0';

	if(nES1)
		_DSM_Status.m_abParamsBuffer[1] = '1';
	else
		_DSM_Status.m_abParamsBuffer[1] = '0';

	if(nES2)
		_DSM_Status.m_abParamsBuffer[2] = '1';
	else
		_DSM_Status.m_abParamsBuffer[2] = '0';

	if(nES3)
		_DSM_Status.m_abParamsBuffer[3] = '1';
	else
		_DSM_Status.m_abParamsBuffer[3] = '0';

	string sBackup;

	string sPSB = static_cast<ostringstream*>( &(ostringstream() << nPSB) )->str();

	if(sPSB.size() > 2)
		_DSM_Status.m_abParamsBuffer[4] = sPSB[0];
	else
	{
		sBackup = sPSB;
		sPSB="0";
		sPSB+=sBackup;
		_DSM_Status.m_abParamsBuffer[4] = '0';
	}

	if(sPSB.size() > 2)
		_DSM_Status.m_abParamsBuffer[5] = sPSB[1];
	else
	{
		sBackup = sPSB;
		sPSB="0";
		sPSB+=sBackup;
		_DSM_Status.m_abParamsBuffer[5] = '0';
	}
	
	_DSM_Status.m_abParamsBuffer[6] = sPSB[2];

	string sPS1 = static_cast<ostringstream*>( &(ostringstream() << nPS1) )->str();
	
	if(sPS1.size() > 2)
		_DSM_Status.m_abParamsBuffer[7] = sPS1[0];
	else
	{
		sBackup = sPS1;
		sPS1="0";
		sPS1+=sBackup;
		_DSM_Status.m_abParamsBuffer[7] = '0';
	}

	if(sPS1.size() > 2)
		_DSM_Status.m_abParamsBuffer[8] = sPS1[1];
	else
	{
		sBackup = sPS1;
		sPS1="0";
		sPS1+=sBackup;
		_DSM_Status.m_abParamsBuffer[8] = '0';
	}
	
	_DSM_Status.m_abParamsBuffer[9] = sPS1[2];

	string sPS2 = static_cast<ostringstream*>( &(ostringstream() << nPS2) )->str();
	
	if(sPS2.size() > 2)
		_DSM_Status.m_abParamsBuffer[10] = sPS2[0];
	else
	{
		sBackup = sPS2;
		sPS2="0";
		sPS2+=sBackup;
		_DSM_Status.m_abParamsBuffer[10] = '0';
	}

	if(sPS2.size() > 2)
		_DSM_Status.m_abParamsBuffer[11] = sPS2[1];
	else
	{
		sBackup = sPS2;
		sPS2="0";
		sPS2+=sBackup;
		_DSM_Status.m_abParamsBuffer[11] = '0';
	}
	
	_DSM_Status.m_abParamsBuffer[12] = sPS2[2];

	string sPS3 = static_cast<ostringstream*>( &(ostringstream() << nPS3) )->str();
	
	if(sPS3.size() > 2)
		_DSM_Status.m_abParamsBuffer[13] = sPS3[0];
	else
	{
		sBackup = sPS3;
		sPS3="0";
		sPS3+=sBackup;
		_DSM_Status.m_abParamsBuffer[13] = '0';
	}

	if(sPS3.size() > 2)
		_DSM_Status.m_abParamsBuffer[14] = sPS3[1];
	else
	{
		sBackup = sPS3;
		sPS3="0";
		sPS3+=sBackup;
		_DSM_Status.m_abParamsBuffer[14] = '0';
	}
	
	_DSM_Status.m_abParamsBuffer[15] = sPS3[2];

	SendPICCommand(g_hUSB,&_DSM_Status);

	ReceivePICCommand(g_hUSB,&_DSM_StatusResponse);
	
	switch(_DSM_StatusResponse.m_bCommandCode)
	{
		case ERROR_PIC_SUCCESS:
			printf("%s",_DSM_Status.m_abParamsBuffer);
			break;
		case ERROR_PIC_UNIMPLEMENTED_COMMAND:
			printf("%s\n","El pic ha respondido, pero no ha ejecuta el comando ya que no lo reconoce");
			printf("%s\n","Verifique que la aplicación esté en ejecución");
			break;
	}	
}
void StartDebugConsole()
{
	BOOL bOk = AllocConsole();
	if(bOk)
	{
		int fd;
		HANDLE hOutput;
		FILE* fp;
		hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		fd = _open_osfhandle((intptr_t)hOutput, _O_TEXT);
		fp = _fdopen(fd,"w");
		*stdout = *fp;
		char *pBuffer=(char*)malloc(32);
		setvbuf(stdout,pBuffer,_IOFBF,32);
		SetConsoleTitle(TEXT("Information and debug Console"));
	}
}
void StartUSBConection()
{
	g_hUSB=CreateFile(
		L"\\\\.\\com10",                //Nombre device
		GENERIC_READ|GENERIC_WRITE,    //Permisos de acceso
		0,							   //Sin compartir
		NULL,                          //Seguridad del usuario actual
		OPEN_EXISTING,				   //Debe existir                
		0,                             //Nada en especial
		NULL);                         //Acceso Mono hilo
	if(g_hUSB==INVALID_HANDLE_VALUE)
	{
		g_hUSB=CreateFile(
			L"\\\\.\\com9",                //Nombre device
			GENERIC_READ|GENERIC_WRITE,    //Permisos de acceso
			0,							   //Sin compartir
			NULL,                          //Seguridad del usuario actual
			OPEN_EXISTING,				   //Debe existir                
			0,                             //Nada en especial
			NULL);                         //Acceso Mono hilo
		if(g_hUSB==INVALID_HANDLE_VALUE)
		{
			g_hUSB=CreateFile(
				L"\\\\.\\com8",                //Nombre device
				GENERIC_READ|GENERIC_WRITE,    //Permisos de acceso
				0,							   //Sin compartir
				NULL,                          //Seguridad del usuario actual
				OPEN_EXISTING,				   //Debe existir                
				0,                             //Nada en especial
				NULL);                         //Acceso Mono hilo
			if(g_hUSB==INVALID_HANDLE_VALUE)
			{
				g_hUSB=CreateFile(
					L"\\\\.\\com7",                //Nombre device
					GENERIC_READ|GENERIC_WRITE,    //Permisos de acceso
					0,							   //Sin compartir
					NULL,                          //Seguridad del usuario actual
					OPEN_EXISTING,				   //Debe existir                
					0,                             //Nada en especial
					NULL);                         //Acceso Mono hilo
				if(g_hUSB==INVALID_HANDLE_VALUE)
				{
					g_hUSB=CreateFile(
						L"\\\\.\\com6",                //Nombre device
						GENERIC_READ|GENERIC_WRITE,    //Permisos de acceso
						0,							   //Sin compartir
						NULL,                          //Seguridad del usuario actual
						OPEN_EXISTING,				   //Debe existir                
						0,                             //Nada en especial
						NULL);                         //Acceso Mono hilo
					if(g_hUSB==INVALID_HANDLE_VALUE)
					{
						g_hUSB=CreateFile(
							L"\\\\.\\com5",                //Nombre device
							GENERIC_READ|GENERIC_WRITE,    //Permisos de acceso
							0,							   //Sin compartir
							NULL,                          //Seguridad del usuario actual
							OPEN_EXISTING,				   //Debe existir                
							0,                             //Nada en especial
							NULL);                         //Acceso Mono hilo
						if(g_hUSB==INVALID_HANDLE_VALUE)
						{
							printf("Conection Problem");
						}
					}
				}
			}
		}
	}
	else
		printf("Conection success");
}