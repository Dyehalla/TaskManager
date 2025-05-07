#pragma once
#include <string>
#include <windows.h>

class NetworkPerformanceItem
{
public:

    INT ProcessId;
    INT State;
    std::string LocalAddress;
    std::string RemoteAddress;
    int LocalPort;
    int RemotePort;
    LONG BytesOut;
    LONG BytesIn;
    LONG OutboundBandwidth;
    LONG InboundBandwidth;
    int Pass = 0;
    std::string CollectionTime;

};
