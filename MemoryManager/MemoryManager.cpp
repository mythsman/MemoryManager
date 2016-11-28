#include<windows.h>
#include<stdio.h>
#include <commctrl.h>
#include <tchar.h>
#include <psapi.h>
#include"resource.h"
HINSTANCE topInstance;

LPWSTR ConvertCharToLPWSTR(char * szString)
{
	int dwLen = strlen(szString) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);
	LPWSTR lpszPath = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);
	return lpszPath;
}
void work(HWND hList){
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded);
	cProcesses = cbNeeded / sizeof(DWORD);
	for (unsigned int i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			LVITEM LvItem;
			memset(&LvItem, 0, sizeof(LvItem));
			LvItem.mask = LVIF_TEXT; 
			LvItem.cchTextMax = 256; 
			LvItem.iItem = 0;   
			LvItem.iSubItem = 0;    
			LvItem.pszText = L""; 

			DWORD cb=NULL;
			HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, aProcesses[i]);
			if (NULL != hProcess ){	
				TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
				char Temp[255]="";
				PROCESS_MEMORY_COUNTERS pmc;
				if(GetModuleBaseName( hProcess, NULL, szProcessName,sizeof(szProcessName)/sizeof(TCHAR) )){
					memset(&LvItem,0,sizeof(LvItem));
					LvItem.mask=LVIF_TEXT; 
					LvItem.cchTextMax = 256; 
					LvItem.iItem=0;

					LvItem.iSubItem=0;
					LvItem.pszText=szProcessName;
					SendMessage(hList,LVM_INSERTITEM,0,(LPARAM)&LvItem);

					LvItem.iSubItem=1;
					sprintf(Temp,"%d",aProcesses[i]);
					LvItem.pszText=ConvertCharToLPWSTR(Temp);
					SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem);

					if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))){
						LvItem.iSubItem=2;
						sprintf(Temp,"%d",pmc.PageFaultCount);
						LvItem.pszText=ConvertCharToLPWSTR(Temp);
						SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem);

						LvItem.iSubItem=3;
						sprintf(Temp,"%d",pmc.PeakWorkingSetSize);
						LvItem.pszText=ConvertCharToLPWSTR(Temp);
						SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem);

						LvItem.iSubItem=4;
						sprintf(Temp,"%d",pmc.WorkingSetSize);
						LvItem.pszText=ConvertCharToLPWSTR(Temp);
						SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem);

						LvItem.iSubItem=5;
						sprintf(Temp,"%d",pmc.QuotaPeakPagedPoolUsage);
						LvItem.pszText=ConvertCharToLPWSTR(Temp);
						SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem);

						LvItem.iSubItem=6;
						sprintf(Temp,"%d",pmc.QuotaPagedPoolUsage);
						LvItem.pszText=ConvertCharToLPWSTR(Temp);
						SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem);

						LvItem.iSubItem=7;
						sprintf(Temp,"%d",pmc.QuotaPeakNonPagedPoolUsage);
						LvItem.pszText=ConvertCharToLPWSTR(Temp);
						SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem); 

						LvItem.iSubItem=8;
						sprintf(Temp,"%d",pmc.QuotaNonPagedPoolUsage);
						LvItem.pszText=ConvertCharToLPWSTR(Temp);
						SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem);

						LvItem.iSubItem=9;
						sprintf(Temp,"%d",pmc.PagefileUsage);
						LvItem.pszText=ConvertCharToLPWSTR(Temp);
						SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem);

						LvItem.iSubItem=10;
						sprintf(Temp,"%d",pmc.PeakPagefileUsage);
						LvItem.pszText=ConvertCharToLPWSTR(Temp);
						SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem);
					}
				}
			}
		}
	}
}
INT_PTR CALLBACK DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE)
		{
			DestroyWindow(hdlg);
			PostQuitMessage(0);
		}
		return 0;
	case WM_INITDIALOG:
		{

			HWND hList = GetDlgItem(hdlg, IDC_LIST);
			SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE,
				0, LVS_EX_FULLROWSELECT);
			LVCOLUMN LvCol;
			ZeroMemory(&LvCol, sizeof(LvCol));
			WCHAR columnName[11][50] = { L"Name", L"PID", L"PageFaultCount",
				L"PeakWorkingSetSize", L"WorkingSetSize",
				L"QuotaPeakPagedPoolUsage", L"QuotaPagedPoolUsage",
				L"QuotaPeakNonPagedPoolUsage", L"QuotaNonPagedPoolUsage",
				L"PagefileUsage", L"PeakPagefileUsage" };
			for (int i = 0; i < 11; i++){
				LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
				LvCol.pszText = columnName[i];
				LvCol.cx = 66;
				SendMessage(hList, LVM_INSERTCOLUMN, i, (LPARAM)&LvCol);
			}
			work(hList);
		}
		return 0;
	default:
		return (INT_PTR)FALSE;
	}
}

LRESULT CALLBACK WindowProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	switch (uMsg){
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	case WM_CREATE:
		{
			HWND hdlg = CreateDialog(topInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)DlgProc);
			if (hdlg)
			{
				ShowWindow(hdlg, SW_NORMAL);
			}
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	WCHAR* cls_Name = L"MemoryManager";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = cls_Name;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);

	topInstance = hInstance;

	HWND hwnd = CreateWindow(
		cls_Name,           //类名，要和刚才注册的一致  
		L"MemoryManager",          //窗口标题文字 
		WS_OVERLAPPEDWINDOW,        //窗口外观样式  
		0,             //窗口相对于父级的X坐标  
		0,             //窗口相对于父级的Y坐标  
		0,                //窗口的宽度  
		0,                //窗口的高度  
		NULL,               //没有父窗口，为NULL
		NULL,               //没有菜单，为NULL  
		hInstance,          //当前应用程序的实例句柄  
		NULL);              //没有附加数据，为NULL  
	if (hwnd == NULL){                //检查窗口是否创建成功 
		return 0;
	}
	ShowWindow(hwnd, SW_HIDE);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}