// WinApi_Lab1.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "WinApi_Lab1.h"
#include <time.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szBoxClass[MAX_LOADSTRING];               // the box window class name
RECT rcSize;
HWND g_hWnd;
HWND g_hWnd_ScoreBar;
HWND g_hWnd_Clone;
HWND g_hWnd_ScoreBar_Clone;
HWND g_hWnd_Boxes[16];
HWND g_hWnd_Boxes_Clone[16];
BOOL status;
static int tiles[16];

HBRUSH hbrBackground, hbrBox;
HBRUSH hbrTile2, hbrTile4, hbrTile8, hbrTile16;
HBRUSH hbrTile32, hbrTile64, hbrTile128, hbrTile256;
HBRUSH hbrTile512, hbrTile1024, hbrTile2048;
HPEN pen;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                MyRegisterClassBox(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void                ClearBoard();
bool                CheckBoard();
void                Spawn2();
void                PaintBox(HWND hWnd, HBRUSH hbr, const WCHAR s[]);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    srand(time(0));
    // Creating brushes
    hbrBackground = CreateSolidBrush(RGB(250, 247, 238));
    hbrBox        = CreateSolidBrush(RGB(204, 192, 174));
    // ---
    hbrTile2      = CreateSolidBrush(RGB(238, 228, 198));
    hbrTile4      = CreateSolidBrush(RGB(239, 225, 218));
    hbrTile8      = CreateSolidBrush(RGB(243, 179, 124));
    hbrTile16     = CreateSolidBrush(RGB(246, 153, 100));
    hbrTile32     = CreateSolidBrush(RGB(246, 125, 98));
    hbrTile64     = CreateSolidBrush(RGB(247, 93, 60));
    hbrTile128    = CreateSolidBrush(RGB(237, 206, 116));
    hbrTile256    = CreateSolidBrush(RGB(239, 204, 98));
    hbrTile512    = CreateSolidBrush(RGB(243, 201, 85));
    hbrTile1024   = CreateSolidBrush(RGB(238, 200, 72));
    hbrTile2048   = CreateSolidBrush(RGB(239, 192, 47));
    pen           = CreatePen(PS_SOLID, 2, RGB(250, 247, 238));

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINAPILAB1, szWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDS_BOXCLASS, szBoxClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    MyRegisterClassBox(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPILAB1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
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
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_2048));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = hbrBackground;
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINAPILAB1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_2048));

    return RegisterClassExW(&wcex);
}

//
//  FUNCTION: MyRegisterClassBox()
//
//  PURPOSE: Registers the box class.
//
ATOM MyRegisterClassBox(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = DefWindowProcW;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_2048));
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINAPILAB1);
    wcex.hbrBackground = hbrBox;
    wcex.lpszClassName = szBoxClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_2048));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates windows
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the program windows.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   
   HWND hWnd = g_hWnd = CreateWindowExW(0, szWindowClass, szTitle, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   HWND hWnd_Clone = g_hWnd_Clone = CreateWindowExW(WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOPMOST, szWindowClass, szTitle, WS_CAPTION | WS_VISIBLE,
       CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   SetLayeredWindowAttributes(hWnd_Clone, 0, (255 * 100) / 100, LWA_ALPHA);

   HWND hWnd_ScoreBar = g_hWnd_ScoreBar = CreateWindowExW(0, szBoxClass, L"ScoreBar", WS_CHILD | WS_VISIBLE, 10, 10, 270, 60, hWnd, nullptr, hInstance, nullptr);
   HWND hWnd_ScoreBar_Clone = g_hWnd_ScoreBar_Clone = CreateWindowExW(0, szBoxClass, L"ScoreBar", WS_CHILD | WS_VISIBLE, 10, 10, 270, 60, hWnd_Clone, nullptr, hInstance, nullptr);

   for (int i = 0; i < 4; ++i) {
           for (int j = 0; j < 4; ++j) {
               g_hWnd_Boxes[i+j*4] = CreateWindowExW(0, szBoxClass, L"Box", WS_CHILD | WS_VISIBLE, 10 + i * 70, 80 + j * 70, 60, 60, hWnd, nullptr, hInstance, nullptr);
           }
       }

   for (int i = 0; i < 4; ++i) {
       for (int j = 0; j < 4; ++j) {
           g_hWnd_Boxes_Clone[i + j * 4] = CreateWindowExW(0, szBoxClass, L"Box", WS_CHILD | WS_VISIBLE, 10 + i * 70, 80 + j * 70, 60, 60, hWnd_Clone, nullptr, hInstance, nullptr);
       }
   }

   ClearBoard();

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   ShowWindow(hWnd_ScoreBar, nCmdShow);
   UpdateWindow(hWnd_ScoreBar);
   for (int i = 0; i < 16; ++i)
   {
       ShowWindow(g_hWnd_Boxes[i], nCmdShow);
       UpdateWindow(g_hWnd_Boxes[i]);
   }

   ShowWindow(hWnd_Clone, nCmdShow);
   UpdateWindow(hWnd_Clone);
   ShowWindow(hWnd_ScoreBar_Clone, nCmdShow);
   UpdateWindow(hWnd_ScoreBar_Clone);
   for (int i = 0; i < 16; ++i)
   {
       ShowWindow(g_hWnd_Boxes_Clone[i], nCmdShow);
       UpdateWindow(g_hWnd_Boxes_Clone[i]);
   }

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND       - process the application menu
//  WM_GETMINMAXINFO - set the size of the window
//  WM_PAINT         - Paint the main window
//  WM_MOVE          - synchronize position of windows relative to screen center
//  WM_DESTROY       - delete brushes, post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_GAME_NEWGAME:
                {
                    //ClearBoard();
                    Spawn2();
                }
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_GETMINMAXINFO:
    {
        SetRect(&rcSize, 0, 0, 290, 360);
        AdjustWindowRect(&rcSize, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE, TRUE);
        MINMAXINFO* minMaxInfo = (MINMAXINFO*)lParam;
        minMaxInfo->ptMaxSize.x = minMaxInfo->ptMaxTrackSize.x = rcSize.right - rcSize.left;
        minMaxInfo->ptMaxSize.y = minMaxInfo->ptMaxTrackSize.y = rcSize.bottom - rcSize.top;
        minMaxInfo->ptMinTrackSize.x = rcSize.right - rcSize.left;
        minMaxInfo->ptMinTrackSize.y = rcSize.bottom - rcSize.top;
    }
    break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_MOVE:
    {
        //RECT Rect;
        //GetWindowRect(hWnd, &Rect);
        //MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&Rect, 2);
        RECT rc, rc_main, rc_clone;
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rc, 0);
        int centerX = (rc.left + rc.right + 1) / 2;
        int centerY = (rc.top + rc.bottom + 1) / 2;
        int xPos = (int)(short)LOWORD(lParam);   // horizontal position 
        int yPos = (int)(short)HIWORD(lParam);   // vertical position 
        if (hWnd == g_hWnd)
            MoveWindow(g_hWnd_Clone, 2 * centerX - xPos - rcSize.right - rcSize.left, 2 * centerY - yPos - rcSize.bottom - rcSize.top, rcSize.right - rcSize.left, rcSize.bottom - rcSize.top, FALSE);
        else if (hWnd == g_hWnd_Clone)
            MoveWindow(g_hWnd, 2 * centerX - xPos - rcSize.right - rcSize.left, 2 * centerY - yPos - rcSize.bottom - rcSize.top, rcSize.right - rcSize.left, rcSize.bottom - rcSize.top, FALSE);
        GetWindowRect(g_hWnd, &rc_main);
        GetWindowRect(g_hWnd_Clone, &rc_clone);
        //MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&rc_main, 2);
        //MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&rc_clone, 2);
        if (IntersectRect(&rc, &rc_main, &rc_clone))
            SetLayeredWindowAttributes(g_hWnd_Clone, 0, (255 * 50) / 100, LWA_ALPHA);
        else
            SetLayeredWindowAttributes(g_hWnd_Clone, 0, (255 * 100) / 100, LWA_ALPHA);
        //WCHAR main_title[256];
        //WCHAR clone_title[256];
        //wsprintf(main_title, L"x: %d\ty: %d", rc_main.left, rc_main.top);
        //SetWindowTextW(g_hWnd, main_title);
        //wsprintf(clone_title, L"x: %d\ty: %d", rc_clone.left, rc_clone.top);
        //SetWindowTextW(g_hWnd_Clone, clone_title);
    }
    break;
    case WM_DESTROY:
        DeleteObject(hbrBackground);
        DeleteObject(hbrBox);
        DeleteObject(hbrTile2);
        DeleteObject(hbrTile4);
        DeleteObject(hbrTile8);
        DeleteObject(hbrTile16);
        DeleteObject(hbrTile32);
        DeleteObject(hbrTile64);
        DeleteObject(hbrTile128);
        DeleteObject(hbrTile256);
        DeleteObject(hbrTile512);
        DeleteObject(hbrTile1024);
        DeleteObject(hbrTile2048);
        DeleteObject(pen);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void ClearBoard()
{
    for (int i = 0; i < 16; ++i)
        tiles[i] = 0;
}

//true - no move possible, false - can move
bool CheckBoard()
{
    if (status == TRUE)
        return true;
    for (int i = 0; i < 16; ++i)
    {
        if (tiles[i] == 0)
            return false; // empty boxes
        //check above
        if (i - 4 >= 0 && tiles[i - 4] == tiles[i]) return false;
        //check left
        else if (i - 1 % 4 != 3 && i - 1 >= 0 && tiles[i - 1] == tiles[i]) return false;
        //check below
        //else if (i + 4 < 16 && tiles[i + 4] == 0 || tiles[i + 4] == tiles[i]) return false;
        //check right
        //else if (i + 1 % 4 != 0 && i + 1 >= 0 && tiles[i + 1] == 0 || tiles[i + 1] == tiles[i]) return false;
    }
    return true;
}

//void Move()
//{
//    if (status == TRUE)
//        return;
//}

void Spawn2()
{
    if (CheckBoard()) //placeholder
        return;
    int i, choice = rand() % 16;
    for (i = 0; tiles[choice] != 0 && i < 1000; ++i)
        choice = rand() % 16;
    if (i == 1000)
        return;
    tiles[choice] = 2;

    PaintBox(g_hWnd_Boxes[choice], hbrTile2, L"2");
    PaintBox(g_hWnd_Boxes_Clone[choice], hbrTile2, L"2");
    if (CheckBoard()) //placeholder
        return;
}

void PaintBox(HWND hWnd, HBRUSH hbr, const WCHAR s[])
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    MapWindowPoints(hWnd, GetParent(hWnd), (LPPOINT)&rc, 2);

    HDC hdc = GetDC(GetParent(hWnd));
    HFONT font = CreateFont(
        -MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72), // Height
        0, // Width
        0, // Escapement
        0, // Orientation
        FW_BOLD, // Weight
        false, // Italic
        FALSE, // Underline
        0, // StrikeOut
        EASTEUROPE_CHARSET, // CharSet
        OUT_DEFAULT_PRECIS, // OutPrecision
        CLIP_DEFAULT_PRECIS, // ClipPrecision
        DEFAULT_QUALITY, // Quality
        DEFAULT_PITCH | FF_SWISS, // PitchAndFamily
        L" Verdana "); // Facename
    HFONT oldFont = (HFONT)SelectObject(hdc, font);

    HBRUSH hbrOld = (HBRUSH)SelectObject(hdc, hbrTile2);
    FillRect(hdc, &rc, hbrBackground);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 15, 15);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);
    DrawTextW(hdc, s, (int)wcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, hbrOld);
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldFont);
    DeleteObject(font);

    ReleaseDC(g_hWnd, hdc);
}