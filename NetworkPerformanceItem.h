#pragma once
#include <string>
#include <windows.h>
#include <vector>

class NetworkPerformanceItem
{
public:

    INT ProcessId;
    INT State;
    std::wstring ExeName;
    std::wstring ExePath;
    std::string LocalAddress;
    std::string RemoteAddress;
    int LocalPort;
    int RemotePort;
    LONG BytesOut;
    LONG BytesIn;
    ULONG64 OutboundBandwidth;
    ULONG64 InboundBandwidth;

};

std::vector<NetworkPerformanceItem> get_networks_list();
void bubble_sort_net(std::vector<NetworkPerformanceItem> &vector);
