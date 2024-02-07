// TicTacToe.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TicTacToe.h"
#include <windowsx.h>
#include <mmsystem.h>

#define MAX_LOADSTRING 100
#define COLOR1 RGB(77, 77, 255)
#define COLOR2 RGB(229, 43, 80)

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICTACTOE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

// Global Cell size

const int CELL_SIZE = 100;
int winner = 0;
int player = 1;
int board[9] = { 0,0,0,0,0,0,0,0,0 };
HBRUSH hbr1, hbr2;
HICON icon1, icon2;
int win[3];
int highscore1 = 0;
int highscore2 = 0;

bool drawBoard(HWND hwnd, RECT* rect) {
    SetRectEmpty(rect);
    if (GetClientRect(hwnd, rect)) {
        int width = rect->right - rect->left;
        int height = rect->bottom - rect->top;

        rect->left = (width - CELL_SIZE * 3) / 2;
        rect->top = (height - CELL_SIZE * 3) / 2;

        rect->right = rect->left + CELL_SIZE * 3;
        rect->bottom = rect->top + CELL_SIZE * 3;

        return true;
    }

    return false;
}

void drawLine(HDC hd, int x1, int y1, int x2, int y2) {
    MoveToEx(hd, x1, y1, nullptr);
    LineTo(hd, x2, y2);
}

int getIndex(HWND hwnd, int x, int y) {
    RECT board;
    POINT pt = { x,y };
    if (drawBoard(hwnd, &board)) {
        if (PtInRect(&board, pt)) {
            x = pt.x - board.left;
            y = pt.y - board.top;

            int column = x / CELL_SIZE;
            int row = y / CELL_SIZE;

            return column + row * 3;
        }
    }

    return -1;
}

bool fillCell(HWND hwnd, int index, RECT* cell) {
    SetRectEmpty(cell);
    if (index < 0 or index > 8)
        return false;

    RECT tempBoard;
    if (drawBoard(hwnd, &tempBoard)) {
        int column = index % 3;
        int row = index / 3;

        cell->left = tempBoard.left + column * CELL_SIZE + 1;
        cell->top = tempBoard.top + row * CELL_SIZE + 1;
        cell->right = cell->left + CELL_SIZE - 1;
        cell->bottom = cell->top + CELL_SIZE - 1;

        return true;
    }

    return false;
}

int getWinner() {
    int cells[24] = {
        0,1,2   ,
        3,4,5   ,
        6,7,8   ,

        0,3,6   ,
        1,4,7   ,
        2,5,8   ,

        0,4,8   ,
        2,4,6
    };

    for (int i = 0; i < 24; i += 3) {
        if (board[cells[i]] != 0 and board[cells[i]] == board[cells[i + 1]] and board[cells[i]] == board[cells[i + 2]]) {
            win[0] = cells[i];
            win[1] = cells[i + 1];
            win[2] = cells[i + 2];
            return board[cells[i]];
        }
    }

    for (int i = 0; i < 9; i++) {
        if (board[i] == 0)
            return 0;
    }
    return 3;
}

void newgame(HWND hwnd) {
    winner = 0;
    player = 1;
    ZeroMemory(board, sizeof(board));
    InvalidateRect(hwnd, nullptr, true);
    UpdateWindow(hwnd);
}

void displayFooter(HWND hwnd, HDC hdc) {

    const WCHAR text1[] = L"Turn : Player 1";
    const WCHAR text2[] = L"Turn : Player 2";

    const WCHAR* ptr = nullptr;

    switch (winner) {
    case 0:
        ptr = player == 1 ? text1 : text2;
        break;
    case 1:
        ptr = L"Player 1 has won";
        break;
    case 2:
        ptr = L"Player 2 has won";
        break;
    case 3:
        ptr = L"Nobody has won, it's a draw";
        break;
    }

    RECT footer;
    if (ptr != nullptr and GetClientRect(hwnd, &footer)) {
        footer.top = footer.bottom - 60;
        FillRect(hdc, &footer, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(102, 102, 0));
        DrawText(hdc, ptr, lstrlen(ptr), &footer, DT_CENTER);
    }
}

void drawCenteredIcon(HDC hdc, RECT* cell, HICON icon) {
    const int width = GetSystemMetrics(SM_CXICON);
    const int height = GetSystemMetrics(SM_CYICON);

    if (cell and icon) {
        int left = cell->left + ((cell->right - cell->left) - width) / 2;
        int top = cell->top + ((cell->bottom - cell->top) - height) / 2;

        DrawIcon(hdc, left, top, icon);
    }

}

void showWinner(HWND hWnd, HDC hd) {
    RECT winCell;
    for (int i = 0; i < 3; i++) {
        if (fillCell(hWnd, win[i], &winCell)) {
            FillRect(hd, &winCell, winner == 1 ? hbr1 : hbr2);
            drawCenteredIcon(hd, &winCell, player == 1 ? icon1 : icon2);
        }
    }
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        hbr1 = CreateSolidBrush(COLOR1);
        hbr2 = CreateSolidBrush(COLOR2);

        icon1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER1));
        icon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER2));
    }
    break;
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* info = (MINMAXINFO*)lParam;

        info->ptMinTrackSize.x = CELL_SIZE * 5;
        info->ptMinTrackSize.y = CELL_SIZE * 5;

    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case ID_FILE_NEWGAME:
        {
            if (winner == 0) {
                int ret = MessageBox(hWnd, L"Do you really wanna restart the game ?", L"Restart Game", MB_YESNO | MB_ICONWARNING);
                if (ret == IDYES) {
                    highscore1 = 0;
                    highscore2 = 0;
                    newgame(hWnd);
                }
                break;
            }
        }
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
    }
    break;
    case WM_LBUTTONDOWN:
    {
        //PlaySound(L"mixkit-classic-click-1117.wav", nullptr, SND_FILENAME | SND_ASYNC);
        if (player == 0)
            break;
        int xAxis = GET_X_LPARAM(lParam);
        int yAxis = GET_Y_LPARAM(lParam);

        int index = getIndex(hWnd, xAxis, yAxis);

        HDC hd = GetDC(hWnd);
        if (hd) {
            /*WCHAR text[100];
            wsprintf(text, L"Index : %d", index);
            TextOut(hd, xAxis, yAxis, text, lstrlen(text));*/
            if (index == -1) {
                WCHAR text[100];
                SetBkMode(hd, TRANSPARENT);
                wsprintf(text, L"Outside Board");
                TextOut(hd, xAxis, yAxis, text, lstrlen(text));
            }
            else {
                RECT cell;
                if (board[index] == 0 and fillCell(hWnd, index, &cell)) {
                    board[index] = player;
                    /*FillRect(hd, &cell, player == 1 ? hbr1 : hbr2);*/
                    drawCenteredIcon(hd, &cell, player == 1 ? icon1 : icon2);

                    winner = getWinner();

                    if (winner == 1 or winner == 2)
                    {
                        if (winner == 1)
                            highscore1++;
                        if (winner == 2)
                            highscore2++;
                        showWinner(hWnd, hd);
                        MessageBox(hWnd, winner == 1 ? L"Player 1 has won." : L"Player 2 has won.", L"You Won!", MB_OK | MB_ICONINFORMATION);
                        player = 0;
                        newgame(hWnd);
                    }
                    else if (winner == 3)
                    {
                        MessageBox(hWnd, L"No one has won.", L"It's a draw!", MB_OK | MB_ICONHAND);
                        player = 0;
                        newgame(hWnd);
                    }
                    else
                    {
                        player = player == 1 ? 2 : 1;
                    }
                    displayFooter(hWnd, hd);

                }
            }


            ReleaseDC(hWnd, hd);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...

        RECT gameboard;

        if (drawBoard(hWnd, &gameboard)) {
            FillRect(hdc, &gameboard, (HBRUSH)GetStockObject(WHITE_BRUSH));

            for (int i = 1; i < 3; i++) {
                drawLine(hdc, gameboard.left + CELL_SIZE * i, gameboard.top, gameboard.left + CELL_SIZE * i, gameboard.bottom);
                drawLine(hdc, gameboard.left, gameboard.top + CELL_SIZE * i, gameboard.right, gameboard.top + CELL_SIZE * i);
            }

            for (int i = 0; i < 9; i++) {
                RECT cell;
                if (board[i] != 0 and fillCell(hWnd, i, &cell)) {
                    //FillRect(hdc, &cell, board[i] == 1 ? hbr1 : hbr2);
                    drawCenteredIcon(hdc, &cell, board[i] == 1 ? icon1 : icon2);
                }
            }

            if (winner == 1 or winner == 2) {
                newgame(hWnd);
                showWinner(hWnd, hdc);
            }

            RECT playerTag;
            if (GetClientRect(hWnd, &playerTag)) {
                WCHAR score1[50];
                WCHAR score2[50];
                wsprintf(score1, L"Score : %d", highscore1);
                wsprintf(score2, L"Score : %d", highscore2);

                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, COLOR1);
                TextOut(hdc, playerTag.left + 16, 16, L"Player 1", 8);
                DrawIcon(hdc, playerTag.left + 25, 40, icon1);
                TextOut(hdc, playerTag.left + 16, 75, score1, lstrlen(score1));

                SetTextColor(hdc, COLOR2);
                TextOut(hdc, playerTag.right - 74, 16, L"Player 2", 8);
                DrawIcon(hdc, playerTag.right - 60, 40, icon2);
                TextOut(hdc, playerTag.right - 74, 75, score2, lstrlen(score2));

                displayFooter(hWnd, hdc);
            }

        }
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        DeleteObject(hbr1);
        DeleteObject(hbr2);
        DestroyIcon(icon1);
        DestroyIcon(icon2);
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
