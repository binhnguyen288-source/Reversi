#include <Windows.h>
#include <tchar.h>
#include <algorithm>
#include <chrono>
#include "AI.h"



#define side 110
#define rows 8
#define cols 8


void fillcircle(HDC hdc, int i, int j)
{
    Ellipse(hdc, side * i, side * j, side * i + side, side * j + side);
}
void clear(HWND hwnd)
{
    HDC hdc = GetDC(hwnd);
    SetROP2(hdc, R2_WHITE);
    Rectangle(hdc, 0, 0, 1000, 1000);
    ReleaseDC(hwnd, hdc);
}
BB x, o;
void drawline(HDC hdc, int x1, int y1, int x2, int y2)
{
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    using namespace std::chrono;
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        resetKH();
        POINTS p = MAKEPOINTS(lparam);
        int i = p.x / side;
        int j = p.y / side;
        if (i > -1 && j > -1 && i < 8 && j < 8) 
        {
            makemove(x, o, set(j * 8 + i));
            clear(hwnd);

            HDC hdc = GetDC(hwnd);
            for (int i = 0; i <= rows; ++i)
                drawline(hdc, 0, i * side, side * cols, i * side);
            for (int i = 0; i <= cols; ++i)
                drawline(hdc, i * side, 0, i * side, rows * side);

            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                {
                    if (x & set(8 * i + j))
                        fillcircle(hdc, j, i);
                }
            SetROP2(hdc, R2_BLACK);
            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                {
                    if (o & set(8 * i + j))
                        fillcircle(hdc, j, i);
                }
            ReleaseDC(hwnd, hdc);
            int score = 0;
            uint64_t time = 0;
            int depth;
            for (depth = 1; score != mate && depth <= 5; depth += 2)
            {
                auto s = steady_clock::now();
                score = search(o, x, -mate, mate, depth, 0, false);
                auto e = steady_clock::now();
                time += duration_cast<milliseconds>(e - s).count();
            }
            if (score == -mate) exit(0);
            makemove(o, x, getbestmove());
            clear(hwnd);

            hdc = GetDC(hwnd);
            for (int i = 0; i <= rows; ++i)
                drawline(hdc, 0, i * side, side * cols, i * side);
            for (int i = 0; i <= cols; ++i)
                drawline(hdc, i * side, 0, i * side, rows * side);

            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                {
                    if (x & set(8 * i + j))
                        fillcircle(hdc, j, i);
                }
            SetROP2(hdc, R2_BLACK);
            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                {
                    if (o & set(8 * i + j))
                        fillcircle(hdc, j, i);
                }
            ReleaseDC(hwnd, hdc);
        }
        else if (i == 8 || j == 8)
        {
            int score = 0;
            uint64_t time = 0;
            int depth;
            for (depth = 1; score != mate && depth <= 5; depth += 2)
            {
                auto s = steady_clock::now();
                score = search(o, x, -mate, mate, depth, 0, true);
                auto e = steady_clock::now();
                time += duration_cast<milliseconds>(e - s).count();
            }
            if (score == -mate) exit(0);
            makemove(o, x, getbestmove());
            clear(hwnd);

            HDC hdc = GetDC(hwnd);
            for (int i = 0; i <= rows; ++i)
                drawline(hdc, 0, i * side, side * cols, i * side);
            for (int i = 0; i <= cols; ++i)
                drawline(hdc, i * side, 0, i * side, rows * side);

            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                {
                    if (x & set(8 * i + j))
                        fillcircle(hdc, j, i);
                }
            SetROP2(hdc, R2_BLACK);
            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                {
                    if (o & set(8 * i + j))
                        fillcircle(hdc, j, i);
                }
            ReleaseDC(hwnd, hdc);
        }
    }
        break;
    case WM_PAINT:
    {
        x = set(28) | set(35);
        o = set(27) | set(36);
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        for (int i = 0; i <= rows; ++i)
        {
            drawline(hdc, 0, i * side, side * cols, i * side);
        }
        for (int i = 0; i <= cols; ++i)
            drawline(hdc, i * side, 0, i * side, rows * side);
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
            {
                if (x & set(8 * i + j))
                    fillcircle(hdc, j, i);
            }
        SetROP2(hdc, R2_BLACK);
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
            {
                if (o & set(8 * i + j))
                    fillcircle(hdc, j, i);
            }
        EndPaint(hwnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}
/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR args, int ncmdline)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"class";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   // wc.style = CS_HREDRAW | CS_VREDRAW;
    if (!RegisterClass(&wc)) return -1;
    HWND hwnd = CreateWindow(L"class", L"Reversi", WS_OVERLAPPEDWINDOW,
        0, 0, 1500, 1100,
        NULL, NULL, hInstance, NULL);
    UpdateWindow(hwnd);
    ShowWindow(hwnd, ncmdline);
    MSG msg;
    msg.message = WM_NULL;
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
        {
            DispatchMessage(&msg);
        }
        else
        {

        }
    }
    return 0;
    }*/
#include <iostream>
int main()
{
    search(set(28) | set(35), set(27) | set(36), -mate, mate, 15, 0, false);
    std::cout << getnode();
    
    return 0;
}