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
            info.pid = pe32.th32ProcessID;
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
            wchar_t filePath[MAX_PATH];
            size_t memory_usage;
            ULONGLONG total_time;
            FILETIME creationTime, exitTime, kernelTime, userTime;

            if (GetModuleFileNameExW(hProcess, NULL, filePath, MAX_PATH)) {
                info.path = filePath;
            } else continue;

            //Получаем память
            PROCESS_MEMORY_COUNTERS_EX2 pmc;
            if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
                memory_usage = pmc.PrivateWorkingSetSize / 1024; // В килобайтах
            } else memory_usage = 0;

            //Получаем время ЦП
            if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime)) {

                // Преобразуем FILETIME в 64-битное число
                ULARGE_INTEGER kernel_time_ul, user_time_ul;
                kernel_time_ul.LowPart = kernelTime.dwLowDateTime;
                kernel_time_ul.HighPart = kernelTime.dwHighDateTime;
                user_time_ul.LowPart = userTime.dwLowDateTime;
                user_time_ul.HighPart = userTime.dwHighDateTime;

                // Суммируем время в режиме ядра и пользователя
                total_time = kernel_time_ul.QuadPart + user_time_ul.QuadPart;

                // Преобразуем в секунды (1 сек = 10 000 000 * 100 нс)
                total_time = static_cast<double>(total_time) / 10000000.0;

            } else {
                qDebug() << "some shit happened in cpu time: " << info.path;
                total_time = 0;
            }


            bool unique = true;
            for (ProcessInfo &i : processes){
                if (i.path == filePath){
                    i.memoryUsage += memory_usage;
                    i.cpuUsage += total_time;
                    unique = false;
                    break;
                }
            }

            if (unique){
                info.cpuUsage = total_time;
                info.memoryUsage = memory_usage;
                processes.push_back(info);
            }

            CloseHandle(hProcess);
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
