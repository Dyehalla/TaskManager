#include <vector>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include "ProcessInfo.h"
using namespace std;

// менялка кодировки
// wstring ansiToUtf16(const string& ansiString) {
//     int len = MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, nullptr, 0);
//     if (len == 0) {
//         return L"";
//     }
//     wstring utf16String(len - 1, L'\0');
//     MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, &utf16String[0], len);
//     return utf16String;
// }


vector<ProcessInfo> get_process_list() {
    vector<ProcessInfo> processes;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            ProcessInfo info;
            info.name = pe32.szExeFile;
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
            wchar_t filePath[MAX_PATH];
            if (GetModuleFileNameExW(hProcess, NULL, filePath, MAX_PATH)) {

                bool unique = true;
                PROCESS_MEMORY_COUNTERS_EX2 pmc;
                for (ProcessInfo &i : processes){
                    if (i.path == filePath){
                        if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
                            i.memoryUsage += pmc.PrivateWorkingSetSize / 1024 / 1024;
                        }
                        unique = false;
                        break;
                    }
                }

                if (unique){
                    info.path = filePath;
                    if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
                        info.memoryUsage = pmc.PrivateWorkingSetSize / 1024 / 1024;
                    }
                    processes.push_back(info);
                }

                CloseHandle(hProcess);
            }
        } while (Process32Next(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
    return processes;
}


// Функция для завершения процесса по ID (не знаю зачем инлайн, слион подсказал я согласился)
inline BOOL TerminateProcessById(DWORD processId) {
    // Открываем процесс с правом на завершение:
    // PROCESS_TERMINATE - процесс с правом на завершение,
    // FALSE - дескриптор не будет наследоваться,
    // procesId - индентификатор процесса.
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess == NULL) {
        return FALSE;
    }

    // Завершаем процесс:
    // hProcess - дескриптор процесса,
    // uExitCode - это код выхода, который передаётся процессу при его завершении, число обозначает статус с которыом процесс завершился.
    // Здесь передаётся 0, что обычно указывает на нормальное завершение.
    BOOL result = ::TerminateProcess(hProcess, 0);
    // Закрываем хэндл процесса
    CloseHandle(hProcess);
    return result;
}
