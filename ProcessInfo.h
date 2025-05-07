#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <string>
#include <vector>

struct ProcessInfo
{
    std::wstring name;
    std::wstring path;
    long memoryUsage{};
    double cpuUsage{};
};

std::vector<ProcessInfo> get_process_list();
int bubbleSort(std::vector<ProcessInfo> &vector);
#endif // PROCESSINFO_H
