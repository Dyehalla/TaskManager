#pragma once
#include <string>
#include <windows.h>
#include <vector>

class NetworkPerformanceItem
{
public:

    INT ProcessId;
    INT State;
    std::string ExeName;
    std::string ExePath;
    std::string LocalAddress;
    std::string RemoteAddress;
    int LocalPort;
    int RemotePort;
    LONG BytesOut;
    LONG BytesIn;
    int OutboundBandwidth;
    int InboundBandwidth;

};

std::vector<NetworkPerformanceItem> get_networks_list();
