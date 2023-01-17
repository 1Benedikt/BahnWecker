#pragma once
#define FD_SETSIZE 255
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "NetzwerkHeader.h"
#include <cstdint>
#include <iphlpapi.h>
#include <sstream>
#include <algorithm>
#include <thread>
#include <fstream>

extern SOCKET SocketToServer;

namespace Client {

    int Connect(std::string IP);

    void Disconnect();

    std::string GetCurrentXMLSettings();

    std::string PutCurrentXMLSettings(std::string);

    bool Send(const char* Request,  SOCKET sock = 0);

    int netbios(std::string, std::string, int);

    bool ICMP(std::string, sockaddr_in&); //Vielleicht upgraden und direkt NETBIOS ansprechen statt erst zu pingen und dann getnameinfo

    std::vector<sockaddr_in> TestICMP(std::string IPAdress);

    std::vector<sockaddr_in> SuperICMP(std::string); //Mit eigenem Netbios verbessern statt gethostname

    int name_mangle(char* In, char* Out, char name_type);

    uint16_t calculate_checksum(ICMPHeader* header, int length);

    std::string getMacAdress(std::string IP);

    std::string UlongMACToString(ULONG* MAC);

    std::string GetDeviceVendorFromMac(std::string MAC);  //Algorithmus verbessern der nach Namen sucht, sollte relativ einfach sein -> Line speichern ab welcher 00, 10, 20, ... FC anfängt.

    std::string GetDeviceName(std::string IP);

    bool InitIPHeader(FullHeader& header, std::string IP, int Protokoll = 1);

    bool PreparePacketForSend(FullHeader& header, std::string IP);

    std::string GetLocalHostIP();

}