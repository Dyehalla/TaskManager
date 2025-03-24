//Хэндл — это абстракция, представляющая собой уникальный идентификатор, который позволяет программе взаимодействовать с определённым ресурсом.
//Хэндлы используются для отслеживания и управления объектами в системе, например можно открыть файл и получить хэндл для него,
//который затем можно использовать для чтения из файла, записи в него и закрытия.

//Дескриптор — это целочисленный идентификатор, который указывает на открытый ресурс, такой как файл, процесс или поток.
//Дескрипторы позволяют операционной системе отслеживать состояние и свойства ресурсов, которые использует приложение.

//Таким образом, основное различие между ними обычно заключается в том, что хэндл используется как ссылочный идентификатор для объектов,
//тогда как дескриптор может относиться как к идентификатору, так и к более сложной структуре, содержащей информацию о состоянии и свойствах ресурса.


#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

using namespace std;

// менялка кодировки
wstring ansiToUtf16(const string& ansiString) {
    int len = MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, nullptr, 0);
    if (len == 0) {
        return L"";
    }
    wstring utf16String(len - 1, L'\0');
    MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, &utf16String[0], len);
    return utf16String;
}

struct ProcessInfo //структура = класс с открытыми полями
{
    DWORD name;
    wstring path;
    long memoryUsage{};
    double cpuUsage{};
};

// Функция для получения списка процессов с инфой о них.
vector<ProcessInfo> GetProcessList() {
    vector<ProcessInfo> processes;

    // Создаем снимок всех процессов. TH32CS_SNAPPROCESS указывает, что нужен снимок процессов. 0 - получаем информацию о всех процессах.
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    //PROCESSENTRY32 – это структура, которая содержит информацию о процессе. Она используется для получения сведений о запущенных процессах.
    PROCESSENTRY32 pe32;
    // Устанавливаем размер структуры в байтах, иначе всё сломается.
    pe32.dwSize = sizeof(PROCESSENTRY32);
    // Получаем информацию о первом процессе в снимке
    if (Process32First(hSnapshot, &pe32)) {
        // Цикл по всем процессам в снимке
        do {
            ProcessInfo info;
            //Копируем имя процесса с помощью szExeFile.
            info.name = wstring(ansiToUtf16(pe32.szExeFile));
            // Открываем процесс для доступа к информации:
            // PROCESS_QUERY_INFORMATION для запроса информации, PROCESS_VM_READ для чтения памяти,
            //False чтобы дочерние процессы не наследовали текущий дескриптор, pe32.th32ProcessID идентификатор процесса.
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
            wchar_t filePath[MAX_PATH]; // Буфер для пути к файлу.
            if (GetModuleFileNameExW(hProcess, NULL, filePath, MAX_PATH)) {
            // Получаем полный путь к исполняемому файлу процесса.
                info.path = filePath;
                PROCESS_MEMORY_COUNTERS_EX pmc;
                if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
                    info.memoryUsage = pmc.PrivateUsage;
                }
                // Получение использования CPU
                FILETIME creationTime, exitTime, kernelTime, userTime;
                ULARGE_INTEGER kernelTimeStart, userTimeStart, kernelTimeEnd, userTimeEnd;
                // Получаем текущее системное время (не используется, но необходимо для GetProcessTimes)
                GetSystemTimeAsFileTime(&creationTime);
                // Получаем время работы процесса в режиме ядра и пользователя
                GetProcessTimes(hProcess, &creationTime, &exitTime, reinterpret_cast<LPFILETIME>(&kernelTimeStart),
                    reinterpret_cast<LPFILETIME>(&userTimeStart));
                Sleep(100);
                // Ждем 100 мс и получаем время работы процесса снова
                GetProcessTimes(hProcess, &creationTime, &exitTime, reinterpret_cast<LPFILETIME>(&kernelTimeEnd),
                    reinterpret_cast<LPFILETIME>(&userTimeEnd));
                // Вычисляем разницу во времени: нужно чтобы более точно измерить время которое процесс тратит на выполнение задач,
                // чтобы проанализировать его производительность
                ULONGLONG kernelTimeDiff = kernelTimeEnd.QuadPart - kernelTimeStart.QuadPart;
                ULONGLONG userTimeDiff = userTimeEnd.QuadPart - userTimeStart.QuadPart;
                ULONGLONG totalTimeDiff = kernelTimeDiff + userTimeDiff;
                // Структура для информации о системе
                SYSTEM_INFO sysinfo;
                GetSystemInfo(&sysinfo);
                int numProcessors = sysinfo.dwNumberOfProcessors;
                // Вычисляем использование CPU (приблизительно)
                info.cpuUsage = (double)totalTimeDiff / 10000.0 / numProcessors;
                // Закрываем хэндл процесса
                CloseHandle(hProcess);
                processes.push_back(info);
            }
        } while (Process32Next(hSnapshot, &pe32)); // переходим к следующему процессу
    }
    // Закрываем хэндл снимка процессов
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



int main() {
    vector<ProcessInfo> processes = GetProcessList();
    for (const auto& process : processes) {
        wcout << L"Имя: " << process.name << endl;
        wcout << L"Пусть: " << process.path << endl;
        wcout << L"Память: " << process.memoryUsage << endl;
        wcout << L"CPU: " << process.cpuUsage << L"%" << endl;
        wcout << endl;
    }
    return 0;
}