#include "ProcessInfo.h"
using namespace std;

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

int bubbleSort(std::vector<ProcessInfo> &vector)
{
    int size = vector.size();
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (vector[j].memoryUsage < vector[j + 1].memoryUsage)
                std::swap(vector[j], vector[j + 1]);
        }
    }
    return size;
}

// Функция для завершения процесса по ID (не знаю зачем инлайн, слион подсказал я согласился)
inline BOOL TerminateProcessById(DWORD processId) {
    // Открываем процесс с правом на завершение:
    // PROCESS_TERMINATE - процесс с правом на завершение,
    // FALSE - дескриптор не будет наследоваться,
    // procesId - индентификатор процесса
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess == NULL) {
        return FALSE;
    }

    BOOL result = ::TerminateProcess(hProcess, 0);
    // Закрываем хэндл процесса
    CloseHandle(hProcess);
    return result;
}
