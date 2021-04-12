// Speedometr.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Speedometr.h"
#include "Core.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

long speed_100[4];
long cur;
long prev1;
long delta;
int period_interval_for_speed = 10; //60 30 10
long speed_sliping_0[60];
long tmp;
int cnt;
int del;
int cnt_del;
char upd_info;
int F_s;
HWND hWnd_Main;

Core Lir;

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter()
{
    LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li))
        MessageBoxA(hWnd_Main, "QueryPerformanceFrequency failed!\n", "Error", NULL);

    PCFreq = double(li.QuadPart) / 1000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}
double GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart - CounterStart) / PCFreq;
}

void TIMER_WIN32_10MS(HWND hWnd, UINT b, UINT_PTR c, DWORD d) {
    
}


// Отправить объявления функций, включенных в этот модуль кода:
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

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SPEEDOMETR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPEEDOMETR));

    MSG msg;

    // Цикл основного сообщения:
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPEEDOMETR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SPEEDOMETR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

  // HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   hWnd_Main = CreateDialog(hInst, MAKEINTRESOURCE(IDD_SPEEDOMETR_DIALOG), NULL, (DLGPROC)WndProc);

   if (!hWnd_Main)
   {
      return FALSE;
   }

   ShowWindow(hWnd_Main, nCmdShow);
   UpdateWindow(hWnd_Main);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        SendMessage(GetDlgItem(hWnd, IDC_SLIDER_FREQ), TBM_SETRANGE,(WPARAM)TRUE,(LPARAM)MAKELONG(0, 100));
        SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELITEL), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 20));
        SendMessage(GetDlgItem(hWnd, IDC_SLIDER_INTERVAL), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 2));
        Lir.print_debug_one_local_with_dot(GetDlgItem(hWnd, IDC_EDIT_COEFF),Lir.new_coeff_[0].binary,Lir.new_coeff_[0].n_after_dot, 0, 0, 0);
        for (int F = 0; F < 8; F++) {
            char tmp[40];
            Lir.convert_channel_counter_to_text_string(0, 0, (unsigned char*)tmp, F);
            SendMessageA(GetDlgItem(hWnd, IDC_COMBO_FORMAT), CB_ADDSTRING, 0, (LPARAM)&tmp);
        }
        SendMessageA(GetDlgItem(hWnd, IDC_COMBO_FORMAT), CB_SETCURSEL, CPrg::FORMAT::LINEAR_3, 0);

        SetTimer(hWnd, 10, 10, NULL);
        
        break;
    case WM_TIMER:
    {
        //{
            //Sleep(1000);
            double tmr = GetCounter() /1000;
           // char buf[40] = { 0 };
           // std::sprintf(buf, "%.2f", tmr);
           // SendMessageA(GetDlgItem(hWnd, IDC_STATIC_TIMER), WM_SETTEXT, NULL, LPARAM(buf));
            StartCounter();
        //}
       
        if (cnt_del == 0) {
            cur += cnt;
            cnt_del = del;
        }
        else 
            --cnt_del;


        delta = cur - prev1; //если дельта меньше 0 то едем влево , если больше нуля то вправо
        prev1 = cur;

        for (int n = period_interval_for_speed - 1; n > 0; n--) {
            speed_sliping_0[n] = speed_sliping_0[n - 1];
        }
        speed_sliping_0[0] = delta;

        speed_100[0] = 0;
        for (unsigned char n = 0; n < period_interval_for_speed; n++) {
            speed_100[0] = speed_sliping_0[n] + speed_100[0];
        }

        tmp = speed_100[0];
        tmp = (tmp * 10) / 4;

        if (period_interval_for_speed == 60)
            tmp = tmp / 6;
        else if (period_interval_for_speed == 30)
            tmp = tmp / 3;
        char buf[1000];
        if (!upd_info) {

            //tmr = 0.01;

            std::sprintf(buf, "Частота: ((%d * 10)/4)/%d = %d Гц", speed_100[0], period_interval_for_speed / 10,tmp);
            SendMessageA(GetDlgItem(hWnd_Main, IDC_STATIC_SHOW_FREQ), WM_SETTEXT, NULL, (WPARAM)buf);
            std::sprintf(buf, "Вход %d/%.2f c = %.2f Гц", cnt, tmr ,(double)cnt/ tmr);
            SendMessageA(GetDlgItem(hWnd, IDC_STATIC_FREQ), WM_SETTEXT, NULL, (WPARAM)buf);
            std::sprintf(buf, "Делитель: %.2f/%d = %.2f Гц", (double)cnt / tmr, del + 1, ((double)cnt / tmr)/ (del + 1));
            SendMessageA(GetDlgItem(hWnd, IDC_STATIC_DELITEL), WM_SETTEXT, NULL, (WPARAM)buf);
            std::sprintf(buf, "Счетчик: %d", cur);
            SendMessageA(GetDlgItem(hWnd, IDC_STATIC_CNT), WM_SETTEXT, NULL, (WPARAM)buf);
            std::sprintf(buf, "Интервал измерений: 0.%d сек", period_interval_for_speed/10);
            SendMessageA(GetDlgItem(hWnd, IDC_STATIC_INTERVAL), WM_SETTEXT, NULL, (WPARAM)buf);

            {
                char buf_s[40];
                long tmp = cur;
                Lir.mult(0, &tmp);
                Lir.convert_channel_counter_to_text_string(0, tmp, (unsigned char*)buf_s, F_s);
                std::sprintf(buf, "Индикация: %s", buf_s);
                SendMessageA(GetDlgItem(hWnd, IDC_STATIC_INDICATION), WM_SETTEXT, NULL, (WPARAM)buf);

                long speed_0 = speed_100[0];
                if (period_interval_for_speed == 30)
                    speed_0 = speed_0 * 2;
                else if (period_interval_for_speed == 10)
                    speed_0 = speed_0 * 6;//speed_0 * 6;

                std::sprintf(buf, "%s: %s %s", Lir.is_linear(F_s) ? "F" : "S", Lir.conv_spd(speed_0,F_s).c_str(), Lir.is_linear(F_s) ? "мм/мин" : "об/мин");
                SendMessageA(GetDlgItem(hWnd_Main, IDC_STATIC_SPEED), WM_SETTEXT, NULL, (WPARAM)buf);
            }

            std::sprintf(buf, "Буфер: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", speed_sliping_0[0],
                speed_sliping_0[1], speed_sliping_0[2], speed_sliping_0[3], speed_sliping_0[4], speed_sliping_0[5],
                speed_sliping_0[6], speed_sliping_0[7], speed_sliping_0[8], speed_sliping_0[9], speed_sliping_0[10], 
                speed_sliping_0[11], speed_sliping_0[12], speed_sliping_0[13], speed_sliping_0[14], speed_sliping_0[15],
                speed_sliping_0[16], speed_sliping_0[17], speed_sliping_0[18], speed_sliping_0[19], speed_sliping_0[20],
                speed_sliping_0[21], speed_sliping_0[22], speed_sliping_0[23], speed_sliping_0[24], speed_sliping_0[25],
                speed_sliping_0[26], speed_sliping_0[27], speed_sliping_0[28], speed_sliping_0[29], speed_sliping_0[30]);
            SendMessageA(GetDlgItem(hWnd, IDC_STATIC_BUFER_SK), WM_SETTEXT, NULL, (WPARAM)buf);
            std::sprintf(buf, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d = %d", speed_sliping_0[31],
                speed_sliping_0[32], speed_sliping_0[33], speed_sliping_0[34], speed_sliping_0[35], speed_sliping_0[36],
                speed_sliping_0[37], speed_sliping_0[38], speed_sliping_0[39], speed_sliping_0[40], speed_sliping_0[41],
                speed_sliping_0[42], speed_sliping_0[43], speed_sliping_0[44], speed_sliping_0[45], speed_sliping_0[46],
                speed_sliping_0[47], speed_sliping_0[48], speed_sliping_0[49], speed_sliping_0[50], speed_sliping_0[51],
                speed_sliping_0[52], speed_sliping_0[53], speed_sliping_0[54], speed_sliping_0[55], speed_sliping_0[56],
                speed_sliping_0[57], speed_sliping_0[58], speed_sliping_0[59], speed_100[0]);
            SendMessageA(GetDlgItem(hWnd, IDC_STATIC_BUFER_SZ2), WM_SETTEXT, NULL, (WPARAM)buf);

            upd_info = 6;
        }
        else
            upd_info--;
    }
        break;
    case WM_HSCROLL:
    {
        if ((HWND)lParam == GetDlgItem(hWnd, IDC_SLIDER_FREQ)) {
            cnt = SendDlgItemMessage(hWnd, IDC_SLIDER_FREQ, TBM_GETPOS, 0, 0);
        }
        else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SLIDER_DELITEL)) {
            del = SendDlgItemMessage(hWnd, IDC_SLIDER_DELITEL, TBM_GETPOS, 0, 0);
        }
        else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SLIDER_INTERVAL)) {
            int tmp = SendDlgItemMessage(hWnd, IDC_SLIDER_INTERVAL, TBM_GETPOS, 0, 0);
            
            if (tmp == 0) {
                period_interval_for_speed = 10;
                memset(&speed_sliping_0[9], 0, 50 * 4);
            }
            else if (tmp == 1) {
                period_interval_for_speed = 30;
                memset(&speed_sliping_0[29], 0, 31 * 4);
            }
            else if (tmp == 2)
                period_interval_for_speed = 60;
        }
        
    }
       break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDC_COMBO_FORMAT:
                if (HIWORD(wParam) == CBN_SELCHANGE) {
                    F_s = SendMessage(GetDlgItem(hWnd, IDC_COMBO_FORMAT), CB_GETCURSEL, NULL, NULL);
                }                   
                   // SetDlgItemTextA(hWnd, IDC_EDIT_COEFF, "hello");
                break;
            case IDC_BUTTON_TO_ZERO:
                cur = 0;
                break;
            case IDC_BUTTON_OK_COEFF:
                {
                char tmp[40];
                GetDlgItemTextA(hWnd, IDC_EDIT_COEFF, tmp, 40);
                Lir.translate_(tmp,40);
                if(Lir.data != 0)                    
                    Lir.set_axis_coeff(0, Lir.data, Lir.n_after_dot);
                Lir.print_debug_one_local_with_dot(GetDlgItem(hWnd, IDC_EDIT_COEFF), Lir.new_coeff_[0].binary, Lir.new_coeff_[0].n_after_dot, 0, 0, 0);
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
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
