// TorInjector.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.

#pragma once

#include <iostream>
#include <Windows.h>
#include <wchar.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <string>

int InjectDLL(DWORD, char*);
int getDLLPath(char*);
DWORD GetProcessIdByName(const wchar_t*);
HANDLE getProcessHandle(HANDLE*, DWORD); 


// TODO: установите здесь ссылки на дополнительные заголовки, требующиеся для программы.
