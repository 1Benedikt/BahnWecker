#include "Client.h"

SOCKET SocketToServer = INVALID_SOCKET;
bool isConnected = false;
namespace Client
{
    int Connect(std::string IP)
    {
        if (IP == "")
        {
            std::cout << "[-] Leere IP-Adresse\n";
            return -1;
        }

        int iResult;
        struct addrinfo* result = NULL, hints;
        ZeroMemory(&hints, sizeof(hints));

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_family = AF_INET;

        iResult = getaddrinfo(IP.c_str(), "8888", &hints, &result);

        if (iResult != 0)
        {
            std::cout << "[-] getaddrinfo failed: " << iResult << "\n";
            return -1;
        }



        SocketToServer = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        std::cout << "[+] Socket erstellt " << SocketToServer << "\n";
        if (SocketToServer == INVALID_SOCKET)
        {

            std::cout << "[-] Socket could not be created:  " << WSAGetLastError() << "\n";
            freeaddrinfo(result);
            return -1;
        }

        iResult = connect(SocketToServer, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(SocketToServer);
            SocketToServer = INVALID_SOCKET;
            freeaddrinfo(result);
            return -1;
        }

        if (SocketToServer == INVALID_SOCKET)
        {
            std::cout << "[-] Cannot connect to Raspberyy \n";
            return -1;
        }
        freeaddrinfo(result);
        isConnected = true;
        return SocketToServer;

    }

    void Disconnect()
    {
        std::cout << "[+] Socket Descriptor " << SocketToServer << "\n";
        isConnected = false;
        closesocket(SocketToServer);
        std::cout << "[+] Socket Descriptor " << SocketToServer << "\n";
    }

    //https://dzone.com/articles/fun-with-netbios-name-service-and-computer-browser
    void encodeNetBiosChar(
        char* buffer,
        char c
    )
    {
        buffer[0] = 'A' + (c >> 4);
        buffer[1] = 'A' + (c & 0x0f);
    }
    //https://dzone.com/articles/fun-with-netbios-name-service-and-computer-browser
   void  encodeNetBiosName(
        char* buffer,
        const char* name,
        char paddingChar = ' ',
        char typeSuffixChar = 0
    )
    {
        char* typeSuffix = buffer + 30;

        while (buffer < typeSuffix)
        {
            unsigned char c = *name++;
            if (!c)
            {
                while (buffer < typeSuffix)
                {
                    encodeNetBiosChar(buffer, paddingChar);
                    buffer += 2;
                }

                break;
            }

            encodeNetBiosChar(buffer, toupper(c));
            buffer += 2;
        }

        encodeNetBiosChar(buffer, typeSuffixChar);
    }


    int netbios(std::string IP, std::string Port, int ai_family)
    {
        int iResult;
        WSADATA wsaData;
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

        SOCKET NetBiosSocket = socket(AF_INET, SOCK_DGRAM , IPPROTO_UDP);
        if (NetBiosSocket == INVALID_SOCKET)
        {
            std::cout << "[-] Socket konnte nicht erstellt werden\n";
        }

        sockaddr_in Name, Server;
        Name.sin_addr.s_addr = inet_addr(IP.c_str());
        Name.sin_family = AF_INET;
        Name.sin_port = htons(137);

        Server.sin_addr.s_addr = inet_addr("192.168.200.145");
        Server.sin_port = htons(8888);
        Server.sin_family = AF_INET;

        iResult = bind(NetBiosSocket, (sockaddr*) & Server, sizeof(sockaddr_in));
        std::cout << iResult <<  "   " << WSAGetLastError() << "\n";
        if (iResult)
        {
            std::cout << "[-] Verbindung konnte nicht aufgebaut werden\n";
        }
      //  listen(NetBiosSocket, 1);

        NetBiosNodeRequest Request;

        Request.Header.NAME_TRN_ID = 0x2320;
        Request.Header.OPNMRC = htons(0xFFFF & 0b0000000000010000);
        Request.Header.QDCOUNT = htons(1); // NACHHER ADDEN
        Request.Header.ANCOUNT = 0; // NACHHER ADDEN
        Request.Header.NSCOUNT = 0; // NACHHER ADDEN
        Request.Header.ARCOUNT = 0; // NACHHER ADDEN
        ZeroMemory(&Request.Query.QUESTION_NAME, 34);
        
        encodeNetBiosName((char*)&Request.Query.QUESTION_NAME, "Windows");
        Request.Query.QUESTION_NAME[0] = 0x20;
        Request.Query.QUESTION_NAME[32] = 0x41;
        Request.Query.QUESTION_TYPE = htons(0x0021);
        Request.Query.QUESTION_CLASS = htons(0x0001);
    
        char Response[1000];
        iResult = connect(NetBiosSocket, (sockaddr*)&Name, sizeof(sockaddr_in));
        
        if (iResult)
        {
            std::cout << "[-] Error connect";
        }
        char cx = 0;
        for (int i = 0; i < 5; ++i)
        {

            
            if (send(NetBiosSocket, (const char*)&Request, sizeof(NetBiosNodeRequest), 0))
            {
                std::cout << "[+] Packet send\n";
            }
            cx++;
        }
   
        int sizesock = sizeof(sockaddr_in);
        if (recvfrom(NetBiosSocket, Response, 1000, 0, (sockaddr*)&Name, &sizesock) > 0)
        { 
            std::cout << "[+] Erfolgreich " << Response << "\n";
        }

        std::cout << WSAGetLastError();     
        WSACleanup();

        return 0;

    }


    
        
        bool ICMP(std::string IPAdress, sockaddr_in & Ziel)
        {

            int optvar = 1;
            WSADATA wsaData;

            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cout << "[-] WinSock konnte nicht gestartet werden\n";
            }

            SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
            if (sock == INVALID_SOCKET)
            {
                std::cout << "[-] Socket konnte nicht erstellt werden\n";
                if (WSAGetLastError() == 10013) {
                    std::cout << "[-] Programm muss als Administrator ausgeführt werden, da RAW-Sockets verwendet werden.\n";
                }
            }

            if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (const char*)&optvar, sizeof(int)) == SOCKET_ERROR)
            {
                std::cout << "[-] Error with setting socket to RAW\n";
            }

            struct timeval tv;
            tv.tv_sec = 200;
            tv.tv_usec = 0; //Sekunden sind hier Millisekunden https://forums.codeguru.com/showthread.php?353217-example-of-SO_RCVTIMEO-using-setsockopt() nach 2h Testen 

            if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval)) == SOCKET_ERROR)
            {
                //  std::cout << "[-] Could not set timeout\n";
            }

            Ziel.sin_addr.s_addr = inet_addr(IPAdress.c_str());
            Ziel.sin_family = AF_INET;

            FullHeader header;
            header.IPH.TotalLength = htons(sizeof(header));
            header.IPH.Identification = 4324;
            header.IPH.FlagsAndFragment = 0;
            header.IPH.CheckSum = 0;
            header.IPH.Protocol = 1;
            header.IPH.Destination = inet_addr(IPAdress.c_str());
            header.IPH.Source = inet_addr("192.168.200.145");
            header.ICMPH.CheckSum = calculate_checksum(&header.ICMPH, sizeof(header.ICMPH));

            char Response[512];
            int sizesock = sizeof(sockaddr);

            int iResult = sendto(sock, (const char*)&header, sizeof(header), 0, (sockaddr*)&Ziel, sizeof(sockaddr));

            if (iResult == SOCKET_ERROR) {
                std::cout << "[-] Could not sent packet: " << WSAGetLastError() << "\n";
            }
            //std::cout << "[+] Send packet\n";

            std::cout << IPAdress << "\n";
            // Sleep(1000);
            iResult = recvfrom(sock, Response, 512, 0, (sockaddr*)&Ziel, &sizesock);
            if (iResult > 0)
            {
                if (Response[IPHEADER_LENGTH + 1] == ICMP_REPLY)
                {
                    return true;
                }
            }
            else if (iResult == -1) {

            }

            return false;



        }
//Generiert mit ChatGPT
        uint16_t calculate_checksum(ICMPHeader* header, int length) {
            uint32_t sum = 0;
            uint16_t* ptr = (uint16_t*)header;
            for (int i = 0; i < length / 2; i++) {
                sum += *ptr;
                ptr++;
            }
            // if the length of the header is odd, add the last byte
            if (length & 1) {
                sum += *((uint8_t*)ptr);
            }
            // fold the sum to 16 bits
            while (sum >> 16) {
                sum = (sum & 0xFFFF) + (sum >> 16);
            }
            return (uint16_t)~sum;
        }

        std::vector<sockaddr_in> SuperICMP(std::string IPAdress)
        {
            int optvar = 1;
            WSADATA wsaData;
            std::vector<SOCKET> SocketListDevices;
            std::vector<sockaddr_in> ListDevices;
            std::vector<sockaddr_in> ListDevicesTemp;
           
            sockaddr_in Ziel;
            int iResult;
            
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cout << "[-] WinSock konnte nicht gestartet werden\n";
            }
            
            FullHeader header;
            header.IPH.TotalLength = htons(sizeof(header));
            header.IPH.Identification = 4324;
            header.IPH.FlagsAndFragment = 0;
            header.IPH.CheckSum = 0;
            header.IPH.Protocol = 1;
    
            header.IPH.Source = inet_addr("192.168.200.145");
            header.ICMPH.CheckSum = calculate_checksum(&header.ICMPH, sizeof(header.ICMPH));
    

            for (int i = 0; i < 255; ++i)
            {   
                SocketListDevices.push_back(socket(AF_INET, SOCK_RAW, IPPROTO_ICMP));
                header.IPH.Destination = inet_addr(IPAdress.c_str());
                header.ICMPH.CheckSum = calculate_checksum(&header.ICMPH, sizeof(header.ICMPH));
              //  SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
               
                if (SocketListDevices[i] == INVALID_SOCKET)
                {
                    std::cout << "[-] Socket konnte nicht erstellt werden\n";
                    if (WSAGetLastError() == 10013) {
                        std::cout << "[-] Programm muss als Administrator ausgeführt werden, da RAW-Sockets verwendet werden.\n";
                    }
                }

                if (setsockopt(SocketListDevices[i], IPPROTO_IP, IP_HDRINCL, (const char*)&optvar, sizeof(int)) == SOCKET_ERROR)
                {
                    std::cout << "[-] Error with setting socket to RAW\n";
                }
                IPAdress.replace(12, 14, std::to_string(i));

                Ziel.sin_addr.s_addr = inet_addr(IPAdress.c_str());
                Ziel.sin_family = AF_INET;
                ListDevicesTemp.push_back(Ziel);
                FullHeader header;
                header.IPH.TotalLength = htons(sizeof(header));
                header.IPH.Identification = 4324;
                header.IPH.FlagsAndFragment = 0;
                header.IPH.CheckSum = 0;
                header.IPH.Protocol = 1;
                header.IPH.Destination = inet_addr(IPAdress.c_str());
                header.IPH.Source = inet_addr("192.168.200.145");
                header.ICMPH.CheckSum = calculate_checksum(&header.ICMPH, sizeof(header.ICMPH));

               

                iResult = sendto(SocketListDevices[i], (const char*)&header, sizeof(header), 0, (sockaddr*)&Ziel, sizeof(sockaddr));

                if (iResult == SOCKET_ERROR) {
                    std::cout << "[-] Could not sent packet: " << WSAGetLastError() << "\n";
                }

            }
            Sleep(500);
            char Response[10000];
            memset(Response, 0, 100);
            int sizesock = sizeof(sockaddr);
            for (int i = 1; i < 255; ++i)
            {
               
                IPAdress.replace(12, 14, std::to_string(i));
                std::cout << IPAdress << "\n";
                Ziel.sin_addr.s_addr = inet_addr(IPAdress.c_str());
                Ziel.sin_family = AF_INET;
                iResult = recvfrom(SocketListDevices[i], Response, 1000, 0, (sockaddr*)&ListDevicesTemp[i], &sizesock);
              
                if (iResult > 0)
                {
                   
                    if ((Response[IPHEADER_LENGTH + 1] == ICMP_REPLY) && (Response[0] != 0))
                    {
                        std::cout << "DRINNEN";
                        ListDevices.push_back(Ziel);
                        memset(Response, 0, 100);
                    }
                }
                else if (iResult == -1) {

                }
            }
            
            return ListDevices;

        }

        
        bool InitIPHeader(FullHeader& header, std::string IP, int Protokoll)
        {
            header.IPH.TotalLength = htons(sizeof(header));
            header.IPH.Identification = 4324; //Einfach eine beliebige Nummer
            header.IPH.FlagsAndFragment = 0;
            header.IPH.CheckSum = 0;
            header.IPH.Protocol = Protokoll; //Standardmäßig ICMP;
            header.IPH.Source = inet_addr(IP.data());

            return true;
        }

        bool PreparePacketForSend(FullHeader& header, std::string IP)
        {
            header.ICMPH.CheckSum = 0; //Sicherstellen dass die Checksumme 0 ist, damit diese richtig berechnet werden kann
            header.IPH.Destination = inet_addr(IP.c_str());
            header.ICMPH.CheckSum = calculate_checksum(&header.ICMPH, sizeof(header.ICMPH));
        
            return true;
        }

        std::vector<sockaddr_in> TestICMP(std::string IPAdress)
        {
            std::vector<sockaddr_in> Devices;
            std::vector<sockaddr_in> ReturnDevices;
            std::vector<sockaddr_in> DevicesCheckAgain;
            int optvar = 1;
            sockaddr_in Ziel;
            int iResult;
        
            FullHeader header;
            InitIPHeader(header, GetLocalHostIP());
            u_long iMode = 0;
        
            SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

            if (sock == INVALID_SOCKET)
            {
                if (WSAGetLastError() == 10013) {
                    std::cout << "[-] Programm muss als Administrator ausgeführt werden, da RAW-Sockets verwendet werden.\n";
                }
                else
                {
                    std::cout << "[-] Socket konnte nicht erstellt werden" << WSAGetLastError() << "\n";
                }
            }

            if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (const char*)&optvar, sizeof(int)) == SOCKET_ERROR)
            {
                std::cout << "[-] Error with setting socket to RAW\n";
            }

            ioctlsocket(sock, FIONBIO, &iMode);
            struct timeval tv;
            tv.tv_sec = 10;
            tv.tv_usec = 0; //Sekunden sind hier Millisekunden https://forums.codeguru.com/showthread.php?353217-example-of-SO_RCVTIMEO-using-setsockopt() nach 2h Testen 

            if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval)) == SOCKET_ERROR)
            {
                  std::cout << "[-] Timeout kann nicht erstellt werden\n";
            }

            int LetztesOktett;
            for (int i = 0; i < 255; ++i)
            {
                //IPAdress.replace(12, 14, std::to_string(i));
                LetztesOktett = IPAdress.find_last_of(".") + 1;
                IPAdress.replace(LetztesOktett, LetztesOktett+2, std::to_string(i));
                PreparePacketForSend(header, IPAdress);

                Ziel.sin_addr.s_addr = inet_addr(IPAdress.c_str());
                Ziel.sin_family = AF_INET;
                Devices.push_back(Ziel);
                
                iResult = sendto(sock, (const char*)&header, sizeof(header), 0, (sockaddr*)&Ziel, sizeof(sockaddr));
               
                if (iResult == SOCKET_ERROR) {
                    std::cout << "[-] Packet konnte nicht gesendet werden: " << WSAGetLastError() << "\n";
                }

              
            }
        
            Sleep(1000);
            char ResponseA[1000];
            char Buf[16];
            int sizesock = sizeof(sockaddr);
            
            for(int j = 0; j < 255; ++j)
            {   
                iResult = recvfrom(sock, ResponseA, 1000, 0, (sockaddr*)(&Devices[j]), &sizesock);
                inet_ntop(AF_INET, (void*)(&Devices[j].sin_addr.s_addr), Buf, 16); //IPadresse von sin_addr.s_addr in lesbare ASCII Zeichen verwandeln

                std::cout << iResult << "  " << Buf << " \n";
                if (iResult > 0)
                {
                    if (ResponseA[IPHEADER_LENGTH + 1] == ICMP_REPLY) //Überprüfen ob Reply eine ICMP_REPLY ist
                    {
                        ReturnDevices.push_back(Devices[j]);
                  
                        ResponseA[IPHEADER_LENGTH + 1] == 0xFF; //Im Response Buffer, das Byte welches anzeigt was es für eine Nachrichtentyp ist zurücksetzen
                    }
                }
            }
            std::cout << ReturnDevices.size() << "\n";
            closesocket(sock);
            return ReturnDevices;
        }


        bool Send(const char* Request, SOCKET sock)
        {
            std::cout << "SOCK: " << sock << "\n";
            if (sock == 0)
            {
                if (!SocketToServer)
                {
                    sock = Connect("192.168.200.124");
                }
                else
                {
                    sock = SocketToServer;
                }
               
                std::cout << "SOCK: " << sock << "\n";
                if (!sock)
                {
                    std::cout << "[-] Socket ist nicht geoeffnet\n";
                    return false;
                }
                
            }
            int MessageLength = strlen(Request);
            int iResult = send(sock, Request, MessageLength, 0);

            if (iResult > 0)
            {
                std::cout << "[+] Nachricht wurde gesendet\n";
                return true;
            }

        }

        std::string GetCurrentXMLSettings()
        {
            SOCKET sock = INVALID_SOCKET;
            if(SocketToServer == INVALID_SOCKET)
            {
                sock = Connect("");
            }
            else
            {
                sock = SocketToServer;
            }

            char Request[] = { "GET TestPut HTTP/1.1\r\n\r\n" };

            Send(Request, sock);

            char Respomse[512];

            recv(sock, Respomse, 512, 0);

            std::cout << Respomse << "\n";
            return Respomse;

        }

        std::string PutCurrentXMLSettings(std::string Content)
        {
            SOCKET sock = INVALID_SOCKET;
            if (SocketToServer == INVALID_SOCKET)
            {
                sock = Connect("");
            }
            else
            {
                sock = SocketToServer;
            }

            char Request[] = { "PUT TestPut HTTP/1.1\r\nContent-Length: 9\r\n\r\n123456789" };
            std::string Con = "PUT TestPut HTTP/1.1\r\nContent-Length: ";
            Con.append(std::to_string(Content.size()));
            Con.append("\r\n\r\n");
            Con.append(Content);
            std::cout << "SEND\n";
            Send(Con.data(), sock);
            std::cout << Con << "  " << Con.size();
            char Response[512];

            recv(sock, Response, 512, 0);

            std::cout << Response << "\n";
            return Response;
        }

        std::string UlongMACToString(ULONG* MAC) //MACs werte im RAM sehen ca so aus in Hex schreibweise: B8 27 EB 96 D4
        {
            std::string ReturnMac;
            std::stringstream str;
            uint8_t Adr[6];
            memcpy(Adr, MAC, 6);
            for (int i = 0; i < 6; ++i)
            {
                int j = (int)Adr[i];        //Hex Wert wird auch wirklich als Hex interpretiert, benutzt man char oder uint8_t (typedef unsigned char)
                if (j < 10)
                {
                    str << 0;
                }
                str << std::hex << (&j)[0]; //Integer wid in stringstream geschrieben (aussehen ca. ffffffB8)

            }
            
            ReturnMac = str.str(); //Vollständiger String ffffffB8ffffff27ffffffEBffffff96ffffffD4
            std::cout << ReturnMac << "\n";
            for (int i = 2; i < 15; i += 3)
            {
                ReturnMac.insert(i, ":");
            }
            
            std::transform(ReturnMac.begin(), ReturnMac.end(), ReturnMac.begin(), std::toupper);
            return ReturnMac;
        }

        std::string getMacAdress(std::string IP)
        {
            ULONG Macadresses[2];
            ULONG size = 6;
            int8_t Adr[6];
     

            std::string MAC;
            if (SendARP(inet_addr(IP.c_str()), inet_addr(GetLocalHostIP().c_str()), Macadresses, &size) == NO_ERROR)
            {
                memcpy(Adr, Macadresses, 6);
            }
            else
            {
                return "";
            }

            MAC = UlongMACToString(Macadresses);
            std::cout << MAC;
            return MAC;

        }

        std::string GetDeviceVendorFromMac(std::string MAC)
        {
            constexpr const char* RaspberryPIMac[6] = { "28:CD:C1","3A:35:41","B8:27:EB", "D8:3A:DD", "DC:A6:32", "E4:5F:01" }; //MAC Adressen von https://gitlab.com/wireshark/wireshark/-/raw/master/manuf
            std::string ReturnVendor;
            char c;
            std::string FullLine;

            for (int i = 0; i < 6; ++i)
            {
                if (strncmp(RaspberryPIMac[i], MAC.data(), 6) == 0)
                {
                    ReturnVendor = "Raspberry Pi";
                    return ReturnVendor;
                }
            }

            std::ifstream DeviceVendorList;
            DeviceVendorList.open("../JugendForschtBahn/DeviceVendorMAC.txt", std::ios::in);

            if (DeviceVendorList.is_open())
            {
                while (!DeviceVendorList.eof())
                {
                    std::getline(DeviceVendorList, FullLine);
                   
                    if (FullLine[0] == MAC[0])
                    {
                    
                        if (strncmp(FullLine.data(), MAC.data(), 8) == 0)
                        {
                            size_t t = FullLine.find_last_of("\t");
                            ReturnVendor = FullLine.substr(t+1);
                            std::cout << ReturnVendor << "\n";
                            DeviceVendorList.close();
                            return ReturnVendor;
                        }
                    }
                }
            }
            else
            {
                std::cout << "[-] VendorList Datei konnte nicht geöffnet werden\n";
                return "";
            }
            std::cout << "[-] Keinen Vendor zur MAC Adresse: " << MAC << " gefunden\n";
            DeviceVendorList.close();
            return "";
        }

        std::string GetDeviceName(std::string IP)
        {
            std::string MAC = getMacAdress(IP);
            return GetDeviceVendorFromMac(MAC);
        }

#define WLAN_INTERFACE_NUMBER 9
        std::string GetLocalHostIP()
        {
            //Warum gibt es nicht einfach eine GetLocalhostWlanInterface Funktion
            static std::string IPAdresse;

            if (!IPAdresse.empty())
            {
                return IPAdresse;
            }
            IP_ADAPTER_ADDRESSES_LH AdapterLinkedList[100];
            PIP_ADAPTER_ADDRESSES AdapterAdress = NULL;
            PIP_ADAPTER_UNICAST_ADDRESS AdapterAdressUnicast = NULL;
            ULONG sizeIP = sizeof(AdapterLinkedList);
            int iResult = GetAdaptersAddresses(AF_INET, 0, 0, AdapterLinkedList, &sizeIP);

            if(iResult == NOERROR){}
            else if (iResult == ERROR_NO_DATA)
            {
                std::cout << "[-] Keine Interfaces gefunden -> Keine IP gefunden\n";
                return "";

            }
            else
            {
                std::cout << "[-] Fehler beim bekommen der Localhost IP-Adresse" << WSAGetLastError() << "\n";
                return "";
            }

            AdapterAdress = AdapterLinkedList;

            while (AdapterAdress)
            {
                AdapterAdressUnicast = AdapterAdress->FirstUnicastAddress;
                char tempC[30];
                sockaddr_in* is = (sockaddr_in*)(AdapterAdressUnicast->Address.lpSockaddr);
                inet_ntop(AF_INET, &is->sin_addr.s_addr, tempC, 30);
                if (AdapterAdress->IfIndex == WLAN_INTERFACE_NUMBER)
                {

                    std::cout << tempC << "\n";
                    IPAdresse = std::string(tempC);
                    return IPAdresse;

                }

                AdapterAdress = AdapterAdress->Next;
            }
            return "";
        }

}

