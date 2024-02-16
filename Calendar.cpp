#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN  
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib,"Comctl32.lib")

#include "resource.h"
#include <string>
#include <windowsx.h>
#include <windows.h>
#include <commctrl.h>
#include <wchar.h>
#include <strsafe.h>
#include <list>

#include <SDKDDKVer.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


HINSTANCE hInst;
HWND hText{};
HWND hSelect_date{};
HWND hMonth_calendar{};
HWND hwnd_dother{};
HWND hDate{};
const int limit = 100;
wchar_t buf_date[12];
wchar_t buf_event[limit];

//структура хранения события
struct Event {
	wchar_t date[12];
	wchar_t event[limit];
}EV;
std::list <Event>events;
std::list <Event>events_temp;
void if_event() {	//проверка существования события на выбранную дату
	for (auto ev : events) {
		if (!std::wstring(buf_date).compare(std::wstring(ev.date).c_str())) {
			SetWindowText(hText, ev.event);
			return;
		}
	}
}
void create_events() {	//создание нового события
	events_temp.clear();
	for (auto ev : events)
	{
		if (std::wstring(buf_date).compare(std::wstring(ev.date).c_str())) {
			events_temp.push_back(ev);
		}
	};
	events.clear();
	for (auto ev : events_temp) {
		events.push_back(ev);
	}
	wcscpy(EV.date, buf_date);
	GetWindowText(hText, EV.event, sizeof(EV.event));
	events.push_back(EV);
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcDother(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	MSG msg{};
	HWND hwnd{};
	WNDCLASSEX wc{};
	wc.hInstance = hInstance;
	wc.lpszClassName = L"Родитель";
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALLICON));
	wc.lpszMenuName = MAKEINTRESOURCE(IDC_MENU);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.cbSize = sizeof(WNDCLASSEX);
	hInst = hInstance;
	if (!RegisterClassEx(&wc)) return EXIT_FAILURE;
	hwnd = CreateWindow(wc.lpszClassName, L"Календарь", WS_OVERLAPPED | WS_SYSMENU, 450, 280, 179, 240, nullptr, nullptr, wc.hInstance, nullptr);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	//обработка сообщений главного окна
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		INITCOMMONCONTROLSEX icex;
		icex.dwICC = ICC_DATE_CLASSES;
		InitCommonControlsEx(&icex);
		hMonth_calendar = CreateWindow(MONTHCAL_CLASS, L"Календарь на месяц", WS_CHILD | WS_VISIBLE, 0, 0, 163, 163, hwnd, NULL, hInst, NULL);
		hSelect_date = CreateWindow(WC_STATIC, L"Выбранная дата", WS_BORDER | WS_CHILD | WS_VISIBLE, 0, 160, 74, 20, hwnd, NULL, hInst, NULL);
		HWND hBottonCreate = CreateWindow(L"BUTTON", L"Событие", WS_BORDER | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 74, 160, 88, 20, hwnd, (HMENU)IDM_EVENT, hInst, NULL);
		break;
	}
	return 0;
	case WM_NOTIFY:
	{
		SYSTEMTIME time;
		SendMessage(hMonth_calendar, MCM_GETCURSEL, 0, (LPARAM)&time);
		StringCbPrintfW(buf_date, 12 * sizeof(wchar_t), L"%d-%d-%d", time.wYear, time.wMonth, time.wDay);
		SetWindowText(hSelect_date, buf_date);
		break;
	}
	return 0;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDM_EXIT:
		{
			DestroyWindow(hwnd);
		}
		break;
		case IDM_ABOUT:
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
		}
		break;
		case IDM_EVENT:
		{
			SetWindowText(hDate, buf_date);
			if_event();
			ShowWindow(hwnd_dother, SW_RESTORE);

			MSG msgD{};
			WNDCLASSEX wc_dother{};
			wc_dother.lpszClassName = L"Дочь";
			wc_dother.lpfnWndProc = WndProcDother;
			wc_dother.cbClsExtra = 0;
			wc_dother.cbWndExtra = 0;
			wc_dother.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
			wc_dother.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wc_dother.style = CS_VREDRAW | CS_HREDRAW;
			wc_dother.cbSize = sizeof(WNDCLASSEX);
			if (!RegisterClassEx(&wc_dother)) return EXIT_FAILURE;
			hwnd_dother = CreateWindow(wc_dother.lpszClassName, L"Создание События", WS_OVERLAPPED | WS_SYSMENU, 629, 350, 300, 80, hwnd, nullptr, nullptr, nullptr);

			ShowWindow(hwnd_dother, SW_SHOWDEFAULT);
			UpdateWindow(hwnd_dother);
		}
		break;
		}
	}
	return 0;
	case WM_DESTROY:
	{
		PostQuitMessage(EXIT_SUCCESS);
	}
	return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK WndProcDother(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	//обработка сообщений дочернего окна
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		hText = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 284, 20, hwnd, NULL, hInst, NULL);
		SendMessage(hText, EM_LIMITTEXT, (limit - 1), 0L);
		hDate = CreateWindow(WC_STATIC, L"Дата события", WS_BORDER | WS_CHILD | WS_VISIBLE, 0, 20, 74, 20, hwnd, NULL, hInst, NULL);
		SetWindowText(hDate, buf_date);
		HWND hBottonOk = CreateWindow(L"BUTTON", L"Создать событие", WS_BORDER | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 75, 20, 140, 20, hwnd, (HMENU)2, hInst, NULL);
		HWND hBottonCancel = CreateWindow(L"BUTTON", L"Отмена", WS_BORDER | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 214, 20, 70, 20, hwnd, (HMENU)3, hInst, NULL);
	}
	return 0;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case 2:
		{
			create_events();
			MessageBox(hwnd, L"Событие записано!", L"Уведомление", MB_OK | MB_ICONINFORMATION);
			SetWindowText(hText, L"");
			ShowWindow(hwnd, SW_HIDE);
		}
		break;
		case 3:
		{
			SetWindowText(hText, L"");
			ShowWindow(hwnd, SW_HIDE);
		}
		break;
		}
	}
	return 0;
	case WM_CLOSE:
	{
		SetWindowText(hText, L"");
		ShowWindow(hwnd, SW_HIDE);
	}
	return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}