#define WIN32_LEAN_AND_MEAN

#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include "NetworkPerformanceItem.h"
#include <iomanip>
#include <iostream>

#include <winsock2.h>
#include <windows.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <Tcpestats.h>
#include <stdlib.h>
#include <stdio.h>

#include "NetworkPerformanceItem.h"

using namespace std;

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

class NetworkPerformanceScanner
{
public:
    NetworkPerformanceScanner();
    ~NetworkPerformanceScanner();

    std::vector<NetworkPerformanceItem> ScanNetworkPerformance(int sampleId);
};

