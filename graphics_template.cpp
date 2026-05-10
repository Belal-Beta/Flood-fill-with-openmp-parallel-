#include <windows.h>
#include <vector>
#include <cmath>
#include <stack>
#include <algorithm> // for swap
#include <omp.h> // for OpenMP
using namespace std;

void NRFloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf)
{
stack<POINT> S;
    S.push({x, y});

    // Start a parallel region
    #pragma omp parallel
    {
        // Only one thread starts the initial loop
        #pragma omp single
        {
            while (!S.empty()) {
                POINT v;
                bool hasPoint = false;

                // Protect stack access
                #pragma omp critical(stack_access)
                {
                    if (!S.empty()) {
                        v = S.top();
                        S.pop();
                        hasPoint = true;
                    }
                }

                if (hasPoint) {
                    // Each thread processes its own point as a task
                    #pragma omp task firstprivate(v) shared(S, hdc)
                    {
                        COLORREF c = GetPixel(hdc, v.x, v.y);
                        if (c != Cb && c != Cf) {
                            SetPixel(hdc, v.x, v.y, Cf);
                            #pragma omp critical(stack_access)
                            {
                                S.push({v.x + 1, v.y});
                                S.push({v.x - 1, v.y});
                                S.push({v.x, v.y + 1});
                                S.push({v.x, v.y - 1});
                            }
                        }
                    }
                }
            }
        }
    }
}
void DrawLine(HDC hdc, POINT st, POINT end, COLORREF color)
{
    int x1 = st.x, y1 = st.y;
    int x2 = end.x, y2 = end.y;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1; 
    int sy = (y1 < y2) ? 1 : -1; 
    int err = dx - dy;           
    int e2;                      
    while (true)
    {
        SetPixel(hdc, x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 > -dy) 
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)   
        {
            err += dx;
            y1 += sy;
        }
    }
}
LRESULT WINAPI wndProc(HWND hwnd, UINT mcode, WPARAM wp, LPARAM lp)
{       
    HDC hdc;
    static vector<POINT> points;
    int x, y;
    switch (mcode)
    {
    case WM_LBUTTONDOWN:
        x = LOWORD(lp);
        y = HIWORD(lp);
        points.push_back({ x, y });
        if (points.size() == 6)
        {
            hdc = GetDC(hwnd);
            for (int i = 0; i <=4; i++)
			{
				DrawLine(hdc, points[i], points[(i + 1) % 5], RGB(255, 0, 0));
			}
			NRFloodFill(hdc, points[5].x, points[5].y, RGB(255, 0, 0), RGB(255, 255, 255));
            ReleaseDC(hwnd, hdc);
        }
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, mcode, wp, lp);
    }
}

int APIENTRY WinMain(HINSTANCE h, HINSTANCE p, LPSTR c, int nsh)
{
    WNDCLASS wc = {0};
    wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hInstance = h;
    wc.lpfnWndProc = wndProc;
    wc.lpszClassName = "myclass"; // Removed L for ANSI compatibility
    wc.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("myclass", "Hello world", 
        WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, h, 0);

    ShowWindow(hwnd, nsh);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
