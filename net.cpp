#define _CRT_SECURE_NO_WARNINGS
#include "NetworkPerformanceScanner.h"
#include "ProcessInfo.h"
#include <QDebug>

std::wstring get_filename_from_path(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        return path.substr(pos + 1);
    }
    return path;
}

int bubbleSortNet(std::vector<NetworkPerformanceItem> &vector)
{
    int size = vector.size();
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (vector[j].InboundBandwidth < vector[j + 1].InboundBandwidth)
                std::swap(vector[j], vector[j + 1]);
        }
    }
    return size;
}

// TODO - implement TCP v6, UDP
std::vector<NetworkPerformanceItem> get_networks_list()
{
    std::vector<unsigned char> buffer;
    DWORD dwSize = sizeof(MIB_TCPTABLE_OWNER_PID);
    DWORD dwRetValue = 0;
    vector<NetworkPerformanceItem> networkPerformanceItems;

    do{
        buffer.resize(dwSize, 0);
        dwRetValue = GetExtendedTcpTable(buffer.data(), &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
    } while (dwRetValue == ERROR_INSUFFICIENT_BUFFER);

    if (dwRetValue == ERROR_SUCCESS)
    {
        PMIB_TCPTABLE_OWNER_PID ptTable = reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(buffer.data());

        for (DWORD i = 0; i < ptTable->dwNumEntries; i++)
        {
            NetworkPerformanceItem networkPerformanceItem;

            // Ищем процесс, получаем имя и путь
            networkPerformanceItem.ProcessId = ptTable->table[i].dwOwningPid;
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ptTable->table[i].dwOwningPid);
            wchar_t path[MAX_PATH] = { 0 };
            if (GetModuleFileNameExW(hProcess, NULL, path, MAX_PATH) == 0) {
                CloseHandle(hProcess);
                // cout << "path not found\n";
                continue;
            }
            networkPerformanceItem.ExePath = path;
            networkPerformanceItem.ExeName = get_filename_from_path(path);

            networkPerformanceItem.State = ptTable->table[i].dwState;

            std::ostringstream localStream;
            localStream << (ptTable->table[i].dwLocalAddr & 0xFF)
                        << "."
                        << ((ptTable->table[i].dwLocalAddr >> 8) & 0xFF)
                        << "."
                        << ((ptTable->table[i].dwLocalAddr >> 16) & 0xFF)
                        << "."
                        << ((ptTable->table[i].dwLocalAddr >> 24) & 0xFF)
                        << ":"
                        << htons((unsigned short)ptTable->table[i].dwLocalPort);
            networkPerformanceItem.LocalAddress = localStream.str();
            networkPerformanceItem.LocalPort = ptTable->table[i].dwLocalPort;

            std::ostringstream remoteStream;
            remoteStream << (ptTable->table[i].dwRemoteAddr & 0xFF)
                         << "."
                         << ((ptTable->table[i].dwRemoteAddr >> 8) & 0xFF)
                         << "."
                         << ((ptTable->table[i].dwRemoteAddr >> 16) & 0xFF)
                         << "."
                         << ((ptTable->table[i].dwRemoteAddr >> 24) & 0xFF)
                         << ":"
                         << htons((unsigned short)ptTable->table[i].dwRemotePort);
            networkPerformanceItem.RemoteAddress = remoteStream.str();
            networkPerformanceItem.RemotePort = ptTable->table[i].dwRemotePort;

            MIB_TCPROW row;
            row.dwLocalAddr = ptTable->table[i].dwLocalAddr;
            row.dwLocalPort = ptTable->table[i].dwLocalPort;
            row.dwRemoteAddr = ptTable->table[i].dwRemoteAddr;
            row.dwRemotePort = ptTable->table[i].dwRemotePort;
            row.dwState = ptTable->table[i].dwState;
            void* processRow = &row;

            if (row.dwRemoteAddr != 0)
            {
                ULONG rosSize = 0, rodSize = 0;
                ULONG winStatus;
                PUCHAR ros = NULL, rod = NULL;
                rodSize = sizeof(TCP_ESTATS_BANDWIDTH_ROD_v0);

                rod = (PUCHAR)malloc(rodSize);
                if (rod == NULL) {
                    free(ros);
                    wprintf(L"\nOut of memory");
                    return networkPerformanceItems;
                }
                else
                    memset(rod, 0, rodSize); // зануляем буфер

                PTCP_ESTATS_BANDWIDTH_ROD_v0 bandwidthRod = { 0 };

                winStatus = GetPerTcpConnectionEStats((PMIB_TCPROW)&row, TcpConnectionEstatsBandwidth, NULL, 0, 0, ros, 0, rosSize, rod, 0, rodSize);
                if (winStatus != NO_ERROR) {
                    free(rod);
                    cout << "what the FUCK is that\n" << endl;
                    continue;
                }

                bandwidthRod = (PTCP_ESTATS_BANDWIDTH_ROD_v0)rod;

                networkPerformanceItem.OutboundBandwidth = bandwidthRod->OutboundBandwidth;
                networkPerformanceItem.InboundBandwidth = bandwidthRod->InboundBandwidth;
                free(rod);
            }
            else {
                networkPerformanceItem.OutboundBandwidth = 0;
                networkPerformanceItem.InboundBandwidth = 0;
            }
            networkPerformanceItems.push_back(networkPerformanceItem);
        }
    }
    else
    {
        // пизда рулям если сюда попали
    }

    bubbleSortNet(networkPerformanceItems);
    return networkPerformanceItems;
}

#define NAME_SORT 0
#define NET_IN_SORT 2
#define NET_OUT_SORT 1
#define LOCAL_IP_SORT 3
#define REMOTE_IP_SORT 4

void bubble_sort_net(std::vector<NetworkPerformanceItem> &vector, int mode)
{
    int size = vector.size();
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            bool cond = false;
            if (mode == NAME_SORT) cond = vector[j].ExeName < vector[j + 1].ExeName;
            else if (mode == NET_IN_SORT ) cond = vector[j].InboundBandwidth < vector[j + 1].InboundBandwidth;
            else if (mode == NET_OUT_SORT) cond = vector[j].OutboundBandwidth < vector[j + 1].OutboundBandwidth;
            else if (mode == LOCAL_IP_SORT) cond = vector[j].LocalAddress < vector[j + 1].LocalAddress;
            else if (mode == REMOTE_IP_SORT) cond = vector[j].RemoteAddress < vector[j + 1].RemoteAddress;
            if (cond) std::swap(vector[j], vector[j + 1]);
        }
    }
}


