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
int status = 0;
static int tiles[16];
static int score;
static int goal;

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
bool                TileArrAlign(int*);
bool                MoveLeft();
bool                MoveRight();
bool                MoveUp();
bool                MoveDown();
void                Spawn2();
void                PaintBox(HWND hWnd, HBRUSH hbr, const WCHAR s[]);
void                RepaintBoard();
void                CheckMenuGoal(UINT uIDCheckItem);
void                PaintStatus(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    srand((unsigned int)time(0));
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

    score = 0;
    CheckMenuGoal(ID_GOAL_2048);
    ClearBoard();

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
                    score = 0;
                    status = 0;
                    HDC hdc = GetDC(g_hWnd);
                    HDC hdc_c = GetDC(g_hWnd_Clone);
                    SendMessageW(g_hWnd, WM_ERASEBKGND, (WPARAM)hdc, NULL);
                    SendMessageW(g_hWnd_Clone, WM_ERASEBKGND, (WPARAM)hdc_c, NULL);
                    ClearBoard();
                    Spawn2();
                }
                break;
            case ID_GOAL_8:
                CheckMenuGoal(ID_GOAL_8);
                goal = 8;
            break;
            case ID_GOAL_16:
                CheckMenuGoal(ID_GOAL_16);
                goal = 16;
            break;
            case ID_GOAL_64:
                CheckMenuGoal(ID_GOAL_64);
                goal = 64;
            break;
            case ID_GOAL_2048:
                CheckMenuGoal(ID_GOAL_2048);
                goal = 2048;
            break;
            default:
                return DefWindowProcW(hWnd, message, wParam, lParam);
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
            WCHAR scoreSTR[64] = L"";
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (hWnd == g_hWnd)
            {
                for (int i = 0; i < 16; ++i)
                {
                    switch (tiles[i])
                    {
                    case 2: PaintBox(g_hWnd_Boxes[i], hbrTile2, L"2"); break;
                    case 4: PaintBox(g_hWnd_Boxes[i], hbrTile4, L"4"); break;
                    case 8: PaintBox(g_hWnd_Boxes[i], hbrTile8, L"8"); break;
                    case 16: PaintBox(g_hWnd_Boxes[i], hbrTile16, L"16"); break;
                    case 32: PaintBox(g_hWnd_Boxes[i], hbrTile32, L"32"); break;
                    case 64: PaintBox(g_hWnd_Boxes[i], hbrTile64, L"64"); break;
                    case 128: PaintBox(g_hWnd_Boxes[i], hbrTile128, L"128"); break;
                    case 256: PaintBox(g_hWnd_Boxes[i], hbrTile256, L"256"); break;
                    case 512: PaintBox(g_hWnd_Boxes[i], hbrTile512, L"512"); break;
                    case 1024: PaintBox(g_hWnd_Boxes[i], hbrTile1024, L"1024"); break;
                    case 2048: PaintBox(g_hWnd_Boxes[i], hbrTile2048, L"2048"); break;
                    default: PaintBox(g_hWnd_Boxes[i], hbrBox, L""); break;
                    }
                }
                wsprintfW(scoreSTR, L"%d", score);
                PaintBox(g_hWnd_ScoreBar, hbrBox, scoreSTR);
            }
            else if (hWnd == g_hWnd_Clone)
            {
                for (int i = 0; i < 16; ++i)
                {
                    switch (tiles[i])
                    {
                    case 2: PaintBox(g_hWnd_Boxes_Clone[i], hbrTile2, L"2"); break;
                    case 4: PaintBox(g_hWnd_Boxes_Clone[i], hbrTile4, L"4"); break;
                    case 8: PaintBox(g_hWnd_Boxes_Clone[i], hbrTile8, L"8"); break;
                    case 16: PaintBox(g_hWnd_Boxes_Clone[i], hbrTile16, L"16"); break;
                    case 32: PaintBox(g_hWnd_Boxes_Clone[i], hbrTile32, L"32"); break;
                    case 64: PaintBox(g_hWnd_Boxes_Clone[i], hbrTile64, L"64"); break;
                    case 128: PaintBox(g_hWnd_Boxes_Clone[i], hbrTile128, L"128"); break;
                    case 256: PaintBox(g_hWnd_Boxes_Clone[i], hbrTile256, L"256"); break;
                    case 512: PaintBox(g_hWnd_Boxes_Clone[i], hbrTile512, L"512"); break;
                    case 1024: PaintBox(g_hWnd_Boxes_Clone[i], hbrTile1024, L"1024"); break;
                    case 2048: PaintBox(g_hWnd_Boxes_Clone[i], hbrTile2048, L"2048"); break;
                    default: PaintBox(g_hWnd_Boxes_Clone[i], hbrBox, L""); break;
                    }
                }
                wsprintfW(scoreSTR, L"%d", score);
                PaintBox(g_hWnd_ScoreBar_Clone, hbrBox, scoreSTR);
            }
            if (status != 0)
            {
                PaintStatus(hWnd);
            }
            EndPaint(hWnd, &ps);  
        }
        break;
    case WM_KEYDOWN:
        {
            bool moved = false;
            switch (wParam)
            {
            case 0x57: // W
                moved = MoveUp();
                break;
            case 0x41: // A
                moved = MoveLeft();
                break;
            case 0x53: // S
                moved = MoveDown();
                break;
            case 0x44: // D
                moved = MoveRight();
                break;
            }
            if (moved)
            {
                Spawn2();
                RepaintBoard();
            }
            return DefWindowProcW(hWnd, message, wParam, lParam);
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
                MoveWindow(g_hWnd_Clone, 2 * centerX - xPos - rcSize.right - rcSize.left, 2 * centerY - yPos - rcSize.bottom - rcSize.top, rcSize.right - rcSize.left,  rcSize.bottom - rcSize.top, FALSE);
            else if (hWnd == g_hWnd_Clone)
                MoveWindow(g_hWnd, 2 * centerX - xPos - rcSize.right - rcSize.left, 2 * centerY - yPos - rcSize.bottom - rcSize.top, rcSize.right - rcSize.left,    rcSize.bottom - rcSize.top, FALSE);
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
        {
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
        }
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
    RepaintBoard();
}

//true - no move possible, false - can move
bool CheckBoard()
{
    if (status != 0)
        return true;
    for (int i = 0; i < 16; ++i)
        if (tiles[i] == goal)
        {
            status = 1;
            return true;
        }
    for (int i = 0; i < 16; ++i)
    {
        if (tiles[i] == 0)
            return false; // empty boxes
        //check above
        if (i - 4 >= 0 && tiles[i - 4] == tiles[i]) return false;
        //check to the left
        else if ((i - 1) % 4 != 3 && i - 1 >= 0 && tiles[i - 1] == tiles[i]) return false;
    }
    RepaintBoard();
    status = 2;
    return true;
}

bool TileArrAlign(int* arr)
{
    bool aligned = false;
    int j;
    for (int i = 0; i < 4; ++i)
    {
        if (arr[i] != 0)
            continue;
        else
            for (j = i + 1; j < 4; ++j)
                if (arr[j] != 0)
                {
                    arr[i] = arr[j];
                    arr[j] = 0;
                    aligned = true;
                    break;
                }
        if (j == 4)
            break;
    }
    return aligned;
}

bool TileArrMerge(int* arr)
{
    bool merged = false;
    for (int i = 0; i < 4; ++i)
    {
        if (arr[i] == 0)
            return merged;
        if (arr[i + 1] != 0 && arr[i] == arr[i + 1])
        {
            arr[i] *= 2;
            score += arr[i];
            arr[i++ + 1] = 0;
            merged = true;
        }
    }
    return merged;
}

bool MoveLeft()
{
    if (status != 0)
        return false;

    bool moved = false;
    int row[4];
    for (int i = 0; i < 4; ++i)
    {
        row[3] = tiles[4 * i + 3];
        row[2] = tiles[4 * i + 2];
        row[1] = tiles[4 * i + 1];
        row[0] = tiles[4 * i];
        if (!moved)
        {
            if (moved = TileArrAlign(row))
                if (TileArrMerge(row))
                    TileArrAlign(row);
                else;
            else if (moved = TileArrMerge(row))
                TileArrAlign(row);
        }
        else
        {
            TileArrAlign(row);
            if (TileArrMerge(row))
                TileArrAlign(row);
        }
        tiles[4 * i + 3] = row[3];
        tiles[4 * i + 2] = row[2];
        tiles[4 * i + 1] = row[1];
        tiles[4 * i] = row[0];
    }
    return moved;
}

bool MoveRight()
{
    if (status != 0)
        return false;

    bool moved = false;
    int row[4];
    for (int i = 0; i < 4; ++i)
    {
        row[0] = tiles[4 * i + 3];
        row[1] = tiles[4 * i + 2];
        row[2] = tiles[4 * i + 1];
        row[3] = tiles[4 * i];
        if (!moved)
        {
            if (moved = TileArrAlign(row))
                if (TileArrMerge(row))
                    TileArrAlign(row);
                else;
            else if (moved = TileArrMerge(row))
                TileArrAlign(row);
        }
        else
        {
            TileArrAlign(row);
            if (TileArrMerge(row))
                TileArrAlign(row);
        }
        tiles[4 * i + 3] = row[0];
        tiles[4 * i + 2] = row[1];
        tiles[4 * i + 1] = row[2];
        tiles[4 * i] = row[3];
    }
    return moved;
}

bool MoveUp()
{
    if (status != 0)
        return false;

    bool moved = false;
    int col[4];
    for (int i = 0; i < 4; ++i)
    {
        col[0] = tiles[i];
        col[1] = tiles[i + 4];
        col[2] = tiles[i + 8];
        col[3] = tiles[i + 12];
        if (!moved)
        {
            if (moved = TileArrAlign(col))
                if (TileArrMerge(col))
                    TileArrAlign(col);
                else;
            else if (moved = TileArrMerge(col))
                TileArrAlign(col);
        }
        else
        {
            TileArrAlign(col);
            if (TileArrMerge(col))
                TileArrAlign(col);
        }
        tiles[i] = col[0];
        tiles[i + 4] = col[1];
        tiles[i + 8] = col[2];
        tiles[i + 12] = col[3];
    }
    return moved;
}

bool MoveDown()
{
    if (status != 0)
        return false;

    bool moved = false;
    int col[4];
    for (int i = 0; i < 4; ++i)
    {
        col[3] = tiles[i];
        col[2] = tiles[i + 4];
        col[1] = tiles[i + 8];
        col[0] = tiles[i + 12];
        if (!moved)
        {
            if (TileArrAlign(col))
                if (TileArrMerge(col))
                    TileArrAlign(col);
                else;
            else if (moved = TileArrMerge(col))
                TileArrAlign(col);
        }
        else
        {
            TileArrAlign(col);
            if (TileArrMerge(col))
                TileArrAlign(col);
        }
        tiles[i] = col[3];
        tiles[i + 4] = col[2];
        tiles[i + 8] = col[1];
        tiles[i + 12] = col[0];
    }
    return moved;
}

void Spawn2()
{
    if (CheckBoard())
        return;
    int i, choice = rand() % 16;
    for (i = 0; tiles[choice] != 0 && i < 1000; ++i)
        choice = rand() % 16;
    if (i == 1000)
        return;
    tiles[choice] = 2;

    PaintBox(g_hWnd_Boxes[choice], hbrTile2, L"2");
    PaintBox(g_hWnd_Boxes_Clone[choice], hbrTile2, L"2");
    if (CheckBoard())
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

    HBRUSH hbrOld = (HBRUSH)SelectObject(hdc, hbr);
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

    ReleaseDC(GetParent(hWnd), hdc);
}

void PaintStatus(HWND hWnd)
{
    static const WCHAR gameover[] = L"GAME OVER!";
    static const WCHAR win[] = L"WIN!";
    RECT rc, rc_clone;
    BLENDFUNCTION bf;
    bf.AlphaFormat = 0;
    bf.BlendFlags = 0;
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 127;
    GetClientRect(hWnd, &rc);
    HDC hdc = GetDC(hWnd);
    HFONT font = CreateFont(
        -MulDiv(32, GetDeviceCaps(hdc, LOGPIXELSY), 72), // Height
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
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);
    if (status == 2)
    {
        HDC hdcRed = CreateCompatibleDC(hdc);
        HBITMAP hbmRed = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcRed, hbmRed);
        HBRUSH hbrRed = CreateSolidBrush(RGB(255, 0, 0));
        HBRUSH hbrOld = (HBRUSH)SelectObject(hdcRed, hbrRed);
        FillRect(hdcRed, &rc, hbrRed);
        AlphaBlend(hdc, rc.left, rc.top, rc.right, rc.bottom, hdcRed, rc.left, rc.top, rc.right, rc.bottom, bf);
        DrawTextW(hdc, gameover, (int)wcslen(gameover), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdcRed, hbmOld);
        SelectObject(hdcRed, hbrOld);
        DeleteObject(hbmRed);
        DeleteObject(hbrRed);
        DeleteDC(hdcRed);
    }
    else if (status == 1)
    {
        HDC hdcGreen = CreateCompatibleDC(hdc);
        HBITMAP hbmGreen = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcGreen, hbmGreen);
        HBRUSH hbrGreen = CreateSolidBrush(RGB(0, 255, 0));
        HBRUSH hbrOld = (HBRUSH)SelectObject(hdcGreen, hbrGreen);
        FillRect(hdcGreen, &rc, hbrGreen);
        AlphaBlend(hdc, rc.left, rc.top, rc.right, rc.bottom, hdcGreen, rc.left, rc.top, rc.right, rc.bottom, bf);
        DrawTextW(hdc, win, (int)wcslen(win), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdcGreen, hbmOld);
        SelectObject(hdcGreen, hbrOld);
        DeleteObject(hbmGreen);
        DeleteObject(hbrGreen);
        DeleteDC(hdcGreen);
    }
    SelectObject(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hWnd, hdc);
}

void RepaintBoard()
{
    SendMessageW(g_hWnd, WM_PAINT, NULL, NULL);
    SendMessageW(g_hWnd_Clone, WM_PAINT, NULL, NULL);
}

void CheckMenuGoal(UINT uIDCheckItem)
{
    CheckMenuItem(GetMenu(g_hWnd), ID_GOAL_8, MFS_UNCHECKED);
    CheckMenuItem(GetMenu(g_hWnd_Clone), ID_GOAL_8, MFS_UNCHECKED);
    CheckMenuItem(GetMenu(g_hWnd), ID_GOAL_16, MFS_UNCHECKED);
    CheckMenuItem(GetMenu(g_hWnd_Clone), ID_GOAL_16, MFS_UNCHECKED);
    CheckMenuItem(GetMenu(g_hWnd), ID_GOAL_64, MFS_UNCHECKED);
    CheckMenuItem(GetMenu(g_hWnd_Clone), ID_GOAL_64, MFS_UNCHECKED);
    CheckMenuItem(GetMenu(g_hWnd), ID_GOAL_2048, MFS_UNCHECKED);
    CheckMenuItem(GetMenu(g_hWnd_Clone), ID_GOAL_2048, MFS_UNCHECKED);

    CheckMenuItem(GetMenu(g_hWnd), uIDCheckItem, MFS_CHECKED);
    CheckMenuItem(GetMenu(g_hWnd_Clone), uIDCheckItem, MFS_CHECKED);
}