// TorInjector.cpp: определяет точку входа для приложения.
//

#include "header.hpp"

using namespace std;

int getDLLPath(char* dll)
{
    cout << "Enter the path to your DLL:" << endl;
    cin.getline(dll, 255);
    return 0;
}

DWORD GetProcessIdByName(const wchar_t* processName) {
    PROCESSENTRY32 processEntry; // Структура для хранения информации о процессе
    processEntry.dwSize = sizeof(PROCESSENTRY32); // Устанавливаем размер структуры
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Создаем снимок всех процессов в системе

    if (snapshot == INVALID_HANDLE_VALUE) { // Проверяем, удалось ли создать снимок
        cout << "ERROR: Failed to screen processes." << std::endl; // Сообщаем об ошибке, если снимок не создан
        return 0;
    }

    // Итерируемся по процессам в снимке
    if (Process32First(snapshot, &processEntry)) {
        do {
            // Сравниваем имя текущего процесса с нужным именем
            if (!_wcsicmp(LPCWSTR(processEntry.szExeFile), processName)) {
                DWORD processId = processEntry.th32ProcessID; // Получаем ID процесса
                CloseHandle(snapshot); // Закрываем снимок после нахождения процесса
                return processId; // Возвращаем ID найденного процесса
            }
        } while (Process32Next(snapshot, &processEntry)); // Переходим к следующему процессу в списке
    }

    CloseHandle(snapshot);
    return 0;
}

HANDLE getProcessHandle(DWORD PID) {
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

    if (processHandle == NULL) {
        cout << "ERROR: Unable to open process. Error code: " << GetLastError() << endl;
        return NULL;
    }
    else {
        cout << "INFO: Process Handler got succesfully! HANDLE:" << processHandle << endl;
        return processHandle;
    }
    
}

int InjectDLL(DWORD PID, char* dll) 
{
    HANDLE handleToProc;
    LPVOID LoadLibAddr;
    LPVOID BaseAddr;
    HANDLE remoteThread;

    unsigned dllLength = strlen(dll) + 1;
    
    handleToProc = getProcessHandle(PID);
    if (!handleToProc) {
        cout << "ERROR: Failed to get handle of target process. Code: -1" << endl;
        return -1;
    }

    LoadLibAddr = (LPVOID)(GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"));

    if (!LoadLibAddr) {
        cout << "ERROR: Unable to load Lib Address. Code: -1" << endl;
        return -1;
    }

    BaseAddr = VirtualAllocEx(handleToProc, NULL, dllLength, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (!BaseAddr) {
        cout << "ERROR: Unable to get Base Address. Code: -1" << endl;
        return -1;
    }

    if (!WriteProcessMemory(handleToProc, BaseAddr, dll, dllLength, NULL)) {
        cout << "Unable to write dll in process" << endl;
        return -1;
    }

    remoteThread = CreateRemoteThread(handleToProc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, BaseAddr, 0, NULL);


    if (!remoteThread) {
        cout << "ERROR: Unable to create thread. Code: -1" << endl;
        return -1;
    }

    WaitForSingleObject(remoteThread,  INFINITE);

    VirtualFreeEx(handleToProc, BaseAddr, dllLength, MEM_RELEASE);

    if (CloseHandle(remoteThread) == 0) {
        cout << "ERROR: Failed to close remote thread handle. Code: -1" << endl;
        return -1;
    }

    if (CloseHandle(handleToProc) == 0) {
        cout << "ERROR: Failed to close process handle. Code: -1" << endl;
        return -1;
    }
    return 0;
}

int main()
{
    SetConsoleTitleA("Tor Injector v1.0. Non-Portable Beta");
    cout << "Tor Injector v1.0. Non-Portable Beta" << endl;

    char processName[127] = {};
    cout << "Please enter the process name:" << endl;
    cin.getline(processName, 127);

    char* dll = new char [255];
    getDLLPath(dll);

    cout << dll << endl;

    DWORD PID;
	//DWORD PID = GetProcessIdByName(L"csgo.exe");
    cin >> PID;
    cout << PID << endl;
    cout << sizeof(PID) << endl;
    InjectDLL(PID, dll);
    system("pause");

    return 0;
}
