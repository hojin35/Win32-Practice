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
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void DrawCircle(HWND hwnd);
void DrawRandomCircle(HWND hwnd);
RECT rect;
bool a = true;

std::vector<RECT> rectangles;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

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

		rect.left = xPos;
		rect.top = yPos;
		return 0;
	}
	case WM_LBUTTONUP:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		rect.right = xPos;
		rect.bottom = yPos;
		DrawCircle(hwnd);
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
void DrawCircle(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
	ReleaseDC(hwnd, hdc);
}

void DrawRandomCircle(HWND hwnd)
{
	RECT r1;
	GetWindowRect(hwnd, &r1);

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
	ch::time_point<ch::high_resolution_clock> start = ch::high_resolution_clock::now();
	ch::time_point<ch::high_resolution_clock> end;
	while (a == true)
	{
		HDC hdc;
		for (int i = 0; i < 10; i++)
		{
			HPEN myPen = CreatePen(PS_SOLID, 5, RGB(colors[colorSelect][0], colors[colorSelect][1], colors[colorSelect][2]));

			hdc = GetDC(hwnd);
			int x = disX(gen);
			int y = disY(gen);
			SelectObject(hdc, myPen);
			RECT rect = { x,y,x + 100,y + 100 };
			Rectangle(hdc, x, y, x + 100, y + 100);
			rectangles.push_back(rect);
			DeleteObject(myPen);
			ReleaseDC(hwnd, hdc);
			colorSelect = (colorSelect + 1) % 3;
		}
		auto diff = start - start;
		do
		{
			end = ch::high_resolution_clock::now();
			diff = end - start;
		} while (ch::duration_cast<ch::milliseconds>(diff).count() <= 1000);

		InvalidateRect(hwnd, NULL, TRUE);

		/*for (auto& rect : rectangles)
		{
			Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
		}*/
		start = end;
	}
}