#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <vector>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <qDebug>

struct ProcessInfo
{
    std::wstring name;
    std::wstring path;
    long memoryUsage{};
    double cpuUsage{};
    std::vector<DWORD> pid;
};

std::vector<ProcessInfo> get_process_list();
int bubbleSortProc(std::vector<ProcessInfo> &vector, int mode);
BOOL TerminateProcessById(DWORD processId);
QString format_mseconds(long long milliseconds);
#endif // PROCESSINFO_H
