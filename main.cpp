#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

using namespace std;

// менялка кодировки
std::wstring ansiToUtf16(const std::string& ansiString) {
    int len = MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, nullptr, 0);
    if (len == 0) {
        return L"";
    }
    std::wstring utf16String(len - 1, L'\0');
    MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, &utf16String[0], len);
    return utf16String;
}

struct ProcessInfo //структура = класс с открытыми полями
{
    wstring name;
    wstring path;
    long memoryUsage{};
    double cpuUsage{};
};


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
                FILETIME creationTime, exitTime, kernelTime, userTime;
                ULARGE_INTEGER kernelTimeStart, userTimeStart, kernelTimeEnd, userTimeEnd;

}
