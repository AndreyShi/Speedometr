// header.h: включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows
#pragma warning( disable : 4091 )
#pragma warning( disable : 4996 )
#pragma comment(lib,"Comctl32.lib")
#include <windows.h>
#include <commctrl.h>
#include <iostream>
#include <string>
// Файлы заголовков среды выполнения C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
