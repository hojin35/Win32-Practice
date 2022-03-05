#include<Windows.h>
#include<windowsx.h>
#include<tchar.h>
#include<iostream>
#include<string>
#include<thread>
#include<random>
#include<vector>
#include<chrono>

namespace ch = std::chrono;
using std::thread;

bool a = true;
int cnt = 0;
ch::time_point<ch::high_resolution_clock> start;
ch::time_point<ch::high_resolution_clock> end;
RECT rect;
HINSTANCE g_hinst;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND CreateFullscreenWindow(HWND hwnd);
void DrawRandomCircle(HWND hwnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	g_hinst = hInstance;
	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Learn to Program Windows",
		WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,
		NULL,
		hInstance,
		NULL
	);


	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	thread t1(DrawRandomCircle, hwnd);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	t1.join();

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	{

		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		if (rect.top <= yPos && rect.bottom >= yPos && rect.left <= xPos && rect.right >= xPos)
		{
			InvalidateRect(hwnd, &rect, TRUE);
			UpdateWindow(hwnd);
			rect = { 0,0,0,0 };
			auto diff = ch::high_resolution_clock::now() - start;
			HDC hdc = GetDC(hwnd);

			TCHAR str[128];
			wsprintf(str, TEXT("점수 : %d 반응 속도 : %d ms"), cnt += 10, ch::duration_cast<ch::milliseconds>(diff).count());
			TextOut(hdc, 10, 10, str, lstrlen(str));

			ReleaseDC(hwnd, hdc);
			break;
		}

		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);

		break;
	}
	case WM_CLOSE:

		if (MessageBox(hwnd, L"프로그램을 종료 하시겠습니까?", L"My application", MB_OKCANCEL) == IDOK)
		{
			DestroyWindow(hwnd);
			a = false;
		}

		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void DrawRandomCircle(HWND hwnd)
{

	RECT r1;
	GetWindowRect(hwnd, &r1);

	// 랜덤 설정
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> disX(r1.left, r1.right - 100);
	std::uniform_int_distribution<int> disY(r1.top, r1.bottom - 100);
	int colorCnt = 0;

	int colors[10][3] =
	{
		{255,0,0},
		{0,255,0},
		{0,0,255},
	};
	int colorSelect = 0;
	start = ch::high_resolution_clock::now();
	end = start;
	while (a == true)
	{
		HDC hdc;
		HPEN myPen = CreatePen(PS_SOLID, 10, RGB(colors[colorSelect][0], colors[colorSelect][1], colors[colorSelect][2]));

		hdc = GetDC(hwnd);
		int x = disX(gen);
		int y = disY(gen);
		SelectObject(hdc, myPen);
		rect = { x,y,x + 100,y + 100 };
		Rectangle(hdc, x + 5, y + 5, x + 95, y + 95);
		DeleteObject(myPen);

		ReleaseDC(hwnd, hdc);
		colorSelect = (colorSelect + 1) % 3;

		auto diff = end - start;
		do
		{
			end = ch::high_resolution_clock::now();
			diff = end - start;
		} while (ch::duration_cast<ch::milliseconds>(diff).count() <= 1000);

		start = end;
		if (!(rect.bottom == 0 && rect.top == 0 && rect.left == 0 && rect.right == 0))
		{
			InvalidateRect(hwnd, &rect, TRUE);
			UpdateWindow(hwnd);
			
		}
	}
}
