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

void Draw8Points(HDC hdc, int x, int y, POINT shift)
{
    SetPixel(hdc, shift.x + x, shift.y + y, RGB(0, 255, 0));
    SetPixel(hdc, shift.x - x, shift.y + y, RGB(0, 255, 0));
    SetPixel(hdc, shift.x + x, shift.y - y, RGB(0, 0, 255));
    SetPixel(hdc, shift.x - x, shift.y - y, RGB(255, 255, 0));
    SetPixel(hdc, shift.x + y, shift.y + x, RGB(0, 255, 255));
    SetPixel(hdc, shift.x - y, shift.y + x, RGB(255, 0, 255));
    SetPixel(hdc, shift.x + y, shift.y - x, RGB(255, 165, 0));
    SetPixel(hdc, shift.x - y, shift.y - x, RGB(128, 0, 128));                    
}

void DrawCircle(HDC hdc, POINT c, POINT r)
{
    int R = (int)round(sqrt(pow(c.x - r.x, 2) + pow(c.y - r.y, 2)));
    int x = 0, y = R, d = 1 - R, ch1 = 3, ch2 = 5 - 2 * R;
    while (x <= y)
    {
        Draw8Points(hdc, x, y, c); // Simplified to pass x, y directly
        if (d <= 0)
        {
            d += ch1;
            ch1 += 2;
            ch2 += 2;
        }
        else
        {
            d += ch2;
            ch1 += 2;
            ch2 += 4;
            y--;
        }
        x++;
    }
}

void DrawTwoCiclesWithFilledAreaBetweenThem(HDC hdc, POINT c, POINT r1, POINT r2, COLORREF color)
{
    int R1 = (int)round(sqrt(pow(c.x - r1.x, 2) + pow(c.y - r1.y, 2)));
    int R2 = (int)round(sqrt(pow(c.x - r2.x, 2) + pow(c.y - r2.y, 2)));
    
    if (R1 > R2) swap(R1, R2);
    
    for (int r = R1; r <= R2; r++)
    {
        POINT rp = { c.x + r, c.y };
        DrawCircle(hdc, c, rp);
    }
}

void hermite(HDC hdc, vector<POINT> points, COLORREF color) 
{
    POINT p0 = points[0], p1 = points[2];
    POINT t0 = { points[1].x - points[0].x, points[1].y - points[0].y };
    POINT t1 = { points[3].x - points[2].x, points[3].y - points[2].y };
    for (double t = 0; t <= 1; t += 0.001)
    {
        double t2 = t * t, t3 = t2 * t;
        double h00 = 2 * t3 - 3 * t2 + 1;
        double h10 = t3 - 2 * t2 + t;
        double h01 = -2 * t3 + 3 * t2;
        double h11 = t3 - t2;
        int x = (int)(h00 * p0.x + h10 * t0.x + h01 * p1.x + h11 * t1.x);
        int y = (int)(h00 * p0.y + h10 * t0.y + h01 * p1.y + h11 * t1.y);
        SetPixel(hdc, x, y, color);
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
    case WM_RBUTTONDOWN:
        x = LOWORD(lp);
        y = HIWORD(lp);
        points.push_back({ x, y });
        if (points.size() == 3)
        {
            hdc = GetDC(hwnd);
            DrawTwoCiclesWithFilledAreaBetweenThem(hdc, points[0], points[1], points[2], RGB(255, 255, 255));
            DrawLine(hdc, points[0], points[1], RGB(0, 255, 0));
            DrawLine(hdc, points[0], points[2], RGB(0, 255, 0));
            points.clear();
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