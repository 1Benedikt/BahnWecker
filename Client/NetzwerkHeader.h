#pragma once
#include <stdint.h>
struct IPHeader {

	uint8_t VersionAndLength = 0x45;
	uint8_t TOS = 0;//	0x48; // 0100 1000 Bei Precedence Immediate, dass heißt schneller und Relibility Bit gesetzt.
	uint16_t TotalLength = 20; //20 Bytes für die Header und 2 für die Nachricht
	uint16_t Identification = 10001; // Irgendein Wert, für meine Vewendung irrelevant
	uint16_t FlagsAndFragment = 0x0; //Irrelevant für ICMP
	uint8_t TimeToLive = 128; 
	uint8_t Protocol = 1; //ICMP
	uint16_t CheckSum = 0; //ICMP Pakte beim Ping Befehl vom CMD zeigen in Wireshark eine Checksum von 0 an, aufgrund der Einfachheit halber mache ich es auch so
	uint32_t Source;
	uint32_t Destination;
	// char messgae[16] = {"ABCEDFGHIJKLMOP"};
};

#define IPHEADER_LENGTH 20
//https://www.rfc-editor.org/rfc/rfc791#section-3.1 

/*  0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |Version|  IHL  |Type of Service|          Total Length         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Identification        |Flags|      Fragment Offset    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Time to Live |    Protocol   |         Header Checksum       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                       Source Address                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Destination Address                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Options                    |    Padding    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

					Example Internet Datagram Header*/



struct ICMPHeader
{

	uint8_t Type = 8;
	uint8_t code = 0;
	uint16_t CheckSum = 0;
	uint16_t Identifier = 1;
	uint16_t SequenceNumber = 0;
	char messgae[16] = { "ABCEDFGHIJKLMOP" };

};

#define ICMP_REPLY 0

struct FullHeader
{
	IPHeader IPH;
	ICMPHeader ICMPH;
};

struct NetBiosHeader
{
	//uint16_t TCP_LENGTH;
	uint16_t NAME_TRN_ID;
	uint16_t OPNMRC; //Zusammenfassung von Opcode, NM_FLAGS und RCODE, da Bitfelder nicht ganz funktioniert haben

	uint16_t QDCOUNT;
	uint16_t ANCOUNT;
	uint16_t NSCOUNT;
	uint16_t ARCOUNT;

};

struct NetBiosNodeQuery
{
	char QUESTION_NAME[34];
	uint16_t QUESTION_TYPE;
	uint16_t QUESTION_CLASS;
};
struct NetBiosNodeRequest
{
	NetBiosHeader Header;
	NetBiosNodeQuery Query;
};

//Infos zu den Headern und den Requests alle aus dem RFC https://www.rfc-editor.org/rfc/rfc1002

/*

					   1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         NAME_TRN_ID           |0|  0x0  |0|0|0|0|0 0|B|  0x0  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |          0x0001               |           0x0000              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |          0x0000               |           0x0000              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                                                               |
   /                         QUESTION_NAME                         /
   |                                                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         NBSTAT (0x0021)       |        IN (0x0001)            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+



*/




